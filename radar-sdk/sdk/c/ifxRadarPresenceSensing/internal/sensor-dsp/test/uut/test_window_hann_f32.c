#include <stdio.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_window_hann_f32.c"


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


/* reference generated using numpy.hanning(64).astype(np.single) */
const float32_t hann_ref_f32[] =
{
    0.0f,        0.00248461f, 0.00991376f, 0.0222136f,  0.03926189f,
    0.06088921f, 0.08688061f, 0.11697778f, 0.15088159f, 0.1882551f,
    0.22872686f, 0.27189466f, 0.3173295f,  0.36457977f, 0.4131759f,
    0.46263495f, 0.51246536f, 0.5621719f,  0.6112605f,  0.65924335f,
    0.70564353f, 0.75f,       0.79187185f, 0.8308429f,  0.86652595f,
    0.89856625f, 0.92664546f, 0.95048445f, 0.9698463f,  0.9845386f,
    0.9944154f,  0.99937844f, 0.99937844f, 0.9944154f,  0.9845386f,
    0.9698463f,  0.95048445f, 0.92664546f, 0.89856625f, 0.86652595f,
    0.8308429f,  0.79187185f, 0.75f,       0.70564353f, 0.65924335f,
    0.6112605f,  0.5621719f,  0.51246536f, 0.46263495f, 0.4131759f,
    0.36457977f, 0.3173295f,  0.27189466f, 0.22872686f, 0.1882551f,
    0.15088159f, 0.11697778f, 0.08688061f, 0.06088921f, 0.03926189f,
    0.0222136f,  0.00991376f, 0.00248461f, 0.0f
};

/*******************************************************************************
* Function Name: test_window_hann_f32
****************************************************************************//**
* Description:
* Checks the validity of ifx_window_hann_f32 calculations
*
*******************************************************************************/
void test_window_hann_f32()
{
    const uint32_t ref_size = sizeof(hann_ref_f32) / sizeof(float32_t);
    float32_t win[ref_size];

    ifx_window_hann_f32(win, ref_size);

    for (uint32_t i = 0; i < ref_size; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(0.00001f, hann_ref_f32[i], win[i]);
    }
}
