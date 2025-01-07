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

#include <string.h>

#include "interface/report.h"
#include "interface/acquisition.h"
#include "interface/record.h"
#include "interface/app_utils.h"
#include "interface/app_argparse.h"
#include "ifxBase/Error.h"

static uint32_t frame_limit = 0;

static bool set_frames(int v) {
    frame_limit = v;
    return true;
}

static const app_option_t run_options[] = {
    APP_OPTION_INT(
        "frame_limit",
        "maximum number of frames to process",
        set_frames),
    APP_OPTION_END
};

static const app_cmdarg_t run_adesc = { "run", "run configuration", run_options };

static const app_cmdarg_t *argdesc[] = {
    &acq_adesc,
    &rec_adesc,
    &run_adesc,
    NULL
};

int main(int argc, char* argv[])
{   
    int exitcode = EXIT_FAILURE;

    rep_init();
    acq_init();
    record_init();

    if(! app_parse_opts(argdesc, argc, argv))
        goto cleanup;

    if (ifx_error_get() != IFX_OK)
        goto cleanup;

    if (!record_start())
        goto cleanup;

    if(!acq_start()) {
        rep_err("failed to start data acquisition\n");
        goto cleanup;
    }

    install_abort_request_signal_handler();

    rep_mark_processing_start();

    while (!abort_requested())
    {
        ifx_Cube_R_t *radar_data_frame = NULL;

        if(!acq_fetch(&radar_data_frame)) {
            rep_err("Data source indicated an error when fetching data\n");
            goto cleanup;
        }
        if(radar_data_frame == NULL) {
            // data source has no more data
            break;
        }

        if(! record_radar_frame(radar_data_frame))
            goto cleanup;

        rep_mark_frame_processing_start();

        // abort the application if a frame limit was specified and has been reached
        if ((frame_limit != 0) && (--frame_limit == 0)) {
            rep_msg("frame limit reached, aborting.\n");
            request_abort();
        }
    }

    // everything successful
    exitcode = EXIT_SUCCESS;
cleanup:
    record_deinit();
    acq_deinit();
    rep_deinit();

    return exitcode;
}
