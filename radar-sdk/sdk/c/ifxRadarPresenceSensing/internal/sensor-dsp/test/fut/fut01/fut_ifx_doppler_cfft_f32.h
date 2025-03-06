#include <stdlib.h>

#include "ifx_sensor_dsp.h"

extern uint32_t cycle_before;
extern uint32_t cycle_after;
extern uint32_t cycles;

void test_ifx_doppler_cfft_f32(uint32_t num_chirps,
                               uint32_t num_range_bins,
                               uint32_t doppler_bins,
                               bool mean_removal)
{
    cfloat32_t *range;
    cfloat32_t *doppler;

    range = malloc(num_chirps * num_range_bins * sizeof(cfloat32_t));
    doppler = malloc(num_range_bins * doppler_bins * sizeof(cfloat32_t));

    // read cycles before
    cycle_before = read_cycle_counter();

    // start function
    ifx_doppler_cfft_f32(range, doppler, mean_removal, NULL, num_range_bins, num_chirps);

    // read cycles after
    cycle_after = read_cycle_counter();
    cycles = cycle_after - cycle_before;

    // print info about test
    printf("ifx_doppler_cfft_f32: mean removal = %s, number of range bins = %ld, chirps per frame = %ld, "
           "number of antennas = %ld, cycles = %ld\n\r",
           mean_removal ? "true" : "false",
           num_range_bins,
           num_chirps,
           cycles);

    // clean
    free(range);
    free(doppler);
}