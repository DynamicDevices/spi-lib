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

// disable warnings about unsafe functions with MSVC
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "interface/record.h"
#include "interface/report.h"
#include <stdio.h>

static const char* record_file_path = NULL;
static FILE* s_file = NULL;

static bool set_recfile(const char *v) {
    record_file_path = v;
    return true;
}

static const app_option_t recording_options[] = {
    APP_OPTION_PATH(
        "file",
        "file to which to record the sensor data",
        set_recfile),
    APP_OPTION_END
};

const app_cmdarg_t rec_adesc = { "rec", "record to file", recording_options };

void record_init()
{
	s_file = NULL;
}

void record_deinit()
{
	record_stop();
}

void record_stop()
{
	if (s_file != NULL) {
		fclose(s_file);
		s_file = NULL;
	}
}

bool record_start()
{
	record_stop();

	if(record_file_path == NULL)
		return true;

	s_file = fopen(record_file_path, "w");

	if (s_file == NULL)
	{
		rep_err("Could not open file '%s' to record incoming data", record_file_path);
		return false;
	}

	return true;
}

static bool record_ing()
{
	return (s_file != NULL);
}

extern bool record_radar_frame(ifx_Cube_R_t* frame)
{
	if (!record_ing())
		return true;

	FILE *f = s_file;

	const uint32_t nAnt = IFX_CUBE_ROWS(frame);
	const uint32_t nChirps = IFX_CUBE_COLS(frame);
	const uint32_t nSamples = IFX_CUBE_SLICES(frame);

	for (uint32_t a = 0; a < nAnt; a++) {
		for (uint32_t c = 0; c < nChirps; c++) {
			for (uint32_t s = 0; s < nSamples; s++) {
				const ifx_Float_t v =
					IFX_CUBE_AT(frame, a, c, s);

				if (fprintf(f, "%f\n", v) < 0)
					goto write_error;
			}
			if (fprintf(f, "\n") < 0)
				goto write_error;
		}
	}

	return true;
write_error:
	rep_err("Recording data to file failed.\n");
	return false;
}
