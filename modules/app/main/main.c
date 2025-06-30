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
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "interface/report.h"
#include "interface/acquisition.h"
#include "interface/record.h"
#include "interface/app_utils.h"
#include "interface/app_argparse.h"
#include "ifxBase/Error.h"

#include "ifxAvian/Avian.h"
#include "ifxRadarPresenceSensing/PresenceSensing.h"

#define FIFO_PATH "/tmp/presence"
#define FIFO_OUTPUT_INTERVAL_SECS 5.0f

typedef struct
{
    ifx_Presence_Sensing_t* handle;
    ifx_Presence_Sensing_Result_t result;
} presence_t;

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

void set_realtime_prio(){
    pthread_t this_thread = pthread_self(); // operates in the current running thread
    struct sched_param params;
    int ret;
    
    // set max prio 
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);    
    ret = pthread_setschedparam(this_thread, SCHED_FIFO, &params);

    if(ret != 0){
	perror("Unsuccessful in setting thread realtime prio\n");
    }

}

int m_max_spi_hz = 25000000;

int main(int argc, char* argv[])
{   
    int exitcode = EXIT_FAILURE;
    int fifo_fd;
    char buf [64];
    time_t start, end;
    double elapsed;
    bool debugging = false;
    float sensitivity_threshold = 1.0f;
    float range_min = 0.2f;
    float range_max = 7.0f;

    rep_msg("Using alternate antenna for testing\n");
    
    char *debug = getenv("RADAR_DEBUG");
    if(debug != NULL) {
        if(!strcmp(getenv("RADAR_DEBUG"), "1")){
            rep_msg("Debugging On\n");
            debugging = true;
        }
    }

    char *spi_speed_hz = getenv("RADAR_SPI_SPEED");
    if(spi_speed_hz != NULL) {
        m_max_spi_hz = atoi(getenv("RADAR_SPI_SPEED"));
    }
    rep_msg("SPI speed %d\n", m_max_spi_hz);

    char *sensitivity = getenv("RADAR_SENSITIVITY");
    if(sensitivity != NULL) {
        sensitivity_threshold = atof(getenv("RADAR_SENSITIVITY"));
    }
    rep_msg("Using sensitivity setting %f\n", sensitivity_threshold);

    char *env_var_range_min = getenv("RADAR_RANGE_MIN");
    if(env_var_range_min != NULL) {
        range_min = atof(getenv("RADAR_RANGE_MIN"));
    }
    rep_msg("Using range min. setting %f\n", range_min);


    char *env_var_range_max = getenv("RADAR_RANGE_MAX");
    if(env_var_range_max != NULL) {
        range_max = atof(getenv("RADAR_RANGE_MAX"));
    }
    rep_msg("Using range max. setting %f\n", range_max);

    set_realtime_prio();

    rep_init();
    acq_init();
    record_init();
    presence_t presence_ctx;

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

    // Presence implementation

    rep_msg("Setup presence sensing\n");

    ifx_Avian_Config_t sensor_config;
    ifx_Presence_Sensing_Config_t presence_config;
    ifx_Presence_Sensing_t* presence_handle;

    rep_msg("Get defaults\n");

    ifx_presence_sensing_get_config_defaults(IFX_AVIAN_BGT60TR13C, &sensor_config, &presence_config);

    presence_config.sensitivity_threshold = sensitivity_threshold;
    presence_config.min_detection_range_m = range_min;
    presence_config.max_detection_range_m = range_max;

    rep_msg("Create\n");

    presence_handle = ifx_presence_sensing_create(&sensor_config, &presence_config);   

    rep_msg("Create done\n");

    ///////// implementation done

    // Create FIFO
    mkfifo(FIFO_PATH, 0666);

    rep_mark_processing_start();

    // Log startup status to FIFO
    if ((fifo_fd = open (FIFO_PATH, O_WRONLY | O_NONBLOCK)) >= 0) {
        snprintf(buf, sizeof(buf), "STATUS Starting up\n");
        write(fifo_fd, buf, strlen(buf));
        close(fifo_fd);
    }

    time(&start);  /* start the FIFO output timer */

    while (!abort_requested())
    {
        ifx_Presence_Sensing_Result_t result;
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
        /*rep_msg("%d %d %d \n",radar_data_frame->shape[0],radar_data_frame->shape[1],radar_data_frame->shape[2]);
        rep_msg("%f %f %f %f", 
            IFX_CUBE_AT(radar_data_frame, 0, 0, 0), 
            IFX_CUBE_AT(radar_data_frame, 0, 1, 0),
            IFX_CUBE_AT(radar_data_frame, 0, 2, 0),
            IFX_CUBE_AT(radar_data_frame, 0, 3, 0));*/

        ifx_presence_sensing_run(presence_handle, radar_data_frame,
                &result);

        if(debugging) {
            rep_msg("%d %f\n", result.target_state, result.target_distance_m);
        } else {
            time(&end);
            elapsed = difftime(end, start);
            if (elapsed > FIFO_OUTPUT_INTERVAL_SECS) {
                // Rese time
                time(&start);
            
                // We reopen the FIFO each time here as we can't open it for non-blocking writes unless the reader already opened it non-blocking
                if ((fifo_fd = open (FIFO_PATH, O_WRONLY | O_NONBLOCK)) >= 0) {
                    snprintf(buf, sizeof(buf), "%d %f\n",  result.target_state, result.target_distance_m);
                    write(fifo_fd, buf, strlen(buf));
                    close(fifo_fd);
                }
            }
        }

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

    // Log exit status to FIFO
    if ((fifo_fd = open (FIFO_PATH, O_WRONLY | O_NONBLOCK)) >= 0) {
        snprintf(buf, sizeof(buf), "STATUS App exit\n");
        write(fifo_fd, buf, strlen(buf));
        close(fifo_fd);
    }
    
    return exitcode;
}
