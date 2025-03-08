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

#include "ifxAvian/Avian.h"
#include "ifxRadarPresenceSensing/PresenceSensing.h"

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

    rep_msg("rep_init()");

    rep_init();

    rep_msg("acq_init()");

    acq_init();

    rep_msg("record_init()");

    record_init();

    rep_msg("app_parse_opts()");

    if(! app_parse_opts(argdesc, argc, argv))
        goto cleanup;

    rep_msg("ifx_error_get()");

    if (ifx_error_get() != IFX_OK)
        goto cleanup;

    rep_msg("record_start()");

    if (!record_start())
        goto cleanup;

    rep_msg("acq_start()");

    if(!acq_start()) {
        rep_err("failed to start data acquisition\n");
        goto cleanup;
    }

    install_abort_request_signal_handler();

    // Presence implementation

    rep_msg("Setup presence sensing\n");

    ifx_Avian_Config_t sensor_config;
    ifx_Presence_Sensing_Config_t presence_config;
    ifx_Presence_Sensing_t* presence_handle;

    rep_msg("Get defaults\n");

    ifx_presence_sensing_get_config_defaults(IFX_AVIAN_BGT60TR13C, &sensor_config, &presence_config);

    rep_msg("Create\n");

    presence_handle = ifx_presence_sensing_create(&sensor_config, &presence_config);    

    rep_msg("Create done\n");

    rep_mark_processing_start();

    rep_msg("Processing starting\n");

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

#if 1
        rep_msg("%f %f %f %f\n", 
            IFX_CUBE_AT(radar_data_frame, 0, 0, 0), 
            IFX_CUBE_AT(radar_data_frame, 0, 1, 0),
            IFX_CUBE_AT(radar_data_frame, 0, 2, 0),
            IFX_CUBE_AT(radar_data_frame, 0, 3, 0)); 
        rep_msg("Rows: %d Cols %d Slices %d\n", 
            IFX_CUBE_ROWS(radar_data_frame), 
            IFX_CUBE_COLS(radar_data_frame),
            IFX_CUBE_SLICES(radar_data_frame));
#endif

        // Transform cube
//        ifx_Cube_R_t* t_cube = ifx_cube_create_r(1,16,128);

        rep_msg("Analysis starting\n");

        ifx_Presence_Sensing_Result_t* result;

        ifx_presence_sensing_run(presence_handle, radar_data_frame,
                result);

  //      ifx_cube_destroy_r(t_cube);

//        rep_msg("Analysis done\n");

        rep_msg("Presence sensing result: %d %f\n", 
            result->target_state, result->target_distance_m);

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
