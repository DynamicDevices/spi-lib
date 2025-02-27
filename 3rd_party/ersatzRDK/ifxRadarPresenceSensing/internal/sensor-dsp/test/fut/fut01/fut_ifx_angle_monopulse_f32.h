#include <stdlib.h>

#include "ifx_sensor_dsp.h"

extern uint32_t cycle_before;
extern uint32_t cycle_after;
extern uint32_t cycles;

void test_ifx_angle_monopulse_f32(uint32_t size, float32_t wavelength, float32_t spacing)
{
    cfloat32_t *rx1 = NULL;
    cfloat32_t *rx2 = NULL;
    float32_t *output = NULL;

    rx1 = malloc(size * sizeof(cfloat32_t));
    rx2 = malloc(size * sizeof(cfloat32_t));
    output = malloc(size * sizeof(float32_t));

    // read cycles before
    cycle_before = read_cycle_counter();

    ifx_angle_monopulse_f32(rx1, rx2, size, wavelength, spacing, output);

    // read cycles after
    cycle_after = read_cycle_counter();
    cycles = cycle_after - cycle_before;

    // print info about test
    printf("ifx_angle_monopulse_f32: size = %ld, wavelength = %.4f, "
           "antenna spacing = %.4f, cycles = %ld\n\r",
           size,
           wavelength,
           spacing,
           cycles);

    free(rx1);
    free(rx2);
    free(output);
}