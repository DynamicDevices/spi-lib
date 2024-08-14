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

#include <stddef.h>
#include "bgt60.h"

#ifdef _MSC_VER
#include <windows.h>
static int usleep(uint64_t usec) {
    HANDLE timer; 
    LARGE_INTEGER ft; 

    // Convert to 100 nanosecond interval, negative value indicates relative time
    ft.QuadPart = -(10*usec);

    timer = CreateWaitableTimer(NULL, TRUE, NULL); 
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
    WaitForSingleObject(timer, INFINITE); 
    CloseHandle(timer); 
}
#else
#include <unistd.h>
#endif


#include "interface/report.h"

#define BGT60TR13C_CHIPID               (0x000303)
#define BGT60TR13D_CHIPID               (0x000303)

#define BGT60_SPI_WR_OP_MSK             (0x01000000UL)
#define BGT60_SPI_WR_OP_POS             (24UL)
#define BGT60_SPI_GSR0_MSK              (0x0F000000UL)
#define BGT60_SPI_GSR0_POS              (24UL)
#define BGT60_SPI_REGADR_MSK            (0xFE000000UL)
#define BGT60_SPI_REGADR_POS            (25UL)
#define BGT60_SPI_DATA_MSK              (0x00FFFFFFUL)
#define BGT60_SPI_DATA_POS              (0UL)
#define BGT60_SPI_BURST_MODE_CMD        (0xFF000000UL)
#define BGT60_SPI_BURST_MODE_SADR_MSK   (0x00FE0000UL)
#define BGT60_SPI_BURST_MODE_SADR_POS   (17UL)
#define BGT60_SPI_BURST_MODE_RWB_MSK    (0x00010000UL)
#define BGT60_SPI_BURST_MODE_RWB_POS    (16UL)
#define BGT60_SPI_BURST_MODE_LEN_MSK    (0x0000FE00UL)
#define BGT60_SPI_BURST_MODE_LEN_POS    (9UL)
#define BGT60_SPI_BURST_MODE_SADR_FIFO  (0x60)

static uint32_t htonl(uint32_t x)
{
    //return x;
    return (((x & 0x000000ffUL) << 24) |
            ((x & 0x0000ff00UL) << 8) |
            ((x & 0x00ff0000UL) >> 8) |
            ((x & 0xff000000UL) >> 24));
}
int32_t bgt60_init(bgt60_dev_t *const dev, const uint32_t *const regs)
{
    int32_t status;
    uint32_t chipid;
    uint32_t tmp;
    if ((dev == NULL) || (regs == NULL))
    {
        rep_msg("DEV | regs NULL\n");    
        return BGT60_STATUS_PARAM_ERROR;
    }
    dev->reset();
    bgt60_soft_reset(dev, BGT60_RESET_FSM);
    status = bgt60_set_reg(dev, BGT60_REG_SFCTL, 0x102000);
    if (status != 0)
    {
        rep_msg( "error set freq\n");
        return status;
    }
    
    status = bgt60_get_reg(dev, BGT60_REG_SFCTL, &chipid);
    if (status != 0)
    {
        rep_msg( "error getting spi/fifo ctrl\n");
        return status;
    }
    
    status = bgt60_get_reg(dev, BGT60_REG_CHIP_ID, &chipid);
    rep_msg("get status chipid %d  excepted id : %08x ret : %08x\n",status,BGT60TR13C_CHIPID,chipid);

    if (chipid != BGT60TR13C_CHIPID)
    {
        status = bgt60_get_reg(dev, BGT60_REG_CHIP_ID, &chipid);
    }
    if (chipid != BGT60TR13C_CHIPID)
    {
        return BGT60_STATUS_CHIPID_ERROR;
    }

    int reg_idx= 0;
    while (regs[reg_idx] != 0xFFFFFFFF)
    {
        status = bgt60_set_reg(dev, (regs[reg_idx] & BGT60_SPI_REGADR_MSK) >> BGT60_SPI_REGADR_POS, (regs[reg_idx] & BGT60_SPI_DATA_MSK) >> BGT60_SPI_DATA_POS);
        if (status != 0)
        {
            rep_msg("ERROR  on writing list... %x \n",reg_idx);
            break;
        }
        usleep(1000);
        reg_idx++;
    }

    if (status == BGT60_STATUS_OK)
    {
	status = bgt60_get_reg(dev, BGT60_REG_SFCTL, &tmp);
        if (status == 0)
        {
            dev->slice_size = ((tmp & BGT60_REG_SFCTL_FIFO_CREF_MSK) >> BGT60_REG_SFCTL_FIFO_CREF_POS) + 1;
            rep_msg("slice size %d \n", dev->slice_size);
        }
        else
        {
            dev->slice_size = 0;
            rep_msg("ERROR get slice size ==0 \n");
        }
    }
    else
    {
        rep_msg("Failed to set the slice size !! \n");
    }

    return status;
}

int32_t bgt60_set_reg(bgt60_dev_t *const dev, uint32_t reg_addr, uint32_t data)
{
    int32_t status;

    if (dev == NULL)
    {
        return BGT60_STATUS_PARAM_ERROR;
    }

    reg_addr = (reg_addr << BGT60_SPI_REGADR_POS) & BGT60_SPI_REGADR_MSK;
    reg_addr |= BGT60_SPI_WR_OP_MSK;
    reg_addr |= (data << BGT60_SPI_DATA_POS) & BGT60_SPI_DATA_MSK;

    reg_addr = htonl(reg_addr);
    status = dev->spi_transfer((uint8_t *)&reg_addr, NULL, 4);

    return status;
}

int32_t bgt60_get_reg(bgt60_dev_t *const dev, uint32_t reg_addr, uint32_t *const data)
{
    int32_t status;

    if (dev == NULL)
    {
        return BGT60_STATUS_PARAM_ERROR;
    }

    reg_addr = (reg_addr << BGT60_SPI_REGADR_POS) & BGT60_SPI_REGADR_MSK;

    reg_addr = htonl(reg_addr);
    status = dev->spi_transfer((uint8_t *)&reg_addr, (uint8_t *)data, 4);

    if (status == 0)
    {
        *data = htonl(*data);//bgt60_platform_ntohl
        *data &= BGT60_SPI_DATA_MSK;
    }

    return status;
}

int32_t bgt60_get_fifo_data(bgt60_dev_t *const dev, uint8_t *const data)
{
    uint32_t status;
    uint32_t reg_addr;

    if ((dev == NULL) || (data == NULL))
    {
        return BGT60_STATUS_PARAM_ERROR;
    }

    reg_addr = htonl( BGT60_SPI_BURST_MODE_CMD | 
                     (BGT60_SPI_BURST_MODE_SADR_FIFO << BGT60_SPI_BURST_MODE_SADR_POS));

    // IRQ FIFO interrupt is generated based on FIFO words. 
    // One FIFO word of 24 bit captures two ADC samples of 12 bit. 
    // For dual and quad ADC operation all samples result in 1 or more FIFO words. 
    // In single ADC mode, if an odd number of samples are selected, the FIFO interrupt will be generated after the following (even) sample. 
    // For single channel mode an even number of samples is recommended.
    status = dev->spi_transfer((uint8_t *)&reg_addr, data, 4 + (dev->slice_size * 3));

    return status;
}

int32_t bgt60_frame_start(bgt60_dev_t *const dev, bool start)
{
    uint32_t tmp=0;
    int32_t status;

    if (dev == NULL)
    {
        return BGT60_STATUS_PARAM_ERROR;
    }

    if (start)
    {
        status = bgt60_get_reg(dev, BGT60_REG_MAIN, &tmp);
        if (status != 0)
        {
            return status;
        }

        tmp |= BGT60_REG_MAIN_FRAME_START_MSK;
        status = bgt60_set_reg(dev, BGT60_REG_MAIN, tmp);
    }
    else
    {
        /* Stop chirp generation */
        status = bgt60_soft_reset(dev, BGT60_RESET_FSM);
    }

    return status;
}

int32_t bgt60_soft_reset(bgt60_dev_t *const dev, int32_t reset_type)
{
    uint32_t tmp = 0;
    int32_t status;

    if (dev == NULL)
    {
        return BGT60_STATUS_PARAM_ERROR;
    }

    status = bgt60_get_reg(dev, BGT60_REG_MAIN, &tmp);
    if (status != 0)
    {
        return status;
    }
    tmp |= reset_type;

    status = bgt60_set_reg(dev, BGT60_REG_MAIN, tmp);
    return status;
}

int32_t bgt60_enable_data_test_mode(bgt60_dev_t *const dev, bool enable)
{
    uint32_t tmp;
    int32_t status;

    if (dev == NULL)
    {
        return BGT60_STATUS_PARAM_ERROR;
    }

    status = bgt60_get_reg(dev, BGT60_REG_SFCTL, &tmp);
    if (status != 0)
    {
        return status;
    }

    if (enable)
    {
        tmp |= BGT60_REG_SFCTL_LFSR_EN_MSK;
    }
    else
    {
        tmp &= (uint32_t)~BGT60_REG_SFCTL_LFSR_EN_MSK;
    }

    status = bgt60_set_reg(dev, BGT60_REG_SFCTL, tmp);
    return status;

}
