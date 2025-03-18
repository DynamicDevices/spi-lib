/* ===========================================================================
** Copyright (C) 2020-2023 Infineon Technologies AG
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

/**
 * @file PresenceSensing.c
 *
 * @brief This file implements the API for presence sensing use case.
 *
 */

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <math.h>    // for NAN
#include <string.h>  // for memset

#include "ifxBase/Error.h"
#include "ifxBase/internal/Util.h"
#include "ifxBase/Mem.h"
#include "ifxRadarDeviceCommon/RadarDeviceCommon.h"

#include "PresenceSensing.h"
#include "xensiv_radar_presence.h"
#include "stdio.h"


/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

/* default values for sensor configuration */
#define IFX_SENSOR_DEFAULT_SAMPLES_PER_CHIRP (128)
#define IFX_SENSOR_DEFAULT_CHIRPS_PER_FRAME  (16)
#define IFX_SENSOR_DEFAULT_START_RF_FREQ_Hz  (61020098000)  // 60GHz ISM band
#define IFX_SENSOR_DEFAULT_END_RF_FREQ_Hz    (61479902000)  // 450MHz bandwidth with some guard band
#define IFX_SENSOR_DEFAULT_SAMPLING_FREQ_Hz  (2330000)
#define IFX_SENSOR_DEFAULT_CHIRP_REP_TIME_s  (7e-05f)
#define IFX_SENSOR_DEFAULT_FRAME_REP_TIME_s  (5e-3f)  // 100Hz
#define IFX_SENSOR_DEFAULT_IF_GAIN_dB        (60)      // 60dB
#define IFX_SENSOR_DEFAULT_TX_POWER_LEVEL    (31)
#define IFX_SENSOR_DEFAULT_HP_CUTOFF_Hz      (80000)
#define IFX_SENSOR_DEFAULT_AAF_CUTOFF_Hz     (500000)  // 500kHz for TR13C, 600kHz for UTR11

/* default values for presence sensing configuration */
#define IFX_PRESENCE_SENSING_DEFAULT_MIN_RANGE_M (0.2f)
#define IFX_PRESENCE_SENSING_DEFAULT_MAX_RANGE_M (4.5f)
#define IFX_PRESENCE_SENSING_DEFAULT_SENSITIVITY (0.5f)  // medium

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/

static struct
{
    bool target_state;      /**< Current presence state.*/
    uint32_t time_stamp_ms; /**< Timestamp tracked by presence algo state. */
    uint32_t range_bin;     /**< Distance of strongest target from sensor in meters. Only valid in presence state. */
} presence_result;

struct ifx_Presence_Sensing_s
{
    ifx_Float_t distance_per_bin;
    uint32_t presence_time_stamp_ms;
    uint32_t frame_period_ms;
    ifx_Float_t* frame_data;
    ifx_Avian_Config_t sensor_config;
    xensiv_radar_presence_handle_t xensiv_handle;
};

/*
==============================================================================
   4. LOCAL DATA
==============================================================================
*/

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

/**< @brief Definition for state status callback */
void presence_detection_cb(xensiv_radar_presence_handle_t handle,
                           const xensiv_radar_presence_event_t* event,
                           void* data);

/*
==============================================================================
   6. LOCAL FUNCTIONS
==============================================================================
*/

static void map_sensitivity_to_thresholds(ifx_Float_t sensitivity, xensiv_radar_presence_config_t* config)
{
    config->micro_fft_size = 128;
    config->micro_fft_decimation_enabled = false;
    config->macro_movement_confirmations = 0;
    config->macro_compare_interval_ms = 250;
    config->micro_movement_compare_idx = 5;

    if (sensitivity <= 0.1f)
    {  // Low Sensitivity
        config->micro_threshold = 100.0f;
        config->macro_threshold = 2.0f;
    }
    else if (sensitivity > 0.1f && sensitivity <= 0.5f)
    {  // Medium Sensitivity
        config->micro_threshold = 50.0f;
        config->macro_threshold = 1.0f;
    }
    else if (sensitivity > 0.5f && sensitivity < 1.0f)
    {  // High Sensitivity
        config->micro_threshold = 25.0f;
        config->macro_threshold = 0.5f;
    }
    else
    {  // Max Sensitivity
    config->micro_fft_size = 256;
    config->micro_fft_decimation_enabled = true;
        config->macro_movement_confirmations = 3;
        config->macro_compare_interval_ms = 50;
        config->micro_movement_compare_idx = 20;
        config->micro_threshold = 20.0f;
        config->macro_threshold = 0.5f;
    }
}

//----------------------------- callback ---------------------------------

void presence_detection_cb(xensiv_radar_presence_handle_t handle,
                           const xensiv_radar_presence_event_t* event,
                           void* data)
{
    (void)handle;
    (void)data;

    presence_result.range_bin = event->range_bin;
    presence_result.time_stamp_ms = event->timestamp;

    switch (event->state)
    {
        case XENSIV_RADAR_PRESENCE_STATE_MACRO_PRESENCE:
        case XENSIV_RADAR_PRESENCE_STATE_MICRO_PRESENCE:

            presence_result.target_state = true;
            break;

        case XENSIV_RADAR_PRESENCE_STATE_ABSENCE:
        default:
            presence_result.target_state = false;
            break;
    }
}

//--------------------------------------------------------------------------------------------

void frame_converter(ifx_Presence_Sensing_t* handle, const ifx_Cube_R_t* frame_data)
{

    IFX_ERR_BRK_NULL(handle);

    uint32_t nrx = ifx_util_popcount(handle->sensor_config.rx_mask);
    uint32_t nchirps = handle->sensor_config.num_chirps_per_frame;
    uint32_t nsamples = handle->sensor_config.num_samples_per_chirp;

    IFX_CUBE_BRK_VALID(frame_data);
    IFX_ERR_BRK_ARGUMENT(IFX_CUBE_ROWS(frame_data) < nrx);
    IFX_ERR_BRK_ARGUMENT(IFX_CUBE_COLS(frame_data) < nchirps);
    IFX_ERR_BRK_ARGUMENT(IFX_CUBE_SLICES(frame_data) < nsamples);

    ifx_Float_t* dst = handle->frame_data;

    if (IFX_CUBE_ROWS(frame_data) == 3 && nrx == 2)
    {
        uint32_t rx = 0;
        for (uint32_t chirp = 0; chirp < nchirps; chirp++)
        {
            for (uint32_t sample = 0; sample < nsamples; sample++)
            {
                *dst++ = IFX_CUBE_AT(frame_data, rx, chirp, sample);
            }
        }
        rx = 2;
        for (uint32_t chirp = 0; chirp < nchirps; chirp++)
        {
            for (uint32_t sample = 0; sample < nsamples; sample++)
            {
                *dst++ = IFX_CUBE_AT(frame_data, rx, chirp, sample);
            }
        }
    }
    else
    {
        uint32_t rx = 0;  // only one Rx enabled

        for (uint32_t chirp = 0; chirp < nchirps; chirp++)
        {
            for (uint32_t sample = 0; sample < nsamples; sample++)
            {
                *dst++ = IFX_CUBE_AT(frame_data, rx, chirp, sample);
            }
        }
    }
}

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

void ifx_presence_sensing_get_config_defaults(ifx_Radar_Sensor_t sensor_type,
                                              ifx_Avian_Config_t* sensor_config,
                                              ifx_Presence_Sensing_Config_t* presence_config)
{
    IFX_ERR_BRK_NULL(sensor_config);
    IFX_ERR_BRK_NULL(presence_config);

    /* default sensor configuration for presence handle */
    if (sensor_type == IFX_AVIAN_BGT60UTR11AIP)
    {
        sensor_config->rx_mask = 1;
        sensor_config->aaf_cutoff_Hz = 600000;
        sensor_config->sample_rate_Hz = (2080000);
        sensor_config->tx_mask = 1;

        sensor_config->if_gain_dB = 55;
        sensor_config->start_frequency_Hz = (61020000000);
        sensor_config->end_frequency_Hz = (61480000000);
    }
    else
    {
        sensor_config->rx_mask = 4;
        sensor_config->aaf_cutoff_Hz = IFX_SENSOR_DEFAULT_AAF_CUTOFF_Hz;
        sensor_config->sample_rate_Hz = IFX_SENSOR_DEFAULT_SAMPLING_FREQ_Hz;
        sensor_config->tx_mask = 1;
        
        sensor_config->if_gain_dB = IFX_SENSOR_DEFAULT_IF_GAIN_dB;
        sensor_config->start_frequency_Hz = IFX_SENSOR_DEFAULT_START_RF_FREQ_Hz;
        sensor_config->end_frequency_Hz = IFX_SENSOR_DEFAULT_END_RF_FREQ_Hz;
    }

    sensor_config->num_samples_per_chirp = IFX_SENSOR_DEFAULT_SAMPLES_PER_CHIRP;
    sensor_config->num_chirps_per_frame = IFX_SENSOR_DEFAULT_CHIRPS_PER_FRAME;
    sensor_config->chirp_repetition_time_s = IFX_SENSOR_DEFAULT_CHIRP_REP_TIME_s;
    sensor_config->frame_repetition_time_s = IFX_SENSOR_DEFAULT_FRAME_REP_TIME_s;
    sensor_config->hp_cutoff_Hz = IFX_SENSOR_DEFAULT_HP_CUTOFF_Hz;
    sensor_config->tx_power_level = IFX_SENSOR_DEFAULT_TX_POWER_LEVEL;
    sensor_config->mimo_mode = IFX_MIMO_OFF;

    /* default presence handle algo configuration */
    presence_config->min_detection_range_m = IFX_PRESENCE_SENSING_DEFAULT_MIN_RANGE_M;
    presence_config->max_detection_range_m = IFX_PRESENCE_SENSING_DEFAULT_MAX_RANGE_M;
    presence_config->sensitivity_threshold = IFX_PRESENCE_SENSING_DEFAULT_SENSITIVITY;
}

//--------------------------------------------------------------------------------------------

ifx_Presence_Sensing_t* ifx_presence_sensing_create(const ifx_Avian_Config_t* sensor_config, const ifx_Presence_Sensing_Config_t* presence_config)
{
    IFX_ERR_BRN_NULL(sensor_config);
    IFX_ERR_BRN_NULL(presence_config);

    ifx_Presence_Sensing_t* h = ifx_mem_calloc(1, sizeof(struct ifx_Presence_Sensing_s));
    IFX_ERR_BRN_MEMALLOC(h);

    h->frame_data = (ifx_Float_t*)ifx_mem_alloc(sensor_config->num_chirps_per_frame * sensor_config->num_samples_per_chirp * sizeof(ifx_Float_t));
    if (h->frame_data == NULL)
    {
        IFX_ERR_BRN_MEMALLOC(h);
    }

    // set alloc and free methods to IFX memory methods
    xensiv_radar_presence_set_malloc_free(ifx_mem_alloc, ifx_mem_free);

    ifx_Float_t bandwidth_Hz = (ifx_Float_t)(sensor_config->end_frequency_Hz - sensor_config->start_frequency_Hz);
    h->distance_per_bin = IFX_LIGHT_SPEED_M_S / (2.0f * bandwidth_Hz);

    h->presence_time_stamp_ms = 0;
    h->frame_period_ms = (uint32_t)(ceilf(sensor_config->frame_repetition_time_s * 1000U));

    int32_t min_range_bin = (int32_t)(presence_config->min_detection_range_m / h->distance_per_bin);
    int32_t max_range_bin = (int32_t)(presence_config->max_detection_range_m / h->distance_per_bin);

    // init presence_result struct
    presence_result.target_state = false;
    presence_result.range_bin = 0;
    presence_result.time_stamp_ms = 0;

    // initialize Xensiv Presence Configuration
    xensiv_radar_presence_config_t default_config =
        {
            .bandwidth = bandwidth_Hz,
            .num_samples_per_chirp = sensor_config->num_samples_per_chirp,
            .micro_fft_decimation_enabled = false,
            .micro_fft_size = 128,
            .macro_threshold = 0.5f,
            .micro_threshold = 12.5f,
            .min_range_bin = min_range_bin,
            .max_range_bin = max_range_bin,
            .macro_compare_interval_ms = 250,
            .macro_movement_validity_ms = 1000,
            .micro_movement_validity_ms = 4000,
            .macro_movement_confirmations = 0,
            .macro_trigger_range = 1,
            .mode = XENSIV_RADAR_PRESENCE_MODE_MICRO_IF_MACRO,
            .macro_fft_bandpass_filter_enabled = false,
            .micro_movement_compare_idx = 5};

    map_sensitivity_to_thresholds(presence_config->sensitivity_threshold, &default_config);

    // xensiv_radar_presence_init_config(&default_config);

    if (xensiv_radar_presence_alloc(&h->xensiv_handle, &default_config) != 0)
    {
        IFX_ERR_BRN_MEMALLOC(h);
    }

    // set callback
    xensiv_radar_presence_set_callback(h->xensiv_handle, presence_detection_cb, NULL);

    h->sensor_config = *sensor_config;  // copy sensor config within handle for frame conversion

    return h;
}

//----------------------------------------------------------------------------

void ifx_presence_sensing_destroy(ifx_Presence_Sensing_t* handle)
{
    if (handle == NULL)
        return;

    xensiv_radar_presence_free(handle->xensiv_handle);
    ifx_mem_free(handle->frame_data);
    ifx_mem_free(handle);
}

//----------------------------------------------------------------------------

void ifx_presence_sensing_run(ifx_Presence_Sensing_t* handle, const ifx_Cube_R_t* frame_data,
                              ifx_Presence_Sensing_Result_t* result)
{
    IFX_CUBE_BRK_VALID(frame_data);
    IFX_ERR_BRK_NULL(handle);
    IFX_ERR_BRK_NULL(result);
    
    // Process raw data frame with artificially generated time stamp required by algo.
    handle->presence_time_stamp_ms += handle->frame_period_ms;
    
    // extract one rx antenna out of cube
    if (IFX_CUBE_ROWS(frame_data) != 1)
    {
        frame_converter(handle, frame_data);
        xensiv_radar_presence_process_frame(handle->xensiv_handle, handle->frame_data, handle->presence_time_stamp_ms);
    }
    else
    {  // only one Rx present in frame, thus no extraction required
        xensiv_radar_presence_process_frame(handle->xensiv_handle, frame_data->data, handle->presence_time_stamp_ms);
    }

    /*printf("Presence sensing result: %d %f\n", 
            presence_result.target_state, presence_result.range_bin * handle->distance_per_bin);*/
    // fill out the result structure
    result->target_state = presence_result.target_state;
    result->target_distance_m = presence_result.range_bin * handle->distance_per_bin;
}
