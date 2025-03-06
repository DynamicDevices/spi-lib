#include <stdlib.h>

#include "ifx_sensor_dsp.h"

extern uint32_t cycle_before;
extern uint32_t cycle_after;
extern uint32_t cycles;

void test_mean_removal_f32(uint32_t data_size)
{
    float32_t mean_removal_test_data[data_size];

    // read cycles before
    cycle_before = read_cycle_counter();

    // start function
    ifx_mean_removal_f32(mean_removal_test_data, data_size);

    // read cycles after
    cycle_after = read_cycle_counter();
    cycles = cycle_after - cycle_before;

    // print info about test
    printf("ifx_mean_removal_f32: length: %ld, cycles = %ld\n\r", data_size, cycles);
}