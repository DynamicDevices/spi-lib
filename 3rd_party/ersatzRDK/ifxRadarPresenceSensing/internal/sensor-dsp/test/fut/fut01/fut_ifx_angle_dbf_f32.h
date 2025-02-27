#include <stdlib.h>

#include "ifx_sensor_dsp.h"

extern uint32_t cycle_before;
extern uint32_t cycle_after;
extern uint32_t cycles;

void test_ifx_angle_dbf_f32(uint32_t steering_matrix_columns,
                            uint32_t steering_matrix_rows,
                            uint32_t input_signal_columns,
                            uint32_t input_signal_rows)
{
    float32_t steering_matrix_f32[steering_matrix_columns * steering_matrix_rows];
    float32_t InputSignal_f32[128] = {0};

    arm_matrix_instance_f32 steering_matrix;
    arm_matrix_instance_f32 input_signal;
    arm_matrix_instance_f32 output_angle_dbf;
    float32_t *output_data;

    arm_mat_init_f32(&steering_matrix, steering_matrix_columns, steering_matrix_rows, steering_matrix_f32);
    arm_mat_init_f32(&input_signal, input_signal_columns, input_signal_rows, InputSignal_f32);
    output_data = malloc(steering_matrix_columns * input_signal_rows * sizeof(cfloat32_t));
    arm_mat_init_f32(&output_angle_dbf, steering_matrix_columns, input_signal_rows, output_data);

    // read cycles before
    cycle_before = read_cycle_counter();

    // run function
    ifx_angle_dbf_f32(&input_signal, &steering_matrix, &output_angle_dbf);

    // read cycles after
    cycle_after = read_cycle_counter();
    cycles = cycle_after - cycle_before;

    // print info about test
    printf("ifx_angle_dbf_f32: steering matrix columns = %ld, steering matrix rows = %ld, input signal "
           "columns = %ld, input signal rows = %ld, cycles = %ld\n\r",
           steering_matrix_columns,
           steering_matrix_rows,
           input_signal_columns,
           input_signal_rows,
           cycles);

    free(output_data);
}