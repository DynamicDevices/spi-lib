#include <stdio.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_rotate_f32.c"
#include "ifx_shift_cfft_f32.c"


const cfloat32_t v_even_ref[] =
{
    8*I, 9*I, 10*I, 11*I, 12*I, 13*I, 14*I, 15*I,
    0*I, 1*I, 2*I,  3*I,  4*I,  5*I,  6*I,  7*I
};

const cfloat32_t v_odd_ref[] =
{
    8*I, 9*I, 10*I, 11*I, 12*I, 13*I, 14*I,
    0*I, 1*I, 2*I,  3*I,  4*I,  5*I,  6*I, 7*I
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
void test_shift_cfft_odd_f32()
{
    const uint32_t size = 15;
    cfloat32_t v[size];
    for (uint32_t i = 0; i < size; ++i)
    {
        v[i] = i*I;
    }

    ifx_shift_cfft_f32(v, size, 1);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY((float*)v_odd_ref, (float*)v, size * CMPLX_DIM);
}


void test_shift_cfft_even_f32()
{
    const uint32_t size = 16;
    cfloat32_t v[size];
    for (uint32_t i = 0; i < size; ++i)
    {
        v[i] = i*I;
    }

    ifx_shift_cfft_f32(v, size, 1);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY((float*)v_even_ref, (float*)v, size * CMPLX_DIM);
}
