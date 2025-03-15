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
 * @file app_argparse.h
 *
 * @brief Commandline argument parsing routines and structures
 *
 */

#ifndef IFX_APP_ARGPARSE_H
#define IFX_APP_ARGPARSE_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "interface/report.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef enum {
    AOT_STRING,
    AOT_PATH,
    AOT_INT,
    AOT_BOOL,
} app_opt_type_t;

typedef struct {
    const char *key;
    const char *description;
    app_opt_type_t type;
    void *set_fn;
} app_option_t;

#define APP_OPTION_STRING(key, desc, fn) { key, desc, AOT_STRING, (void *)(fn) }
#define APP_OPTION_PATH(key, desc, fn) { key, desc, AOT_PATH, (void *)(fn) }
#define APP_OPTION_INT(key, desc, fn) { key, desc, AOT_INT, (void *)(fn) }
#define APP_OPTION_BOOL(key, desc, fn) { key, desc, AOT_BOOL, (void *)(fn) }
#define APP_OPTION_END { NULL, NULL, AOT_STRING, NULL }

typedef struct {
    const char *group;
    const char *description;
    const app_option_t *options;
} app_cmdarg_t;

extern void app_print_cmdargs(const app_cmdarg_t **args, const char *appname);

extern bool app_parse_opt(const app_cmdarg_t **args, const char *group, const char *key, const char *value);

extern bool app_parse_opts(const app_cmdarg_t **args, int argc, char* argv[]);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // IFX_APP_ARGPARSE_H
