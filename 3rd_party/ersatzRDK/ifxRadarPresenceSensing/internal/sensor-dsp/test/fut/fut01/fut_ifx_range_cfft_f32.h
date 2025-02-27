#include <stdlib.h>

#include "ifx_sensor_dsp.h"

extern uint32_t cycle_before;
extern uint32_t cycle_after;
extern uint32_t cycles;

void test_ifx_range_cfft_f32(uint32_t num_samples_per_chirp, uint32_t num_chirps_per_frame)
{
    cfloat32_t *frame = malloc(num_samples_per_chirp * num_chirps_per_frame);
    float32_t *win_range = malloc(num_samples_per_chirp);

    // read cycles before
    cycle_before = read_cycle_counter();

    // start function
    ifx_range_cfft_f32(frame, false, win_range, num_samples_per_chirp, num_chirps_per_frame);

    // read cycles after
    cycle_after = read_cycle_counter();
    cycles = cycle_after - cycle_before;

    // print info about test
    printf("ifx_range_cfft_f32: samples per chirp = %ld, chirps per frame = %ld, number of antennas = %ld, "
           "cycles = "
           "%ld\n\r",
           num_samples_per_chirp,
           num_chirps_per_frame,
           cycles);

    // clean
    free(frame);
    free(win_range);
}