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

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "interface/bgt60_platform.h"
#include "spi.h"
#include "gpio.h"
#include <unistd.h>
#include <interface/report.h>

/*******************************************************************************
* Macros
*******************************************************************************/

/*******************************************************************************
* Global Variables
*******************************************************************************/

#define IMX_GPIO_PIN(bank, pin) ((bank * 32) + (pin & 0x1f))

#define BANK_RST 3
#define BANK_IRQ 3

#define PIN_RST 28
#define PIN_IRQ 29

#define INPUT 0
#define OUTPUT 1

#define LO 0
#define HI 1

const uint32_t MAX_BUF = 4096;
const uint32_t BIG_MAX_BUF = 3 * MAX_BUF;

spi_t spi = {0};
gpio_t gpio_int = {0};
gpio_t gpio_rst = {0};
char const* spi_dev = "/dev/spidev1.0";

/*******************************************************************************
 * Local functions
 */
 
 int32_t bgt60_platform_init()
 {
    int status = gpio_init(&gpio_int, IMX_GPIO_PIN(BANK_IRQ, PIN_IRQ), INPUT);
    if(status < 0) {
        rep_err("Failed init interrupt gpio pin (%d) \n", status);
        return status;
    }

    status = gpio_init(&gpio_rst, IMX_GPIO_PIN(BANK_RST, PIN_RST), OUTPUT);
    if(status < 0) {
        rep_err("Failed init reset gpio pin (%d)\n", status);
        return status;
    }

    status = gpio_write(&gpio_rst, HI);
    if(status < 0) {
        rep_err("Failed set reset gpio state (%d)\n", status);
        return status;
    }

    status = bgt60_platform_spi_init();
    if(status < 0) {
        rep_err("Failed init spi (%d)\n", status);
        return status;
    }

    status = gpio_read(&gpio_int);
    if(status < 0) {
        rep_err("Failed read interrupt status (%d)\n", status);
        return status;
    }

    return 0;
}

int32_t bgt60_platform_deinit()
{
    spi_close(&spi);
    return 0;
}  

/*******************************************************************************
* Function Name: bgt60_platform_spi_init
********************************************************************************
* Summary
*
* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int32_t bgt60_platform_spi_init(void)
{

    int status = spi_open(spi_dev, &spi);

    if(status == 0)
        status = spi_configure(&spi, 40000000, 8, 0);

    return status;
}

/*******************************************************************************
* Function Name: spi_transfer
********************************************************************************
* Summary:
* This is the interrupt handler function form the data ready pin / timer interrupt.
*
* Parameters:
*    callback_arg    Arguments passed to the interrupt callback
*    event            Timer/counter interrupt triggers
*
*******************************************************************************/
int32_t bgt60_platform_spi_transfer(uint8_t *tx_data, uint8_t *rx_data, uint32_t bytes)
{
    int32_t status = (int32_t)spi_transfer(&spi, rx_data, tx_data, bytes);
    if(status < 0)
        return status;

    return 0;
}

/*******************************************************************************
* Function Name: bgt60_platform_reset
********************************************************************************
* Summary:
* This is the interrupt handler function for the timer interrupt.
*
* Parameters:
*    callback_arg    Arguments passed to the interrupt callback
*    event            Timer/counter interrupt triggers
*
*******************************************************************************/
void bgt60_platform_reset(void)
{
    gpio_write(&gpio_rst, 0);
    usleep(10000);
    gpio_write(&gpio_rst, 1);
    usleep(100000);
}

int32_t bgt60_platform_wait_interrupt(void)
{
    return (int32_t)gpio_wait_interrupt(&gpio_int);
}
