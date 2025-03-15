#include <stdio.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_cmplx_mean_removal_f32.c"

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


#define DATA_SIZE (10)
#define DELTA     (0.00001F)

static cfloat32_t mean_removal_test_data[DATA_SIZE] =
{
    1.0f + I * 1.0f, 1.0f + I * 1.0f, 1.0f + I * 1.0f, 1.0f + I * 1.0f, 1.0f + I * 1.0f,
    1.0f + I * 1.0f, 1.0f + I * 1.0f, 1.0f + I * 1.0f, 1.0f + I * 1.0f, 1.0f + I * 1.0f
};

static const cfloat32_t mean_removal_test_data_zeros[DATA_SIZE] =
{
    0.0f + I * 0.0f, 0.0f + I * 0.0f, 0.0f + I * 0.0f, 0.0f + I * 0.0f, 0.0f + I * 0.0f,
    0.0f + I * 0.0f, 0.0f + I * 0.0f, 0.0f + I * 0.0f, 0.0f + I * 0.0f, 0.0f + I * 0.0f
};

/*******************************************************************************
* Function Name: test_cmplx_mean_removal_f32
****************************************************************************//**
* Description:
* Checks the validity of test_mean_removal_f32 calculations
*
*******************************************************************************/
void test_cmplx_mean_removal_f32()
{
    ifx_cmplx_mean_removal_f32(mean_removal_test_data, DATA_SIZE);

    for (uint32_t i = 0; i < DATA_SIZE; ++i)
    {
        // TEST_ASSERT_FLOAT_WITHIN (delta, expected, actual)
        TEST_ASSERT_FLOAT_WITHIN(DELTA, crealf(mean_removal_test_data_zeros[i]),
                                 crealf(mean_removal_test_data[i]));
        TEST_ASSERT_FLOAT_WITHIN(DELTA, cimagf(mean_removal_test_data_zeros[i]),
                                 cimagf(mean_removal_test_data[i]));
    }
}
