#include <stdio.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_rotate_f32.c"


const float32_t v_ref[] =
{
    0.0f, 1.0f, 2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,
    8.0f, 8.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f
};


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
* Function Name: test_rotate_f32
****************************************************************************//**
* Description:
* Checks the validity of ifx_window_blackman_f32 calculations
*
*******************************************************************************/
void test_zero_rotate_f32()
{
    const uint32_t size = sizeof(v_ref) / sizeof(float32_t);

    float32_t v[size];
    memcpy(v, v_ref, size * sizeof(float32_t));

    ifx_rotate_f32(v, size, 0);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(v_ref, v, size);
}


void test_full_rotate_f32()
{
    const uint32_t size = sizeof(v_ref) / sizeof(float32_t);

    float32_t v[size];
    memcpy(v, v_ref, size * sizeof(float32_t));

    ifx_rotate_f32(v, size, size);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(v_ref, v, size);
}


void test_rotate_f32()
{
    const uint32_t size = sizeof(v_ref) / sizeof(float32_t);

    float32_t v[size];
    memcpy(v, v_ref, size * sizeof(float32_t));

    ifx_rotate_f32(v, size, size - 1);

    for (uint32_t i = 0; i < size; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(0.00001f, v_ref[i], v[(i + 1) % size]);
    }
}
