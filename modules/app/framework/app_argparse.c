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

#include "interface/app_argparse.h"

typedef bool (*set_string_fn)(const char *v);
typedef bool (*set_int_fn)(int v);
typedef bool (*set_bool_fn)(int v);

static bool cvt_set_string(const char *v, void *setter)
{
    set_string_fn fn = (set_string_fn)setter;
    return fn(v);
}

static bool cvt_set_int(const char *v, void *setter)
{
    set_int_fn fn = (set_int_fn)setter;
    return fn(atoi(v));
}

static bool cvt_set_bool(const char *v, void *setter)
{
    set_bool_fn fn = (set_bool_fn)setter;
    if(strcmp("true", v) == 0)
        return fn(true);

    if(strcmp("false", v) == 0)
        return fn(false);

    rep_err("Boolean option must be set to either 'true' or 'false', current value = '%s'\n", v);

    return false;
}

static const struct {
    const char *name;
    bool (*set_cvt)(const char *, void *);
}
aot_map[] = {
    {
        "string",
        cvt_set_string
    },
    {
        "path",
        cvt_set_string
    },
    {
        "int",
        cvt_set_int
    },
    {
        "bool",
        cvt_set_bool
    }
};

static void print_options(const char *group, const app_option_t *opts)
{
    while(opts->description != NULL) {
        rep_msg("    %s.%s=%s: %s\n", group, opts->key, aot_map[opts->type].name, opts->description);
        opts++;
    }
}

void app_print_cmdargs(const app_cmdarg_t **args, const char *appname)
{
    rep_msg("Usage: %s [options]...\n", appname);
    rep_msg("where the following group.key-value [options] are allowed:\n");
    while(*args != NULL) {
        const char *group = (*args)->group;
        rep_msg(" group '%s': %s\n", group, (*args)->description);
        print_options(group, (*args)->options);
        args++;
    }
    rep_msg("\n");
}

static bool apply_opts(const app_option_t *opts, const char *group, const char *key, const char *value)
{
    while(opts->description != NULL) {
        if(strcmp(opts->key, key) == 0) {
            return aot_map[opts->type].set_cvt(value, opts->set_fn);
        }
        opts++;
    }

    rep_err("option '%s' in argument '%s.%s' not found in group '%s'\n", key, group, key, group);
    return false;
}

bool app_parse_opt(const app_cmdarg_t **args, const char *group, const char *key, const char *value)
{
    while(*args != NULL) {
        if(strcmp((*args)->group, group) == 0) {
            const app_option_t *opts = (*args)->options;
            return apply_opts(opts, group, key, value);
        }
        args++;
    }

    rep_err("group '%s' in parameter '%s.%s' not found\n", group, group, key);
    return false;
}

static bool parse_opt(const app_cmdarg_t **args, char *optstr)
{
    char *group = optstr;
    char *dot_separator = strchr(group, '.');
    if(dot_separator == NULL) {
        rep_err("command options are of format 'group.option=value', missing dot separator in '%s'.\n", optstr);
        return false;
    }
    char *key = dot_separator + 1;
    char *equals_sign = strchr(key, '=');
    if(equals_sign == NULL) {
        rep_err("command options are of format 'group.option=value', missing equals sign in '%s'.\n", optstr);
        return false;
    }
    char *value = equals_sign + 1;

    *dot_separator = '\0';
    *equals_sign = '\0';

    return app_parse_opt(args, group, key, value);
}

bool app_parse_opts(const app_cmdarg_t **args, int argc, char* argv[])
{
    if(argc < 2) {
        app_print_cmdargs(args, argv[0]);
        return false;
    }

    for(int i = 1; i < argc; i++) {
        if(! parse_opt(args, argv[i]))
            return false;        
    }

    return true;
}

