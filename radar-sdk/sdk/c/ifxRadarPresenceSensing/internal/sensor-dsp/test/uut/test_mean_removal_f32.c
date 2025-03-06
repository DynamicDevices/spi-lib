#include <stdio.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_mean_removal_f32.c"
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

static float32_t mean_removal_test_data[DATA_SIZE] =
{
    +1.00000000, +1.00000000, +1.00000000, +1.00000000, +1.00000000,
    +1.00000000, +1.00000000, +1.00000000, +1.00000000, +1.00000000
};

static const float32_t mean_removal_test_data_zeros[DATA_SIZE] =
{
    +0.00000000, +0.00000000, +0.00000000, +0.00000000, +0.00000000,
    +0.00000000, +0.00000000, +0.00000000, +0.00000000, +0.00000000
};

/*******************************************************************************
* Function Name: test_mean_removal_f32
****************************************************************************//**
* Description:
* Checks the validity of test_mean_removal_f32 calculations
*
*******************************************************************************/
void test_mean_removal_f32()
{
    ifx_mean_removal_f32(mean_removal_test_data, DATA_SIZE);

    for (uint32_t i = 0; i < DATA_SIZE; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(DELTA, mean_removal_test_data[i],
                                 mean_removal_test_data_zeros[i]);
    }
}
