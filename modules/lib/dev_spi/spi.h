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

/**
 * @file spi.h
 *
 * @brief Spi interface
 * 
 */

#ifndef SPI_H
#define SPI_H
#include <stdint.h>
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct spi_s
{
    int fd;
    uint32_t speed_hz;
    uint8_t bits_per_word;
    uint8_t mode;
}spi_t;


/**
* \brief Opens and configures spi using ioctl.

* \param[in] device  The size in number of bytes for each spi transaction.
* \param[out] spi    Structure saving file descriptor and configuration.

* \return 0 on succses, -1 indicates error 
*/
int spi_open(char const* device, spi_t* spi);

/**
* \brief Configures spi device
*
* \param[in,out] spi  Structure saving file descriptor and configuration.
* \param[in] speed_hz Speed in Hz
* \param[in] bits_per_words Number of bits transfered each transaction
* \param[in] mode Mode of the operation
*
* \return 0 on succses, -1 on error 
*/
int spi_configure(spi_t* spi, uint32_t speed_hz, uint8_t bits_per_word, uint8_t mode);

/**
* \brief Initiates spi transfer
*
* \param[in] spi  Structure saving file descriptor and configuration.
* \param[in] read_buf Buffer MOSI data will be received to.
* \param[in] write_buf Buffer MISO data will be written from.
* \param[in] count Number of bytes to transfer.
*
* \return 0 on succses, -1 on error 
*/
int spi_transfer(spi_t* spi, uint8_t* read_buf, uint8_t* write_buf, uint32_t count);

/**
* \brief Initiates spi read
*
* \param[in] spi  Structure saving file descriptor and configuration.
* \param[in] read_buf Buffer MOSI transaction will be received to.
* \param[in] count Number of bytes to read.
*
* \return 0 on succses, -1 on error 
*/
int spi_read(spi_t* spi, uint8_t* read_buf, uint32_t count);

/**
* \brief Closes the device file handle.
*/
void spi_close(spi_t* spi);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif