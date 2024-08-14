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
 * @file acquisition.h
 *
 * @brief This file defines the interface which data sources need to
 *        implement.
 *
 * Implement these interfaces if you want to get the radar data via some transport
 * interface that is not provided as part of the standard distribution.
 * @note This file intentionally doesn't define any function
 */

#ifndef IFX_ACQUISITION_H
#define IFX_ACQUISITION_H

#include <stdbool.h>
#include "ifxBase/Cube.h"
#include "interface/app_utils.h"
#include "interface/app_argparse.h"
//#include "ifxSeamless/Segmentation.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

extern const app_cmdarg_t acq_adesc;

extern void acq_init();
extern void acq_deinit();
//extern bool acq_get_segmentation_config(ifx_Segmentation_Config_t *out);
extern bool acq_start();
extern void acq_stop();
extern bool acq_fetch(ifx_Cube_R_t **out);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // IFX_ACQUISITION_H
