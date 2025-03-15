#include <stdio.h>
#include <stdlib.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_doppler_cfft_f32.c"
#include "ifx_cmplx_mean_removal_f32.c"


uint32_t num_chirps;
uint32_t num_range_bins;
uint32_t num_doppler_bins;
cfloat32_t* range;
cfloat32_t* doppler_ref;
cfloat32_t* doppler;

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
    FILE* fp;
    const uint32_t buffer_len = 255;
    char buffer[buffer_len];

    fp = fopen("range.txt", "r");
    TEST_ASSERT_TRUE(fp != NULL);

    fgets(buffer, buffer_len, fp);
    TEST_ASSERT_TRUE(sscanf(buffer, "(%d, %d)", &num_chirps, &num_range_bins) == 2);

    range = malloc(num_chirps * num_range_bins * sizeof(cfloat32_t));
    TEST_ASSERT_TRUE(range != NULL);

    uint32_t i = 0;
    while (fgets(buffer, buffer_len, fp))
    {
        float32_t real;
        float32_t imag;
        sscanf(buffer, " (%f%fj)", &real, &imag);

        range[i] = real + imag * I;
        i++;
    }

    fclose(fp);

    fp = fopen("rdi.txt", "r");
    TEST_ASSERT_TRUE(fp != NULL);

    fgets(buffer, buffer_len, fp);
    TEST_ASSERT_TRUE(sscanf(buffer, "(%d, %d)", &num_doppler_bins, &num_range_bins) == 2);

    doppler_ref = malloc(num_range_bins * num_doppler_bins * sizeof(cfloat32_t));
    TEST_ASSERT_TRUE(range != NULL);

    i = 0;
    while (fgets(buffer, buffer_len, fp))
    {
        float32_t real;
        float32_t imag;
        sscanf(buffer, " (%f%fj)", &real, &imag);

        doppler_ref[i] = real + imag * I;
        i++;
    }

    fclose(fp);
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
    free(range);
    free(doppler_ref);
    free(doppler);
}


/*******************************************************************************
* Function Name: test_rotate_f32
****************************************************************************//**
* Description:
* Checks the validity of ifx_window_blackman_f32 calculations
*
*******************************************************************************/
void test_doppler_cfft_f32()
{
    doppler = malloc(num_range_bins * num_doppler_bins * sizeof(cfloat32_t));
    TEST_ASSERT_TRUE(range != NULL);

    ifx_doppler_cfft_f32(range, doppler, false, NULL, num_range_bins, num_chirps);

    for (uint32_t range_idx = 0; range_idx < num_range_bins; ++range_idx)
    {
        for (uint32_t doppler_idx = 0; doppler_idx < num_doppler_bins; ++doppler_idx)
        {
            uint32_t idx = doppler_idx + (num_doppler_bins * range_idx);
            TEST_ASSERT_FLOAT_WITHIN(0.0001f,
                                     creal(doppler_ref[idx]),
                                     creal(doppler[idx]));
            TEST_ASSERT_FLOAT_WITHIN(0.0001f,
                                     cimag(doppler_ref[idx]),
                                     cimag(doppler[idx]));
        }
    }
}


void test_doppler_cfft_f32_arguments()
{
    doppler = malloc(num_range_bins * num_doppler_bins * sizeof(cfloat32_t));
    TEST_ASSERT_TRUE(range != NULL);

    int32_t status = ifx_doppler_cfft_f32(range, doppler, false, NULL, num_range_bins, 127);
    TEST_ASSERT_TRUE(status == IFX_SENSOR_DSP_ARGUMENT_ERROR);
}
