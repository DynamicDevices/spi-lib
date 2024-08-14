/* ===========================================================================
** Copyright (C) 2021 Infineon Technologies AG
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

/**
 * @file direct.h
 *
 * @brief Helper library for direct hw access.
 *
 * Use this library if you have some HW interface for SPI direct access
 * to the BGT with a register list for configuring the BGT.
 */

#ifndef DIRECT_H
#define DIRECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "ifxBase/Cube.h"

typedef enum {
    IFX_ORIENTATION_LANDSCAPE = 0U, /**< Sensor is oriented in landscape format (default) */
    IFX_ORIENTATION_PORTRAIT  = 1U, /**< Sensor is oriented in portrait format */
    IFX_ORIENTATION_LANDSCAPE_FLIPPED = 2U,  /**< Sensor is oriented in landscape format but flipped 180 degrees */
    IFX_ORIENTATION_PORTRAIT_FLIPPED = 3U, /**< Sensor is oriented in portrait format  but flipped 180 degrees */
} ifx_Orientation_t;

typedef struct
{
    uint32_t          num_samples_per_chirp;    /**< Number of samples per chirp. */
    uint32_t          num_chirps_per_frame;     /**< Number of chirps per frame. */
    ifx_Float_t       bandwidth_Hz;             /**< Frequency bandwidth in Hz. */
    ifx_Float_t       center_frequency_Hz;      /**< Center frequency in Hz. */
    ifx_Orientation_t orientation;              /**< Orientation of the sensor \ref ifx_Orientation_t */
} ifx_Config_t;

typedef struct {
    const char* specifier;
    const char* name;
    uint8_t num_antennas;
    const uint32_t *regs;
    ifx_Config_t seg_config;
} direct_mode_description_t;

extern void direct_device_init();
extern void direct_device_deinit();
extern void direct_device_configure_data_integrity_test(bool enable);
extern bool direct_device_start(const direct_mode_description_t *mode);
extern void direct_device_stop();
extern bool direct_device_acq_fetch(
    ifx_Cube_R_t **out);


/* Default mode table which might be re-used in another application.
 * If the default modes don't fit, create your own table and use it
 * instead */
extern direct_mode_description_t direct_device_default_mode_table[];

/* Helper function to find a mode entry from the default table
 */
extern const direct_mode_description_t*
direct_device_default_mode_find(const char* name);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // DIRECT_H
