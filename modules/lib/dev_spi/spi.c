/* ============================================================================
 ** Copyright (C) 2014-2021 Infineon Technologies AG
 ** All rights reserved.
 ** ===========================================================================
 ** This software contains proprietary information of Infineon Technologies AG.
 ** Passing on and copying of this software, and communication of its contents
 ** is not permitted without Infineon's prior written authorisation.
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h> // close function
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include "spi.h"

int spi_open(char const* device, spi_t* spi)
{
    int status = -1;
    spi->fd = open(device, O_RDWR, 0);

    if(spi->fd == -1)
        return -1;

    if ((status = ioctl(spi->fd, SPI_IOC_WR_MODE, &spi->mode))  < 0) 
    {
        return status;
	}

    if ((status = ioctl(spi->fd, SPI_IOC_WR_BITS_PER_WORD, &spi->bits_per_word)) < 0) 
    {
        return status;
	}

    status = ioctl(spi->fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi->speed_hz);

    return status;
}

int spi_configure(spi_t* spi, uint32_t speed_hz, uint8_t bits_per_word, uint8_t mode)
{
    int status;

    if ((status = ioctl(spi->fd, SPI_IOC_WR_MODE, &mode))  < 0) 
    {
        return status;
	}

    spi->mode = mode;
    if ((status = ioctl(spi->fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word)) < 0) 
    {
        return status;
	}
    spi->bits_per_word = bits_per_word;

    if ((status = ioctl(spi->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz)) < 0)
    {
        return status;
    }
    spi->speed_hz = speed_hz;

    return status;
}

int spi_transfer(spi_t* spi, uint8_t* read_buf, uint8_t* write_buf, uint32_t count)
{
    struct spi_ioc_transfer transfer;
    memset(&transfer, 0, sizeof(transfer));

    //compare count with spi.blocksiz 
    transfer.tx_buf = (unsigned long)write_buf;
    transfer.rx_buf = (unsigned long)read_buf;
    transfer.len = count;
    transfer.delay_usecs = 0;
    transfer.speed_hz = spi->speed_hz;
    transfer.bits_per_word = spi->bits_per_word;

    int status = ioctl(spi->fd, SPI_IOC_MESSAGE(1), &transfer);
    return status;
}

int spi_read(spi_t* spi, uint8_t* read_buf, uint32_t count)
{
    return read(spi->fd, read_buf, count);
}

void spi_close(spi_t* spi)
{
    if(spi && spi->fd != -1) 
	{
        close(spi->fd);
    }
    
    spi->fd = -1;
    spi->bits_per_word = 0;
    spi->speed_hz = 0;
    spi->mode = 0;
}