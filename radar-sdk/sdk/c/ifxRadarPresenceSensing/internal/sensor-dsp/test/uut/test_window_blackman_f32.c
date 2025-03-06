#include <stdio.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_window_blackman_f32.c"


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


/* reference generated using numpy.blackman(64).astype(np.single) */
const float32_t blackman_ref_f32[] =
{
    -1.3877788e-17f, 8.9841132e-04f, 3.6318530e-03f, 8.3126994e-03f,
    1.5120840e-02f,  2.4292914e-02f, 3.6107894e-02f, 5.0869633e-02f,
    6.8887137e-02f,  9.0453424e-02f, 1.1582390e-01f, 1.4519517e-01f,
    1.7868534e-01f,  2.1631649e-01f, 2.5800049e-01f, 3.0352849e-01f,
    3.5256478e-01f,  4.0464568e-01f, 4.5918295e-01f, 5.1547271e-01f,
    5.7270867e-01f,  6.3000000e-01f, 6.8639290e-01f, 7.4089545e-01f,
    7.9250437e-01f,  8.4023350e-01f, 8.8314229e-01f, 9.2036361e-01f,
    9.5112985e-01f,  9.7479635e-01f, 9.9086124e-01f, 9.9898094e-01f,
    9.9898094e-01f,  9.9086124e-01f, 9.7479635e-01f, 9.5112985e-01f,
    9.2036361e-01f,  8.8314229e-01f, 8.4023350e-01f, 7.9250437e-01f,
    7.4089545e-01f,  6.8639290e-01f, 6.3000000e-01f, 5.7270867e-01f,
    5.1547271e-01f,  4.5918295e-01f, 4.0464568e-01f, 3.5256478e-01f,
    3.0352849e-01f,  2.5800049e-01f, 2.1631649e-01f, 1.7868534e-01f,
    1.4519517e-01f,  1.1582390e-01f, 9.0453424e-02f, 6.8887137e-02f,
    5.0869633e-02f,  3.6107894e-02f, 2.4292914e-02f, 1.5120840e-02f,
    8.3126994e-03f,  3.6318530e-03f, 8.9841132e-04f, -1.3877788e-17f
};

/*******************************************************************************
* Function Name: test_window_blackman_f32
****************************************************************************//**
* Description:
* Checks the validity of ifx_window_blackman_f32 calculations
*
*******************************************************************************/
void test_window_blackman_f32()
{
    const uint32_t ref_size = sizeof(blackman_ref_f32) / sizeof(float32_t);
    float32_t win[ref_size];

    ifx_window_blackman_f32(win, ref_size);

    for (uint32_t i = 0; i < ref_size; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(0.00001f, blackman_ref_f32[i], win[i]);
    }
}
