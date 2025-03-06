#include <stdio.h>
#include <stdlib.h>
#include "unity.h"
#include "math.h"
#include "ifx_sensor_dsp.h"
#include "ifx_arcsin_f32.c"

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
* Function Name: test_arcsin_f32
****************************************************************************//**
* Description:
* Checks the validity of ifx_arcsin_f32 calculations
*
*******************************************************************************/
void test_arcsin_f32()
{
    for (float32_t i = -1; i < 1; i += 0.1F)
    {
        float32_t output = 0.0F;
        arm_status status = ifx_arcsin_f32(i, &output);
        float32_t expected = asin(i);
        TEST_ASSERT_FLOAT_WITHIN(0.00001f, expected, output);
        TEST_ASSERT_TRUE(status == ARM_MATH_SUCCESS);
    }
}


/*******************************************************************************
* Function Name: test_atan2_f32_cmsis_dsp
****************************************************************************//**
* Description:
* Checks the validity of arm_atan2_f32 calculations
*
*******************************************************************************/
void test_atan2_f32_cmsis_dsp()
{
    for (float32_t i = -3; i < 3; i += 0.5F)
    {
        for (float32_t j = -3; j < 3; j += 0.5F)
        {
            float32_t output = 0.0F;
            arm_status status = arm_atan2_f32(i, j, &output);
            float32_t expected = atan2(i, j);
            TEST_ASSERT_FLOAT_WITHIN(0.00001f, expected, output);
            if ((i == 0) && (j == 0))
            {
                TEST_ASSERT_TRUE(status == ARM_MATH_NANINF);
            }
            else
            {
                TEST_ASSERT_TRUE(status == ARM_MATH_SUCCESS);
            }
        }
    }
}
