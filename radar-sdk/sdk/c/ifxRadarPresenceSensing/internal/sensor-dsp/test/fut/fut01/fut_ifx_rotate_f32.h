#include <stdlib.h>

#include "ifx_sensor_dsp.h"

extern uint32_t cycle_before;
extern uint32_t cycle_after;
extern uint32_t cycles;

void test_ifx_rotate_f32(uint32_t length)
{
    uint32_t size = length * sizeof(float32_t);
    float32_t *v = malloc(size);

    // read cycles before
    cycle_before = read_cycle_counter();

    // start function
    ifx_rotate_f32(v, size, size);

    // read cycles after
    cycle_after = read_cycle_counter();
    cycles = cycle_after - cycle_before;

    // print info about test
    printf("ifx_rotate_f32: length = %ld, cycles = %ld\n\r", length, cycles);

    // clean
    free(v);
}