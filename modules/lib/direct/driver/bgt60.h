/******************************************************************************
 * Copyright (C) 2014-2021 Infineon Technologies AG
 * All rights reserved.
 ******************************************************************************
 * This software contains proprietary information of Infineon Technologies AG.
 * Passing on and copying of this software, and communication of its contents
 * is not permitted without Infineon's prior written authorisation.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef BGT60_H_
#define BGT60_H_

#include <stdint.h>
#include <stdbool.h>

#include "driver/bgt60_regs.h"
#include "interface/bgt60_platform.h"
#define BGT60_STATUS_OK           (0)
#define BGT60_STATUS_INIT_ERROR   (-1)
#define BGT60_STATUS_SPI_ERROR    (-2)
#define BGT60_STATUS_CHIPID_ERROR (-3)
#define BGT60_STATUS_PARAM_ERROR  (-4)

#define BGT60_RESET_SW            (0x000002)
#define BGT60_RESET_FSM           (0x000004)
#define BGT60_RESET_FIFO          (0x000008)

#define BGT60_BYTE_SIZE_FACTOR(x) ((3 * (x)) / 2)
#define BGT60_FIFO_SIZE     (8192)
#define BGT_SPI_HW   0
#define BGT_CS_N 8
#define BGT_SPI_DATA_RDY 18
#define BGT_LDO_EN 21
#define BGT_RST_N 17
#define POL_PHA 0b00
#define BGT_SPI_CONF (0|POL_PHA)

typedef int32_t (*bgt60_spi_transfer_fptr_t)(uint8_t *tx_data, uint8_t *rx_data, uint32_t bytes);
typedef void (*bgt60_reset_fptr_t)(void);

typedef struct bgt60_dev
{
    bgt60_spi_transfer_fptr_t spi_transfer;
    bgt60_reset_fptr_t reset;
    uint16_t slice_size;
} bgt60_dev_t;

typedef struct bgt60_config
{
    const uint32_t *regs;
    uint16_t num_samples_per_chirp;
    uint16_t num_chirps_per_frame;
    uint8_t num_rx_antennas;
} bgt60_config_t;

#ifdef __cplusplus
extern "C" {
#endif

int32_t bgt60_init(bgt60_dev_t *const dev, const uint32_t *const regs);
int32_t bgt60_set_reg(bgt60_dev_t *const dev, uint32_t reg_addr, uint32_t data);
int32_t bgt60_get_reg(bgt60_dev_t *const dev, uint32_t reg_addr, uint32_t *const data);
int32_t bgt60_get_fifo_data(bgt60_dev_t *const dev, uint8_t *const data);
int32_t bgt60_frame_start(bgt60_dev_t *const dev, bool start);
int32_t bgt60_soft_reset(bgt60_dev_t *const dev, int32_t reset_type);
int32_t bgt60_enable_data_test_mode(bgt60_dev_t *const dev, bool enable);

#ifdef __cplusplus
}
#endif

#endif
