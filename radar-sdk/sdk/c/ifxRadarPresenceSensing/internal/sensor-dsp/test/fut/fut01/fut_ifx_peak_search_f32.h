#include "ifx_sensor_dsp.h"
#include <stdlib.h>

extern uint32_t cycle_before;
extern uint32_t cycle_after;
extern uint32_t cycles;

void test_ifx_peak_search_f32(uint32_t count, int32_t max_peaks)
{
    float32_t *signal_data = malloc(count * sizeof(float32_t));
    int32_t *peak_indices = malloc(max_peaks * sizeof(int32_t));
    ifx_peak_search_opts_f32_t opts;

    // configure options
    opts.height = 0.0f;
    opts.threshold = FLT_EPSILON;
    opts.distance = 1;
    opts.width = 1;

    // read cycles before
    cycle_before = read_cycle_counter();

    // start function
    ifx_peak_search_f32(signal_data, count, peak_indices, max_peaks, &opts);

    // read cycles after
    cycle_after = read_cycle_counter();
    cycles = cycle_after - cycle_before;

    // print info about test
    printf("ifx_peak_search_f32: length = %ld, max peaks = %ld, cycles = %ld\n\r", count, max_peaks, cycles);

    // clean
    free(signal_data);
    free(peak_indices);
}