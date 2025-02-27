#include "ifx_sensor_dsp.h"
#include <stdlib.h>

extern uint32_t cycle_before;
extern uint32_t cycle_after;
extern uint32_t cycles;

void test_ifx_mti_f32(uint32_t size, float32_t alpha)
{
    float32_t history[size];
    float32_t in[size];
    float32_t out[size];
    const ifx_mti_inst_f32 inst = {size, alpha, history};

    // read cycles before
    cycle_before = read_cycle_counter();

    // start function
    ifx_mti_f32(&inst, in, out);

    // read cycles after
    cycle_after = read_cycle_counter();
    cycles = cycle_after - cycle_before;

    // print info about test
    printf("ifx_mti_f32: length = %ld, alpha = %.1f, cycles = %ld\n\r", size, alpha, cycles);
}