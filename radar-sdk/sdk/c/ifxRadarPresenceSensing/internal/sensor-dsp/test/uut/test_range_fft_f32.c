#include <stdio.h>
#include <stdlib.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_range_fft_f32.c"
#include "ifx_mean_removal_f32.c"

uint32_t num_chirps;
uint32_t num_samples;
uint32_t num_bins;
float32_t* frame;
cfloat32_t* range_ref;
cfloat32_t* range;

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

    fp = fopen("frame.txt", "r");
    TEST_ASSERT_TRUE(fp != NULL);

    fgets(buffer, buffer_len, fp);
    TEST_ASSERT_TRUE(sscanf(buffer, "(%d, %d)", &num_chirps, &num_samples) == 2);

    frame = malloc(num_chirps * num_samples * sizeof(float32_t));
    TEST_ASSERT_TRUE(frame != NULL);

    uint32_t i = 0;
    while (fgets(buffer, buffer_len, fp))
    {
        frame[i] = strtof(buffer, NULL);
        i++;
    }

    fclose(fp);

    fp = fopen("range.txt", "r");
    TEST_ASSERT_TRUE(fp != NULL);

    fgets(buffer, buffer_len, fp);
    TEST_ASSERT_TRUE(sscanf(buffer, "(%d, %d)", &num_chirps, &num_bins) == 2);

    range_ref = malloc(num_chirps * num_bins * sizeof(cfloat32_t));
    TEST_ASSERT_TRUE(range_ref != NULL);

    i = 0;
    while (fgets(buffer, buffer_len, fp))
    {
        float32_t real;
        float32_t imag;
        sscanf(buffer, " (%f%fj)", &real, &imag);

        range_ref[i] = real + imag * I;
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
    free(frame);
    free(range_ref);
    free(range);
}


/*******************************************************************************
* Function Name: test_range_fft_f32
****************************************************************************//**
* Description:
* Checks the validity of test_range_fft_f32 calculations
*
*******************************************************************************/
void test_range_fft_f32()
{
    range = malloc(num_chirps * num_bins * sizeof(cfloat32_t));
    TEST_ASSERT_TRUE(range != NULL);

    ifx_range_fft_f32(frame, range, false, NULL, num_samples, num_chirps);

    for (uint32_t chirp = 0; chirp < num_chirps; ++chirp)
    {
        for (uint32_t bin = 0; bin < num_bins; ++bin)
        {
            uint32_t idx = bin + (num_bins * chirp);
            TEST_ASSERT_FLOAT_WITHIN(0.00001f,
                                     crealf(range_ref[idx]),
                                     crealf(range[idx]));

            TEST_ASSERT_FLOAT_WITHIN(0.00001f,
                                     cimag(range_ref[idx]),
                                     cimag(range[idx]));
        }
    }
}


void test_range_fft_f32_arguments()
{
    range = malloc(num_chirps * num_bins * sizeof(cfloat32_t));
    TEST_ASSERT_TRUE(range != NULL);

    int32_t status = ifx_range_fft_f32(frame, range, false, NULL, 127, num_chirps);
    TEST_ASSERT_TRUE(status == IFX_SENSOR_DSP_ARGUMENT_ERROR);
}
