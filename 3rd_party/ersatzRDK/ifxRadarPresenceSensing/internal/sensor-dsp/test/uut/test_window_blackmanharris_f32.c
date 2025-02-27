#include <stdio.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_window_blackmanharris_f32.c"


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


/* reference generated using signal.blackmanharris(64).astype(np.single) */
const float32_t blackmanharris_ref_f32[] =
{
    5.9999998e-05f, 2.0410141e-04f, 6.7726622e-04f, 1.6020078e-03f,
    3.1820950e-03f, 5.7012765e-03f, 9.5202932e-03f, 1.5071173e-02f,
    2.2847781e-02f, 3.3391725e-02f, 4.7272988e-02f, 6.5065138e-02f,
    8.7315381e-02f, 1.1451042e-01f, 1.4703956e-01f, 1.8515706e-01f,
    2.2894618e-01f, 2.7828735e-01f, 3.3283350e-01f, 3.9199451e-01f,
    4.5493332e-01f, 5.2057499e-01f, 5.8762932e-01f, 6.5462685e-01f,
    7.1996742e-01f, 7.8197896e-01f, 8.3898401e-01f, 8.8936979e-01f,
    9.3165928e-01f, 9.6457791e-01f, 9.8711282e-01f, 9.9856049e-01f,
    9.9856049e-01f, 9.8711282e-01f, 9.6457791e-01f, 9.3165928e-01f,
    8.8936979e-01f, 8.3898401e-01f, 7.8197896e-01f, 7.1996742e-01f,
    6.5462685e-01f, 5.8762932e-01f, 5.2057499e-01f, 4.5493332e-01f,
    3.9199451e-01f, 3.3283350e-01f, 2.7828735e-01f, 2.2894618e-01f,
    1.8515706e-01f, 1.4703956e-01f, 1.1451042e-01f, 8.7315381e-02f,
    6.5065138e-02f, 4.7272988e-02f, 3.3391725e-02f, 2.2847781e-02f,
    1.5071173e-02f, 9.5202932e-03f, 5.7012765e-03f, 3.1820950e-03f,
    1.6020078e-03f, 6.7726622e-04f, 2.0410141e-04f, 5.9999998e-05f
};

/*******************************************************************************
* Function Name: test_window_blackmanharris_f32
****************************************************************************//**
* Description:
* Checks the validity of test_window_blackmanharris_f32 calculations
*
*******************************************************************************/
void test_window_blackmanharris_f32()
{
    const uint32_t ref_size = sizeof(blackmanharris_ref_f32) / sizeof(float32_t);
    float32_t win[ref_size];

    ifx_window_blackmanharris_f32(win, ref_size);

    for (uint32_t i = 0; i < ref_size; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(0.00001f, blackmanharris_ref_f32[i], win[i]);
    }
}
