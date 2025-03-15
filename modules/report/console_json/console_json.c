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

#include "interface/report.h"
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#define TIME_FORMAT "%02" "d:%02" "d:%02" "d.%03" "d"

typedef struct
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    LARGE_INTEGER freq;
    LARGE_INTEGER start;
#else
    struct timeval start;
#endif
} chrono_t;


static chrono_t s_processing_start_time;
static chrono_t s_frame_processing_start_time;
static uint32_t s_frame_count = 0;

static void chrono_start(chrono_t *chrono)
{
    assert(chrono != NULL);
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    QueryPerformanceFrequency(&chrono->freq);
    QueryPerformanceCounter(&chrono->start);
#else
    gettimeofday(&chrono->start, NULL);
#endif
}

static int64_t chrono_elapsed_ms(chrono_t *chrono)
{
    assert(chrono != NULL);

    int64_t elapsed = 0;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    elapsed = ((end.QuadPart - chrono->start.QuadPart) * 1000) / chrono->freq.QuadPart;

#else
    struct timeval end;
    gettimeofday(&end, NULL);
    elapsed = (end.tv_sec - chrono->start.tv_sec) * 1000;
    elapsed += (end.tv_usec - chrono->start.tv_usec) / 1000;
#endif
    return elapsed;
}


static void chrono_print_elapsed(chrono_t *chrono)
{
    assert(chrono != NULL);

    const int64_t time_ms = chrono_elapsed_ms(chrono);

    const int64_t time_sec = time_ms / 1000;
    const int64_t time_min = time_sec / 60;
    const int64_t time_h = time_min / 60;
    
    const int ms = (int)(time_ms % 1000);
    const int s = (int)(time_sec % 60);
    const int m = (int)(time_min % 60);
    const int h = (int)(time_h % 24);
    int d = (int)(time_h / 24);

    if (d > 999)
        d = 0;

    if (d)
        printf("%dd "TIME_FORMAT, d, h, m, s, ms);
    else
        printf(TIME_FORMAT, h, m, s, ms);

}

void rep_init()
{
    // set buffering mode to line buffering on stdout and stderr
    //fflush(stdout);
    //setvbuf(stdout, NULL, _IOLBF, 4096);
    //fflush(stderr);
    //setvbuf(stderr, NULL, _IOLBF, 4096);

    chrono_start(&s_processing_start_time);
    chrono_start(&s_frame_processing_start_time);

    s_frame_count = 0;
}

void rep_deinit()
{
    s_frame_count = 0;
}

void rep_msg(const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    fflush(stdout);
}

void rep_err(const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fflush(stderr);
}

void rep_mark_processing_start()
{
    chrono_start(&s_frame_processing_start_time);

    s_frame_count = 0;
}

void rep_mark_frame_processing_start()
{
    s_frame_count++;
    chrono_start(&s_frame_processing_start_time);
}
