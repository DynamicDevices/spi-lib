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

#include "direct.h"
#include <string.h>

/* Okan

static const uint32_t presence[] =
{
    0x11e8270UL,
    0x3088210UL,
    0x9e967fdUL,
    0xb0805b4UL,
    0xd1027ffUL,
    0xf010700UL,
    0x11000000UL,
    0x13000000UL,
    0x15000000UL,
    0x17000be0UL,
    0x19000000UL,
    0x1b000000UL,
    0x1d000000UL,
    0x1f000b60UL,
    0x2113cc51UL,
    0x236ff41fUL,
    0x25006f7bUL,
    0x2d000490UL,
    0x3b000480UL,
    0x49000480UL,
    0x57000480UL,
    0x5911be0eUL,
    0x5b3ef40aUL,
    0x5d00f000UL,
    0x5f787e1eUL,
    0x61f5208aUL,
    0x630000a4UL,
    0x65000252UL,
    0x67000080UL,
    0x69000000UL,
    0x6b000000UL,
    0x6d000000UL,
    0x6f093910UL,
    0x7f000100UL,
    0x8f000100UL,
    0x9f000100UL,
    0xad000000UL,
    0xb7000000UL,
};
*/

// Gowniyan

static const uint32_t presence[] =
{
    0x11e8270UL,
    0x3088210UL,
    0x9e967fdUL,
    0xb0805b4UL,
    0xd1027ffUL,
    0xf010700UL,
    0x11000000UL,
    0x13000000UL,
    0x15000000UL,
    0x17000be0UL,
    0x19000000UL,
    0x1b000000UL,
    0x1d000000UL,
    0x1f000b60UL,
    0x2113cc51UL,
    0x236ff41fUL,
    0x25006f7bUL,
    0x2d000490UL,
    0x3b000480UL,
    0x49000480UL,
    0x57000480UL,
    0x5911be0eUL,
    0x5b4ccc0aUL,
    0x5d00f000UL,
    0x5f787e1eUL,
    0x61f5208aUL,
    0x630000a4UL,
    0x65000252UL,
    0x67000080UL,
    0x69000000UL,
    0x6b000000UL,
    0x6d000000UL,
    0x6f093910UL,
    0x7f000100UL,
    0x8f000100UL,
    0x9f000100UL,
    0xad000000UL,
    0xb7000000UL,
};

static const uint32_t regs_landscape_460MHz[] =
/* BGT60 register settings for segmentation */
{
    0x011E8270,
    0x030A0210,
    0x09E967FD,
    0x0B0805B4,
    0x0D102FFF,
    0x0F010700,
    0x11000000,
    0x13000000,
    0x15000000,
    0x17000BE0,
    0x19000000,
    0x1B000000,
    0x1D000000,
    0x1F000B60,
    0x21133C51,
    0x235FF41F,
    0x25706F7B,
    0x2D000490,
    0x3B000480,
    0x49000480,
    0x57000480,
    0x5911BE0E,
    0x5B65AC0A,
    0x5D03F000,
    0x5F787E1E,
    0x61F4A65A,
    0x63000099,
    0x650002B2,
    0x67000080,
    0x69000000,
    0x6B000000,
    0x6D000000,
    0x6F253B10,
    0x7F000100,
    0x8F000100,
    0x9F000100,
    0xAD000000,
    0xB7000000,
};

static const uint32_t regs_landscape_1GHz[] =
/* BGT60 register settings for segmentation 1GHz bandwidth */
{ 
    0x11e8270, 
    0x30a0210, 
    0x9e967fd, 
    0xb0805b4, 
    0xd102fff, 
    0xf010700, 
    0x11000000, 
    0x13000000, 
    0x15000000, 
    0x17000be0, 
    0x19000000, 
    0x1b000000, 
    0x1d000000, 
    0x1f000b60, 
    0x21133c51, 
    0x235ff41f, 
    0x25706f7b, 
    0x2d000490, 
    0x3b000480, 
    0x49000480, 
    0x57000480, 
    0x5911be0e, 
    0x5b65ac0a, 
    0x5d03f000, 
    0x5f787e1e, 
    0x61e7cd4a, 
    0x63000131, 
    0x650002b2, 
    0x67000080, 
    0x69000000, 
    0x6b000000, 
    0x6d000000, 
    0x6f253b10, 
    0x7f000100, 
    0x8f000100, 
    0x9f000100, 
    0xab000000, 
    0xad000000, 
    0xb7000000,
    0xFFFFFFFF
};

static const uint32_t regs_utr11[] =
/* BGT60 register settings for segmentation 1GHz bandwidth */
{ 
    0x11c0e20, 
    0x3140210, 
    0x9e967fd, 
    0xb4805b4, 
    0xd1083ff, 
    0x11000000, 
    0x13000000, 
    0x15000000, 
    0x17d0d9e0, 
    0x19000000, 
    0x1b000000, 
    0x1d000000, 
    0x1f000960, 
    0x21003c71, 
    0x2314001f, 
    0x2500000a, 
    0x2d000490, 
    0x3b000480, 
    0x49000480, 
    0x57000480, 
    0x5911be0e, 
    0x5b651c0a, 
    0x5d03f000, 
    0x5fbf3e1e, 
    0x61c1ed32, 
    0x630001d5, 
    0x65030532, 
    0x67000080, 
    0x69000000, 
    0x6b000000, 
    0x6d000000, 
    0x6f261b10, 
    0x7f000100, 
    0x8f000100, 
    0x9f000100, 
    0xa10a0000, 
    0xad000000, 
    0xb7000000, 
    0xbf000400, 
    0xc1000827
};

#define XENSIV_BGT60TRXX_CONF_DEVICE (XENSIV_DEVICE_BGT60TR13C)
#define XENSIV_BGT60TRXX_CONF_START_FREQ_HZ (61020099000)
#define XENSIV_BGT60TRXX_CONF_END_FREQ_HZ (61479903000)
#define XENSIV_BGT60TRXX_CONF_NUM_SAMPLES_PER_CHIRP (128)
#define XENSIV_BGT60TRXX_CONF_NUM_CHIRPS_PER_FRAME (16)
#define XENSIV_BGT60TRXX_CONF_NUM_RX_ANTENNAS (2)
#define XENSIV_BGT60TRXX_CONF_NUM_TX_ANTENNAS (1)
#define XENSIV_BGT60TRXX_CONF_SAMPLE_RATE (2352941)
#define XENSIV_BGT60TRXX_CONF_CHIRP_REPETITION_TIME_S (6.99625e-05)
#define XENSIV_BGT60TRXX_CONF_FRAME_REPETITION_TIME_S (0.00999593)
#define XENSIV_BGT60TRXX_CONF_NUM_REGS (38)


direct_mode_description_t
direct_device_default_mode_table[] =
{
   {
        "presence", "landscape using 460MHz of bandwidth",
        .num_antennas = 2,
        .regs = presence,
        {
            .num_samples_per_chirp = 128,
            .num_chirps_per_frame = 16,
//            .bandwidth_Hz = (ifx_Float_t)460000000u,
//            .center_frequency_Hz = (ifx_Float_t)61250000000u,
            .bandwidth_Hz = (ifx_Float_t)459804000u,
            .center_frequency_Hz = (ifx_Float_t)61250001000u,
            .orientation = IFX_ORIENTATION_LANDSCAPE,
        }
    },
    {
        "landscape", "landscape using 460MHz of bandwidth",
        .num_antennas = 2,
        .regs = regs_landscape_460MHz,
        {
            .num_samples_per_chirp = 128,
            .num_chirps_per_frame = 64,
            .bandwidth_Hz = (ifx_Float_t)460000000u,
            .center_frequency_Hz = (ifx_Float_t)61250000000u,
            .orientation = IFX_ORIENTATION_LANDSCAPE,
        }
    },
    {
        "landscape-1ghz", "landscape using 1GHz of bandwidth",
        .num_antennas = 2,
        .regs = regs_landscape_1GHz,
        {
            .num_samples_per_chirp = 128,
            .num_chirps_per_frame = 64,
            .bandwidth_Hz = (ifx_Float_t)1000000000u,
            .center_frequency_Hz = (ifx_Float_t)61000000000u,
            .orientation = IFX_ORIENTATION_LANDSCAPE,
        }
    },
    {
        "utr11", "landscape using 1GHz of bandwidth",
        .num_antennas = 1,
        .regs = regs_utr11,
        {
            .num_samples_per_chirp = 128,
            .num_chirps_per_frame = 64,
            .bandwidth_Hz = (ifx_Float_t)1000000000u,
            .center_frequency_Hz = (ifx_Float_t)61000000000u,
            .orientation = IFX_ORIENTATION_LANDSCAPE,
        }
    },
};

const direct_mode_description_t*
direct_device_default_mode_find(const char* name)
{
    const direct_mode_description_t *table =
        direct_device_default_mode_table;
    const size_t table_size = 
        sizeof(direct_device_default_mode_table) / sizeof(direct_device_default_mode_table[0]);

    for (size_t l = 0; l < table_size; l++)
    {
        if (strcmp(name, table[l].specifier) == 0) {
            return &table[l];
        }
    }

    return NULL;
}

