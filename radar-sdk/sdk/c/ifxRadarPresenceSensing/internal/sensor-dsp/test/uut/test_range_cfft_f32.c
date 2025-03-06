#include <stdio.h>
#include <stdlib.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_range_cfft_f32.c"
#include "ifx_cmplx_mean_removal_f32.c"
#include "pffft.h"

const uint32_t num_chirps = 16;
const uint32_t num_samples = 128;
uint32_t num_bins;
cfloat32_t* frame;
cfloat32_t* range_ref;

/*******************************************************************************
* Function Name: setUp
****************************************************************************//**
*
* The setUp function can contain anything you would like to run before each
* test.
*
* Note: Not used for these test.
*
*******************************************************************************/
void setUp(void)
{
    frame = pffft_aligned_malloc(num_chirps * num_samples * sizeof(cfloat32_t));
    range_ref = pffft_aligned_malloc(num_chirps * num_samples * sizeof(cfloat32_t));

    srand(0);
    for (uint32_t chirp = 0; chirp < num_chirps; chirp++)
    {
        for (uint32_t sample = 0; sample < num_samples; sample++)
        {
            float real = (float)rand() / RAND_MAX - 0.5f;
            float imag = (float)rand() / RAND_MAX - 0.5f;
            frame[sample] = real + I * imag;
        }
    }

    PFFFT_Setup* s = pffft_new_setup(num_samples, PFFFT_COMPLEX);
    for (uint32_t chirp = 0; chirp < num_chirps; chirp++)
    {
        pffft_transform(s,
                        (float32_t*)&frame[num_samples * chirp],
                        (float32_t*)range_ref,
                        NULL,
                        PFFFT_FORWARD);
    }
    pffft_destroy_setup(s);
}


/*******************************************************************************
* Function Name: tearDown
****************************************************************************//**
*
* The tearDown function can contain anything you would like to run after each
* test.
*
* Note: Not used for these test.
*
*******************************************************************************/
void tearDown(void)
{
    pffft_aligned_free(range_ref);
    pffft_aligned_free(frame);
}


/*******************************************************************************
* Function Name: test_range_cfft_f32
****************************************************************************//**
* Description:
* Checks the validity of ifx_window_blackman_f32 calculations
*
*******************************************************************************/
void test_range_cfft_f32()
{
    ifx_range_cfft_f32(frame, false, NULL, num_samples, num_chirps);

    for (uint32_t chirp = 0; chirp < num_chirps; ++chirp)
    {
        for (uint32_t bin = 0; bin < num_bins; ++bin)
        {
            uint32_t idx = bin + (num_bins * chirp);
            TEST_ASSERT_FLOAT_WITHIN(0.00001f,
                                     crealf(range_ref[idx]),
                                     crealf(frame[idx]));
            TEST_ASSERT_FLOAT_WITHIN(0.00001f,
                                     cimag(range_ref[idx]),
                                     cimag(frame[idx]));
        }
    }
}


void test_range_cfft_f32_arguments()
{
    int32_t status = ifx_range_cfft_f32(frame, false, NULL, 127, num_chirps);
    TEST_ASSERT_TRUE(status == IFX_SENSOR_DSP_ARGUMENT_ERROR);
}
