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
 * @file report.h
 *
 * @brief This file defines the interface which the seamless tracking application
 *        uses to present the algorithm results and application messages to the user.
 *
 */

#ifndef IFX_SEAMLESS_REP_H
#define IFX_SEAMLESS_REP_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#if defined(__GNUC__) || defined(__clang__)
#   define FMT_FN_ATTR  __attribute__ ((format (printf, 1, 2)))
#else
#   define FMT_FN_ATTR
#endif

/* Forward definitions to avoid including header */
struct ifx_Segment_Result_s;
struct ifx_Segmentation_Track_s;

/* Init data display, return false on error, true on success */
extern void rep_init();

/* De-init data display */
extern void rep_deinit();

/* Display a message to the user */
extern void rep_msg(const char* fmt, ...) FMT_FN_ATTR;

/* Display an error to the user */
extern void rep_err(const char* fmt, ...) FMT_FN_ATTR;

/* Mark start of processing */
extern void rep_mark_processing_start();

/* Tasks to be done before the computation starts */
extern void rep_mark_frame_processing_start();

/* Display results of tracking */
extern bool rep_track_and_segment_results(
    const struct ifx_Segment_Result_s* segments, const uint32_t numSegments,
    const struct ifx_Segmentation_Track_s* tracks, const uint32_t numActiveTracks);

#undef FMT_FN_ATTR

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // IFX_SEAMLESS_REP_H
