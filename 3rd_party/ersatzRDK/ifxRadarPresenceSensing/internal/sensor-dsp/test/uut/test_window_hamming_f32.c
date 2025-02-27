#include <stdio.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_window_hamming_f32.c"

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


/* reference generated using numpy.hamming(64).astype(np.single) */
const float32_t hamming_ref_f32[] =
{
    0.08f,       0.08228584f, 0.08912066f, 0.10043651f, 0.11612094f,
    0.13601808f, 0.15993017f, 0.18761955f, 0.21881106f, 0.2531947f,
    0.29042873f, 0.3301431f,  0.37194312f, 0.41541338f, 0.46012184f,
    0.5056242f,  0.55146813f, 0.5971981f,  0.6423596f,  0.6865039f,
    0.7291921f,  0.77f,       0.8085221f,  0.8443755f,  0.8772039f,
    0.90668094f, 0.93251383f, 0.95444566f, 0.9722586f,  0.98577553f,
    0.9948622f,  0.9994282f,  0.9994282f,  0.9948622f,  0.98577553f,
    0.9722586f,  0.95444566f, 0.93251383f, 0.90668094f, 0.8772039f,
    0.8443755f,  0.8085221f,  0.77f,       0.7291921f,  0.6865039f,
    0.6423596f,  0.5971981f,  0.55146813f, 0.5056242f,  0.46012184f,
    0.41541338f, 0.37194312f, 0.3301431f,  0.29042873f, 0.2531947f,
    0.21881106f, 0.18761955f, 0.15993017f, 0.13601808f, 0.11612094f,
    0.10043651f, 0.08912066f, 0.08228584f, 0.08f
};

/*******************************************************************************
* Function Name: test_window_hamming_f32
****************************************************************************//**
* Description:
* Checks the validity of ifx_window_hamming_f32 calculations
*
*******************************************************************************/
void test_window_hamming_f32()
{
    const uint32_t ref_size = sizeof(hamming_ref_f32) / sizeof(float32_t);
    float32_t win[ref_size];

    ifx_window_hamming_f32(win, ref_size);

    for (uint32_t i = 0; i < ref_size; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(0.00001f, hamming_ref_f32[i], win[i]);
    }
}
