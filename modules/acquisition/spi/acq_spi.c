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

#include "interface/acquisition.h"
#include "interface/report.h"

#include "direct.h"

static bool acq_set_mode(const char *name);
static bool acq_enable_data_integrity_test(bool enable);

static const app_option_t acq_options[] = {
    APP_OPTION_STRING(
        "mode",
        "select which mode to use (landscape, landscape-1ghz)",
        acq_set_mode),
    APP_OPTION_BOOL(
        "data_integrity_test",
        "enable spi data integrity test (all data trasnfered to the algo will be 0 if enabled)",
        acq_enable_data_integrity_test),
    APP_OPTION_END
};

const app_cmdarg_t acq_adesc = { "spi", "spi based data acquisition", acq_options };

static const direct_mode_description_t *mode = 
    &direct_device_default_mode_table[0];

void acq_init()
{
    direct_device_init();
}

void acq_deinit()
{
    acq_stop();
}

/*
bool acq_get_segmentation_config(ifx_Segmentation_Config_t *out)
{
    *out = mode->seg_config;
    return true;
}
*/
bool acq_set_mode(const char *name)
{
    const direct_mode_description_t *new_mode =
        direct_device_default_mode_find(name);
    if(new_mode == NULL) {
        rep_err("Mode '%s' not understood by spi direct access data source.\n", name);
        return false;
    }

    mode = new_mode;
    return true;
}

bool acq_enable_data_integrity_test(bool enable)
{
    direct_device_configure_data_integrity_test(enable);
    return true;
}


bool acq_start()
{

    if(! direct_device_start(mode)) {
        rep_err("failed to start direct device data fetching.\n");
        return false;
    }

    return true;
}

void acq_stop()
{
    direct_device_stop();
}

bool acq_fetch(ifx_Cube_R_t **out)
{
    return direct_device_acq_fetch(out);
}
