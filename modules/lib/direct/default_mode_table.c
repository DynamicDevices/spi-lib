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

#include "direct.h"
#include <string.h>

static const uint32_t regs_landscape_460MHz[] =
/* BGT60 register settings for segmentation */
{
    0x011E8270,
    0x030A0210,
    0x09E967FD,
    0x0B0805B4,
    0x0D102FFF,
    0x0F010700,
    0x11000000,
    0x13000000,
    0x15000000,
    0x17000BE0,
    0x19000000,
    0x1B000000,
    0x1D000000,
    0x1F000B60,
    0x21133C51,
    0x235FF41F,
    0x25706F7B,
    0x2D000490,
    0x3B000480,
    0x49000480,
    0x57000480,
    0x5911BE0E,
    0x5B65AC0A,
    0x5D03F000,
    0x5F787E1E,
    0x61F4A65A,
    0x63000099,
    0x650002B2,
    0x67000080,
    0x69000000,
    0x6B000000,
    0x6D000000,
    0x6F253B10,
    0x7F000100,
    0x8F000100,
    0x9F000100,
    0xAB000000,
    0xAD000000,
    0xB7000000,
    0xFFFFFFFF
};

static const uint32_t regs_landscape_1GHz[] =
/* BGT60 register settings for segmentation 1GHz bandwidth */
{ 
    0x11e8270, 
    0x30a0210, 
    0x9e967fd, 
    0xb0805b4, 
    0xd102fff, 
    0xf010700, 
    0x11000000, 
    0x13000000, 
    0x15000000, 
    0x17000be0, 
    0x19000000, 
    0x1b000000, 
    0x1d000000, 
    0x1f000b60, 
    0x21133c51, 
    0x235ff41f, 
    0x25706f7b, 
    0x2d000490, 
    0x3b000480, 
    0x49000480, 
    0x57000480, 
    0x5911be0e, 
    0x5b65ac0a, 
    0x5d03f000, 
    0x5f787e1e, 
    0x61e7cd4a, 
    0x63000131, 
    0x650002b2, 
    0x67000080, 
    0x69000000, 
    0x6b000000, 
    0x6d000000, 
    0x6f253b10, 
    0x7f000100, 
    0x8f000100, 
    0x9f000100, 
    0xab000000, 
    0xad000000, 
    0xb7000000,
    0xFFFFFFFF
};

direct_mode_description_t
direct_device_default_mode_table[] =
{
    {
        "landscape", "landscape using 460MHz of bandwidth",
        .num_antennas = 2,
        .regs = regs_landscape_460MHz,
        {
            .num_samples_per_chirp = 128,
            .num_chirps_per_frame = 64,
            .bandwidth_Hz = (ifx_Float_t)460000000u,
            .center_frequency_Hz = (ifx_Float_t)61250000000u,
            .orientation = IFX_ORIENTATION_LANDSCAPE,
        }
    },
    {
        "landscape-1ghz", "landscape using 1GHz of bandwidth",
        .num_antennas = 2,
        .regs = regs_landscape_1GHz,
        {
            .num_samples_per_chirp = 128,
            .num_chirps_per_frame = 64,
            .bandwidth_Hz = (ifx_Float_t)1000000000u,
            .center_frequency_Hz = (ifx_Float_t)61000000000u,
            .orientation = IFX_ORIENTATION_LANDSCAPE,
        }
    },
};

const direct_mode_description_t*
direct_device_default_mode_find(const char* name)
{
    const direct_mode_description_t *table =
        direct_device_default_mode_table;
    const size_t table_size = 
        sizeof(direct_device_default_mode_table) / sizeof(direct_device_default_mode_table[0]);

    for (size_t l = 0; l < table_size; l++)
    {
        if (strcmp(name, table[l].specifier) == 0) {
            return &table[l];
        }
    }

    return NULL;
}

