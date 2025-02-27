#include "ifx_sensor_dsp.h"
#include <stdlib.h>

extern uint32_t cycle_before;
extern uint32_t cycle_after;
extern uint32_t cycles;

void test_ifx_window_blackmanharris_f32(uint32_t size)
{
    float32_t win[size];

    // read cycles before
    cycle_before = read_cycle_counter();

    // start function
    ifx_window_blackmanharris_f32(win, size);

    // read cycles after
    cycle_after = read_cycle_counter();
    cycles = cycle_after - cycle_before;

    // print info about test
    printf("ifx_window_blackmanharris_f32: length = %ld, cycles = %ld\n\r", size, cycles);
}