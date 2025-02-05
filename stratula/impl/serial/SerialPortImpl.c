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

#warning Implement serial port support

#include <platform/impl/SerialPortImpl.h>

#include <stdio.h>
#include <impl/chrono.h>
#include <string.h>
//#include <udi_cdc.h>

#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>

/******************************************************************************/
/*Macro Definitions ----------------------------------------------------------*/
/******************************************************************************/
#ifdef USB_DEVICE_HS_SUPPORT
#define USB_ENDPOINT_PACKET_SIZE UDI_CDC_DATA_EPS_HS_SIZE
#else
#define USB_ENDPOINT_PACKET_SIZE UDI_CDC_DATA_EPS_FS_SIZE
#endif

/******************************************************************************/
/*Private/Public Constants ---------------------------------------------------*/
/******************************************************************************/
const char *portname = "/tmp/ttySpiLib";

/******************************************************************************/
/*Private/Public Variables ---------------------------------------------------*/
/******************************************************************************/
int fd = -1;

/******************************************************************************/
/*Private Methods Definition -------------------------------------------------*/
/******************************************************************************/
static uint16_t com_get_data(void *data, uint16_t num_bytes)
{
    ssize_t retval = read(fd, data, num_bytes);
//    printf("Rx: %d\n", retval);
    if(retval < 0) {
        printf("error reading from port: %s\n", strerror(errno));
        return 0;
    }
    return retval;
}

int
set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void
set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
//        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
        tty.c_cc[VTIME] = 0;            // no read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf ("error %d setting term attributes", errno);
}

/******************************************************************************/
/* Public Methods Definition -----------------------------------------------*/
/******************************************************************************/
void SerialPort_Constructor(void)
{
}

sr_t SerialPort_open(uint32_t baudrate)
{
    fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        printf ("error %d opening %s: %s", errno, portname, strerror (errno));
        return E_FAILED;
    }

    if(baudrate != 921600)
    {
        printf ("error must use 921600 baud\n");
        return E_FAILED;
    }

    set_interface_attribs (fd, B921600, 0);  // set speed to 921,600 bps, 8n1 (no parity)
    set_blocking (fd, 0);                // set no blocking

    return E_SUCCESS;
}

bool SerialPort_isOpened(void)
{
    return fd >= 0;
}

sr_t SerialPort_close(void)
{
    close(fd);
    return E_SUCCESS;
}


void SerialPort_clearInputBuffer(void)
{
    uint8_t rxBuffer[32];
    while (com_get_data(rxBuffer, sizeof(rxBuffer)))
        ;
}

void SerialPort_flushOutputBuffer(void)
{
    fsync(fd);
}

sr_t SerialPort_send(const uint8_t data[], uint16_t length)
{
    return write(fd, data, length) == length ? E_SUCCESS : E_FAILED;
}

sr_t SerialPort_receive(uint8_t data[], uint16_t length, uint16_t timeout, bool returnImmediately, uint16_t *count)
{
    sr_t ret                      = E_SUCCESS;
    uint16_t left                 = length;

    const chrono_ticks_t deadline = chrono_get_timepoint(chrono_milliseconds(timeout));
    while (left > 0)
    {
        const uint16_t numReceivedBytes = com_get_data(data, left);
        if ((numReceivedBytes == 0) && returnImmediately)
        {
            break;
        }

        data += numReceivedBytes;
        left -= numReceivedBytes;

        if (chrono_has_passed(deadline))
        {
            ret = E_TIMEOUT;
            break;
        }
    }

    *count = (length - left);
    return ret;
}

sr_t SerialPort_sendString(const char data[])
{
    return SerialPort_send((uint8_t *)data, strlen(data));
}
