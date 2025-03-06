#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cy_result.h"
#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cyhal.h"
#include "ifx_sensor_dsp.h"

#include "dwt.h"
#include "fut_benchmark_parameters.h"
#include "fut_ifx_angle_dbf_f32.h"
#include "fut_ifx_angle_monopulse_f32.h"
#include "fut_ifx_cmplx_mean_removal_f32.h"
#include "fut_ifx_doppler_cfft_f32.h"
#include "fut_ifx_flip_f32.h"
#include "fut_ifx_gen_steering_matrix_f32.h"
#include "fut_ifx_mean_removal_f32.h"
#include "fut_ifx_mti_f32.h"
#include "fut_ifx_peak_search_f32.h"
#include "fut_ifx_range_cfft_f32.h"
#include "fut_ifx_range_fft_f32.h"
#include "fut_ifx_rotate_f32.h"
#include "fut_ifx_shift_cfft_f32.h"
#include "fut_ifx_window_blackman_f32.h"
#include "fut_ifx_window_blackmanharris_f32.h"
#include "fut_ifx_window_hamming_f32.h"
#include "fut_ifx_window_hann_f32.h"

uint32_t cycle_before = 0;
uint32_t cycle_after = 0;
uint32_t cycles = 0;

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);

    /* retarget-io init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the User LED */
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    /* GPIO init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    enable_cycle_counter();

    /* \x1b[2J\x1b[;H - ANSI ESC sequence to clear screen. */
    printf("\x1b[2J\x1b[;H");

    printf("\r\n============================== Benchmark tests begin ==============================\r\n\r\n");

    test_mean_removal_f32(IFX_MEAN_REMOVAL_DATA_SIZE);
    test_ifx_cmplx_mean_removal_f32(IFX_CMPLX_MEAN_REMOVAL_F32_DATA_SIZE);
    test_ifx_range_fft_f32(IFX_RANGE_FFT_F32_MEAN_REMOVAL,
                           IFX_RANGE_FFT_F32_NUM_SAMPLES_PER_CHIRP,
                           IFX_RANGE_FFT_F32_NUM_CHIRPS_PER_FRAME);
    test_ifx_range_cfft_f32(IFX_RANGE_CFFT_F32_NUM_SAMPLES_PER_CHRIP,
                            IFX_RANGE_CFFT_F32_NUM_CHIRPS_PER_FRAME);
    test_ifx_doppler_cfft_f32(IFX_DOPPLER_CFFT_F32_NUM_CHIRPS,
                              IFX_DOPPLER_CFFT_F_32_NUM_RANGE_BINS,
                              IFX_DOPPLER_CFFT_F32_DOPPLER_BINS,
                              IFX_DOPPLER_CFFT_F32_MEAN_REMOVAL);
    test_ifx_window_blackman_f32(IFX_WINDOW_BLACKMAN_F32_SIZE);
    test_ifx_window_blackmanharris_f32(IFX_WINDOW_BLACKMANHARRIS_F32_SIZE);
    test_ifx_window_hamming_f32(IFX_WINDOW_HAMMING_F32_SIZE);
    test_ifx_window_hann_f32(IFX_WINDOW_HANN_F32_SIZE);
    test_ifx_gen_steering_matrix_f32(IFX_GEN_STEERING_MATRIX_F32_RANGE,
                                     IFX_GEN_STEERING_MATRIX_F32_ANGULAR_RESOLUTION,
                                     IFX_GEN_STEERING_MATRIX_F32_ANTENNAS_NUMBER,
                                     IFX_GEN_STEERING_MATRIX_F32_ANTENNA_SPACING_MM,
                                     IFX_GEN_STEERING_MATRIX_F32_LAMBDA);
    test_ifx_angle_dbf_f32(IFX_ANGLE_DBF_F32_STEERING_MATRIX_COLUMNS,
                           IFX_ANGLE_DBF_F32_STEERING_MATRIX_ROWS,
                           IFX_ANGLE_DBF_F32_INPUT_SIGNAL_COLUMNS,
                           IFX_ANGLE_DBF_F32_INPUT_SIGNAL_ROWS);
    test_ifx_shift_cfft_f32(IFX_SHIFT_CFFT_F32_SIZE);
    test_ifx_mti_f32(IFX_MTI_F32_DATA_SIZE, IFX_MTI_F32_ALPHA);
    test_ifx_peak_search_f32(IFX_PEAK_SEARCH_F32_COUNT, IFX_PEAK_SEARCH_F32_MAX_PEAKS);
    test_ifx_flip_f32(IFX_FLIP_F32_SIZE);
    test_ifx_rotate_f32(IFX_ROTATE_F32_V_REF_LENGTH);
    test_ifx_angle_monopulse_f32(IFX_ANGLE_MONOPULSE_F32_SIZE,
                                 IFX_ANGLE_MONOPULSE_F32_WAVELENGTH,
                                 IFX_ANGLE_MONOPULSE_F32_ANT_SPACING);

    printf("\r\n============================== Benchmark tests end ==============================\r\n");

    for (;;)
    {
        // Toggle the LED periodically in case the test succeeded
        cyhal_gpio_toggle(CYBSP_USER_LED);
        cyhal_system_delay_ms(500);
    }
}
