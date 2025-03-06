#include <stdio.h>
#include <stdlib.h>
#include "unity.h"
#include "math.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_angle_monopulse_f32.c"
#include "ifx_arcsin_f32.c"


uint32_t num_of_tests;
float32_t wavelength;
float32_t ant_spacing;
float32_t* expected_angle;
float32_t* input_rx1_real;
float32_t* input_rx1_imag;
float32_t* input_rx2_real;
float32_t* input_rx2_imag;

/*******************************************************************************
* Function Name: setUp
****************************************************************************//**
*
* The setUp function loads test file with input and expected output data.
*
*******************************************************************************/
void setUp(void)
{
    FILE* fp;
    const uint32_t buffer_len = 255;
    char buffer[buffer_len];

    fp = fopen("angle_monopulse.txt", "r");
    TEST_ASSERT_TRUE(fp != NULL);

    fgets(buffer, buffer_len, fp);
    TEST_ASSERT_TRUE(sscanf(buffer, "(%f,%f,%d)", &wavelength, &ant_spacing, &num_of_tests) == 3);

    input_rx1_real = malloc(num_of_tests * sizeof(float32_t));
    input_rx1_imag = malloc(num_of_tests * sizeof(float32_t));
    input_rx2_real = malloc(num_of_tests * sizeof(float32_t));
    input_rx2_imag = malloc(num_of_tests * sizeof(float32_t));
    expected_angle = malloc(num_of_tests * sizeof(float32_t));

    TEST_ASSERT_TRUE(input_rx1_real != NULL);
    TEST_ASSERT_TRUE(input_rx1_imag != NULL);
    TEST_ASSERT_TRUE(input_rx2_real != NULL);
    TEST_ASSERT_TRUE(input_rx2_imag != NULL);
    TEST_ASSERT_TRUE(expected_angle != NULL);

    uint32_t i = 0;
    while (fgets(buffer, buffer_len, fp))
    {
        float32_t real_rx1;
        float32_t imag_rx1;
        float32_t real_rx2;
        float32_t imag_rx2;
        float32_t angle;
        sscanf(buffer, " (%f%fj)(%f%fj)(%f)", &real_rx1, &imag_rx1, &real_rx2, &imag_rx2, &angle);

        input_rx1_real[i] = real_rx1;
        input_rx1_imag[i] = imag_rx1;
        input_rx2_real[i] = real_rx2;
        input_rx2_imag[i] = imag_rx2;
        expected_angle[i] = angle * (PI / 180);
        i++;
    }

    fclose(fp);
}


/*******************************************************************************
* Function Name: tearDown
****************************************************************************//**
*
* The tearDown function clears the space.
*
*******************************************************************************/
void tearDown(void)
{
    free(input_rx1_real);
    free(input_rx1_imag);
    free(input_rx2_real);
    free(input_rx2_imag);
    free(expected_angle);
}


/*******************************************************************************
* Function Name: test_angle_monopulse_f32
****************************************************************************//**
* Description:
* Checks the validity of ifx_angle_monopulse_f32 calculations
*
*******************************************************************************/
void test_angle_monopulse_f32()
{
    uint32_t size = num_of_tests;
    cfloat32_t* rx1;
    cfloat32_t* rx2;
    float32_t* output;

    rx1 = malloc(size * sizeof(cfloat32_t));
    rx2 = malloc(size * sizeof(cfloat32_t));
    output = malloc(size * sizeof(float32_t));

    TEST_ASSERT_TRUE(rx1 != NULL);
    TEST_ASSERT_TRUE(rx2 != NULL);
    TEST_ASSERT_TRUE(output != NULL);

    for (uint32_t i = 0; i < size; ++i)
    {
        rx1[i] = input_rx1_real[i] + input_rx1_imag[i] * I;
        rx2[i] = input_rx2_real[i] + input_rx2_imag[i] * I;
    }

    ifx_angle_monopulse_f32(rx1, rx2, size, wavelength, ant_spacing, output);

    for (uint32_t i = 0; i < size; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(0.001f, expected_angle[i], output[i]);
    }

    free(rx1);
    free(rx2);
    free(output);
}
