/* ===========================================================================
** Copyright (C) 2021 Infineon Technologies AG
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

#include "interface/acquisition.h"
#include "interface/report.h"
#include "driver/bgt60.h"

#include "direct.h"
#include "SingleReaderSingleWriterRingBuffer.hpp"
#include <vector>
#include <atomic>
#include <thread>
#include <cstring>

static bool data_integrity_test_enabled = false;
static uint16_t test_mode_shift_register = 0x0001;

static struct {
    size_t header_size;
    uint16_t slice_size;
    uint16_t frame_count;
    std::thread data_thread;

    std::atomic<bool> is_started;
    std::atomic<bool> buffer_overflow;
    std::atomic<bool> fifo_error;
    SingleReaderSingleWriterRingBuffer<std::vector<uint16_t>> frame_buffer;
    const direct_mode_description_t* mode;
} radar;

static bgt60_dev_t bgt60_dev = {
    /* .spi_transfer = */ bgt60_platform_spi_transfer,
    /* .reset = */ bgt60_platform_reset,
    /* .slice_size = */ 0
};

static ifx_Cube_R_t* radar_data_frame = NULL;

static bool get_next_frame_from_buffer(uint16_t* buffer, ifx_Cube_R_t* frame);
static void unpack_raw12(uint8_t* src, uint32_t count, uint16_t* dst);
static uint32_t get_num_samples_per_frame();
static uint32_t get_num_slices_per_frame();
static uint32_t get_num_samples_per_slice();
static uint32_t get_spi_transfer_size();

static void test_mode_lsfr_init()
{
    test_mode_shift_register = 0x0001;
}

static uint16_t test_mode_lsfr_get_next(void)
{
	const uint16_t v = test_mode_shift_register;
	const uint16_t next_value =
        (v >>  1) |
        (((v << 11) ^
          (v << 10) ^
          (v <<  9) ^
          (v <<  3) ) & 0x0800);

    test_mode_shift_register = next_value;
    return v;
}


static uint32_t get_num_samples_per_frame()
{
    return  radar.mode->num_antennas *
        radar.mode->seg_config.num_chirps_per_frame * 
        radar.mode->seg_config.num_samples_per_chirp;
}

static uint32_t get_num_slices_per_frame()
{
    return get_num_samples_per_frame() / (radar.slice_size * 2);
}

static uint32_t get_num_samples_per_slice()
{
    return get_num_samples_per_frame() / get_num_slices_per_frame();
}

static uint32_t get_spi_transfer_size()
{
    return radar.slice_size * 3 + radar.header_size;
}

static void read_frame_data(void)
{
    static uint32_t slice_cnt = 0;
    static uint32_t buffer_idx = 0;
    static uint32_t cnt= 0;

    std::vector<uint8_t> slice_data(get_spi_transfer_size());
    std::vector<uint16_t> frame_buffer(get_num_samples_per_frame());
    const uint32_t num_slices_per_frame = get_num_slices_per_frame();

    for(size_t slice = 0; slice < num_slices_per_frame; slice++) 
    {
        if(bgt60_platform_wait_interrupt() > 0 )
        {
            if (bgt60_get_fifo_data(&bgt60_dev, slice_data.data()) == 0)
            {
                slice_data.data()[1] = slice_cnt & (num_slices_per_frame - 1);
                *(uint16_t *)&slice_data.data()[2] = (slice_cnt / num_slices_per_frame) & 0xFFFF;
                buffer_idx++;
                slice_cnt++;
            }
            else
            {
                rep_err("SPI fifo error\n");
                radar.fifo_error = true;
            }
        }
        unpack_raw12(
            slice_data.data() + radar.header_size,
            get_spi_transfer_size() - radar.header_size,
            frame_buffer.data() + slice * get_num_samples_per_slice());
    }

    if(!radar.frame_buffer.try_push(frame_buffer))
    {
        rep_err("Frame buffer overflow (size: %d fill: %d)\n",
            radar.frame_buffer.size(), radar.frame_buffer.fill());
        radar.buffer_overflow = true;
    }
}

static void spi_data_thread()
{
    uint32_t cnt =0;
    while(radar.is_started)
    {
	    read_frame_data();        
    }
}

static bool radar_fetch_frame(ifx_Cube_R_t* frame)
{
    const uint32_t samples_per_frame = get_num_samples_per_frame();
    std::vector<uint16_t> frame_buffer(samples_per_frame);
    if(radar.is_started)
    {
        radar.frame_buffer.wait_fill(1);
        radar.frame_buffer.try_pop(frame_buffer);
    }
    else
    {
        rep_err("trying to fetch data when the acquisition hasn't been started.\n");
        return false;
    }

    if(data_integrity_test_enabled) {
        for(size_t i = 0; 
            i < samples_per_frame; 
            i += radar.mode->num_antennas) 
        {
            uint16_t expected = test_mode_lsfr_get_next();

            if(frame_buffer[i] != expected)
            {
                rep_err(
                    "error: mismatched spi test word at sample index %u (expected 0x%04x,  got 0x%04x)\n",
                    (unsigned)i,
                    (int)expected,
                    (int)frame_buffer[i]
                );
                return false;
            }
        }

        // set all samples to 0 in test-mode so that the algorithm doesn't process the CRC values
        // this means the integrity test can also be used to test what later stages do with input
        // data consisting only of zeroes.
        std::fill(frame_buffer.begin(), frame_buffer.end(), 0);
    }

    return get_next_frame_from_buffer(frame_buffer.data(), frame);
}

static void unpack_raw12(uint8_t* src, uint32_t count, uint16_t* dst)
{
    for (uint_fast32_t i = 0; i < count; i += 3)
    {
        *(dst++) = (src[i + 0] << 4) | (src[i + 1] >> 4);
        *(dst++) = ((src[i + 1] & 15) << 8) | (src[i + 2]);
    }
}

static bool get_next_frame_from_buffer(uint16_t* buffer, ifx_Cube_R_t* frame)
{
    const uint32_t num_chirps_per_frame = IFX_CUBE_COLS(frame);
    const uint32_t num_samples_per_chirp = IFX_CUBE_SLICES(frame);
    const uint8_t num_rx = IFX_CUBE_ROWS(frame);
    const ifx_Float_t adc_max = 4095; 

    for(size_t rx = 0; rx < num_rx; rx++)
    {
        for (size_t chirp = 0; chirp < num_chirps_per_frame; chirp++)
        {
            const size_t chirp_offset = chirp * num_rx * num_samples_per_chirp;

            for (size_t sample = 0; sample < num_samples_per_chirp; sample++)
            {
                // sample0_RX1 sample0_RX2, sample1_RX1, sample2_RX2, ...
                const size_t sample_offset = sample * num_rx;
                const size_t index = rx + sample_offset + chirp_offset;
                IFX_CUBE_AT(frame, rx, chirp, sample) =
                    buffer[index] / adc_max;
            }
        }
    }

    return true;
}


void direct_device_configure_data_integrity_test(bool enable)
{
    data_integrity_test_enabled = enable;
}

void direct_device_init()
{
    radar.frame_count = 0;
    radar.is_started = false;
    radar.buffer_overflow = false;
    radar.fifo_error = false;
    radar.header_size = 4;
}

void direct_device_deinit()
{
    direct_device_stop();
}

static bool setup_bgt(const direct_mode_description_t *mode)
{
    if(bgt60_init(&bgt60_dev, mode->regs) != 0)
        return false;

    radar.slice_size = bgt60_dev.slice_size;
    radar.frame_count = 0;
    radar.mode = mode;

    rep_msg("Assuming %u slices per frame\n", (unsigned)get_num_slices_per_frame());

    return true;
}

bool direct_device_start(const direct_mode_description_t *mode)
{
    if(radar.is_started != 0) {
        rep_err("acquisition already started.\n");
        return false;
    }

    // get antenna count from configuration
    const uint8_t rx_antenna_count = mode->num_antennas;

    radar_data_frame = ifx_cube_create_r(
        rx_antenna_count,
        mode->seg_config.num_chirps_per_frame,
        mode->seg_config.num_samples_per_chirp);

    if(radar_data_frame == NULL) {
        rep_err("Failed to initialize internal data structure for recording\n");
        return false;
    }

    if(bgt60_platform_init() != 0) {
        rep_err("failed to initialize hw interface to radar device.\n");
        return false;
    }

    if(! setup_bgt(mode) != 0) {
        rep_err("failed to initialize BGT60 driver.\n");
        return false;
    }

    test_mode_lsfr_init();
    if(bgt60_enable_data_test_mode(&bgt60_dev, data_integrity_test_enabled) != 0) {
        rep_err(
            "failed spi test mode to '%s' via BGT60 driver.\n",
            data_integrity_test_enabled ? "true" : "false"
        );
        return false;
    }

    radar.frame_buffer.resize(5, [=](std::vector<uint16_t>& f)
    {
        f.resize(rx_antenna_count*
            mode->seg_config.num_chirps_per_frame*
            mode->seg_config.num_samples_per_chirp);
    });

    if(bgt60_frame_start(&bgt60_dev, true) != 0) {
        rep_err("failed to initialize BGT60 driver.\n");
        return false;
    }

    radar.is_started = true;
    std::thread data_thread(spi_data_thread);
    radar.data_thread = std::move(data_thread);


    return true;
}

void direct_device_stop()
{
    if(radar.is_started)
    {
        radar.is_started = false;
        radar.data_thread.join(); 
        radar.frame_buffer.reset();
        radar.frame_count = 0;
    }

    ifx_cube_destroy_r(radar_data_frame);
    radar_data_frame = NULL;

    bgt60_platform_deinit();
}

bool direct_device_acq_fetch(ifx_Cube_R_t **out)
{
    *out = NULL; // already indicate no more data in case anything fails

    if(!radar_fetch_frame(radar_data_frame))
        return false;

    *out = radar_data_frame;
    return true;
}
