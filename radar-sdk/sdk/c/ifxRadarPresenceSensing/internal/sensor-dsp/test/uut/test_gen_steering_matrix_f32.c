#include <stdlib.h>
#include "unity.h"

/* Include the *.c file to be able to access the static functions */
#include "ifx_gen_steering_matrix_f32.c"
//#include "ifx_sensor_dsp.h"

/*******************************************************************************
* Function Name: setUp
********************************************************************************
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
********************************************************************************
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


/* reference generated using matlab */
const float32_t steering_matrix_ref_f32[] =
{
    1.00000000f,  0.0f,         -0.98210584f, 0.18832978f,
    0.92906378f,  -0.36991956f, -0.84277210f, 0.53827054f,
    1.00000000f,  0.0f,         -0.85919898f, 0.51164158f,
    0.47644577f,  -0.87920385f, 0.04047553f,  0.99918052f,
    1.00000000f,  0.0f,         -0.50668859f, 0.86212914f,
    -0.48653334f, -0.87366200f, 0.99973037f,  0.02321999f,
    1.00000000f,  0.0f,         0.09576094f,  0.99540436f,
    -0.98165968f, 0.19064173f,  -0.28377027f, -0.95889229f,
    1.00000000f,  0.0f,         0.72482219f,  0.68893597f,
    0.05073443f,  0.99871217f,  -0.65127530f, 0.75884153f,
    1.00000000f,  0.0f,         1.00000000f,  0.f,
    1.00000000f,  0.0f,         1.00000000f,  0.f,
    1.00000000f,  0.0f,         0.72482219f,  -0.68893597f,
    0.05073443f,  -0.99871217f, -0.65127530f, -0.75884153f,
    1.00000000f,  0.0f,         0.09576094f,  -0.99540436f,
    -0.98165968f, -0.19064173f, -0.28377027f, 0.95889229f,
    1.00000000f,  0.0f,         -0.50668859f, -0.86212914f,
    -0.48653334f, 0.87366200f,  0.99973037f,  -0.02321999f,
    1.00000000f,  0.0f,         -0.85919898f, -0.51164158f,
    0.47644577f,  0.87920385f,  0.04047553f,  -0.99918052f,
    1.00000000f,  0.0f,         -0.98210584f, -0.18832978f,
    0.92906378f,  0.36991956f,  -0.84277210f, -0.53827054f
};

/*******************************************************************************
* Function Name: test_gen_steering_matrix_f32
********************************************************************************
* Description:
* Checks the validity of test_gen_steering_matrix_f32 calculations
*
*******************************************************************************/
void test_gen_steering_matrix_f32()
{
    arm_matrix_instance_f32 steering_matrix;
    const uint32_t ref_size = sizeof(steering_matrix_ref_f32) / sizeof(float32_t);
    float32_t* output_data;

    uint32_t num_ant = 4;
    float32_t ang_est_range = 70.0f * PI / 180.0f;
    float32_t ang_est_resolution = 14.0f * PI / 180.0f;
    float32_t antenna_spacing_mm = 2.5f; // 2.5 mm
    float32_t lambda_mm = 5.0f;          // 5.0 mm

    uint32_t num_angles = (uint32_t)(2.0f * ang_est_range / ang_est_resolution + 1.5f);

    // allocate memory for complex data output
    output_data = malloc(num_angles * num_ant * sizeof(cfloat32_t));
    arm_mat_init_f32(&steering_matrix, num_angles, num_ant, output_data);

    // function under test
    ifx_gen_steering_matrix_f32(ang_est_range, num_angles, antenna_spacing_mm,
                                lambda_mm, num_ant, &steering_matrix);

    // verification
    for (uint32_t i = 0; i < ref_size; ++i)
    {
        TEST_ASSERT_FLOAT_WITHIN(0.001f, steering_matrix_ref_f32[i], steering_matrix.pData[i]);
    }
    free(output_data);
}
