/***************************************************************************//**
* \file main.c
*
* \brief
* Sensor-DSP snippet and build compile test
*
*******************************************************************************/

/*! [snippet_required_includes] */
#include "ifx_sensor_dsp.h"
/*! [snippet_required_includes] */

/*! [snippet_sensor_dsp] */

#define NUM_SAMPLES_PER_CHRIP (128)
#define NUM_CHIRPS_PER_FRAME  (16)
#define NUM_RANGE_BINS        (NUM_SAMPLES_PER_CHRIP / 2)
#define NUM_DOPPLER_BINS      (NUM_CHIRPS_PER_FRAME)

static float32_t win_range[NUM_SAMPLES_PER_CHRIP];
static float32_t win_doppler[NUM_CHIRPS_PER_FRAME];
static float32_t frame_array[NUM_SAMPLES_PER_CHRIP * NUM_CHIRPS_PER_FRAME];
static cfloat32_t range_array[NUM_RANGE_BINS * NUM_SAMPLES_PER_CHRIP];
static cfloat32_t doppler_array[NUM_DOPPLER_BINS * NUM_RANGE_BINS];

/*******************************************************************************
* Function Name: main
*******************************************************************************/
int main(void)
{
    ifx_window_blackman_f32(win_range, NUM_SAMPLES_PER_CHRIP);
    ifx_range_fft_f32(frame_array,
                      range_array,
                      false,
                      win_range,
                      NUM_SAMPLES_PER_CHRIP,
                      NUM_CHIRPS_PER_FRAME);

    ifx_window_blackmanharris_f32(win_doppler, NUM_DOPPLER_BINS);
    ifx_doppler_cfft_f32(range_array,
                         doppler_array,
                         true,
                         win_doppler,
                         NUM_RANGE_BINS,
                         NUM_CHIRPS_PER_FRAME);

    ifx_shift_cfft_f32(doppler_array, NUM_DOPPLER_BINS, NUM_RANGE_BINS);

    for (;;)
    {
    }
}


/*! [snippet_sensor_dsp] */
