#include <stdlib.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_peak_search_f32.c"
#include "ifx_test_helper.h"

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
}


/*******************************************************************************
* Function Name: test_gen_steering_matrix_f32
****************************************************************************//**
* Description:
* Checks the validity of test_gen_steering_matrix_f32 calculations
*
*******************************************************************************/
void test_peak_search_f32()
{
    uint32_t count;

    // reference outputs for scenarios covering code
    int32_t peak_outputs_ref_01[] =
        { 42, 147, 299, 416, 533, 685, 791, 933 };

    int32_t peak_outputs_ref_02[] =
        { 42, 147, 416, 685, 791 };

    int32_t peak_outputs_ref_03[] =
        { 331,  386,  524, 729, 867, 956,
          1034, 1210, 1377 };

    int32_t peak_outputs_ref_04[] =
        { 331, 386, 524, 729 };

    int32_t ref_size;
    // read signal data
    float32_t* signal_data;
    int32_t* peak_indices;
    int32_t num_peaks;
    int32_t max_peaks = 100;
    ifx_peak_search_opts_f32_t opts;

    // configure options
    opts.height = 0.0f;
    opts.threshold = FLT_EPSILON;
    opts.distance = 1;
    opts.width = 1;

    peak_indices = malloc(max_peaks * sizeof(int32_t));

    count = count_lines("./data/peak_search_input_01.txt");
    signal_data = malloc(count * sizeof(float32_t));
    TEST_ASSERT_TRUE(read_float32_from_file("./data/peak_search_input_01.txt", signal_data,
                                            count) == count);

    // function under test
    num_peaks = ifx_peak_search_f32(signal_data, count, peak_indices, max_peaks, NULL);

    ref_size = sizeof(peak_outputs_ref_01) / sizeof(int32_t);
    TEST_ASSERT_TRUE(num_peaks = ref_size);
    for (uint32_t i = 0; i < ref_size; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(0.1f, (float32_t)peak_outputs_ref_01[i],
                                 (float32_t)peak_indices[i]);
    }

    // function under test
    num_peaks = ifx_peak_search_f32(signal_data, count, peak_indices, max_peaks, &opts);

    ref_size = sizeof(peak_outputs_ref_02) / sizeof(int32_t);
    TEST_ASSERT_TRUE(num_peaks = ref_size);
    for (uint32_t i = 0; i < ref_size; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(0.1f, (float32_t)peak_outputs_ref_02[i],
                                 (float32_t)peak_indices[i]);
    }
    free(signal_data);

    count = count_lines("./data/peak_search_input_02.txt");
    signal_data = malloc(count * sizeof(float32_t));
    TEST_ASSERT_TRUE(read_float32_from_file("./data/peak_search_input_02.txt", signal_data,
                                            count) == count);

    opts.height = 0.0f;
    opts.threshold = 0.002f;
    opts.distance = 50;
    opts.width = 10;

    // function under test
    num_peaks = ifx_peak_search_f32(signal_data, count, peak_indices, max_peaks, &opts);
    ref_size = sizeof(peak_outputs_ref_03) / sizeof(int32_t);
    TEST_ASSERT_TRUE(num_peaks = ref_size);
    for (uint32_t i = 0; i < ref_size; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(0.1f, (float32_t)peak_outputs_ref_03[i],
                                 (float32_t)peak_indices[i]);
    }

    max_peaks = 4;
    // function under test
    num_peaks = ifx_peak_search_f32(signal_data, count, peak_indices, max_peaks, &opts);
    ref_size = sizeof(peak_outputs_ref_04) / sizeof(int32_t);
    TEST_ASSERT_TRUE(num_peaks = ref_size);
    for (uint32_t i = 0; i < ref_size; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(0.1f, (float32_t)peak_outputs_ref_04[i],
                                 (float32_t)peak_indices[i]);
    }

    free(signal_data);
    free(peak_indices);
}
