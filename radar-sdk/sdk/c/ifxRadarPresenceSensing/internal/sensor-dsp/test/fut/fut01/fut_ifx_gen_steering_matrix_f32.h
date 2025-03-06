#include "ifx_sensor_dsp.h"
#include <stdlib.h>

extern uint32_t cycle_before;
extern uint32_t cycle_after;
extern uint32_t cycles;

void test_ifx_gen_steering_matrix_f32(float32_t range,
                                      float32_t angular_resolution,
                                      uint32_t antennas_number,
                                      float32_t antennas_spacing_mm,
                                      float32_t lambda)
{
    arm_matrix_instance_f32 steering_matrix;
    uint32_t num_ant = antennas_number;
    float32_t ang_est_range = range * PI / 180.0f;
    float32_t ang_est_resolution = angular_resolution * PI / 180.0f;
    float32_t antenna_spacing_mm = antennas_spacing_mm; // 2.5 mm
    float32_t lambda_mm = lambda;                       // 5.0 mm
    float32_t *output_data;

    uint32_t num_angles = (uint32_t)(2.0f * ang_est_range / ang_est_resolution + 1.5f);

    // read cycles before
    cycle_before = read_cycle_counter();
    output_data = malloc(num_angles * num_ant * sizeof(cfloat32_t));
    arm_mat_init_f32(&steering_matrix, num_angles, num_ant, output_data);

    // start function
    ifx_gen_steering_matrix_f32(ang_est_range,
                                ang_est_resolution,
                                antenna_spacing_mm,
                                lambda_mm,
                                num_ant,
                                &steering_matrix);

    // read cycles after
    cycle_after = read_cycle_counter();
    cycles = cycle_after - cycle_before;

    // print info about test
    printf("ifx_gen_steering_matrix_f32: range = %.1f, angular resolution = %.1f, antenna spacing = %.1f, "
           "lambda = %.1f, number of antennas = %ld, cycles = %ld\n\r",
           range,
           angular_resolution,
           antennas_spacing_mm,
           lambda,
           antennas_number,
           cycles);

    free(output_data);
}