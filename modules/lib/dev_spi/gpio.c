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
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>
#include "interface/report.h"


#define GPIO_SYSFS_EXPORT           "/sys/class/gpio/export"
#define GPIO_SYSFS_DIRECTION        "/sys/class/gpio/gpio%d/direction"
#define GPIO_SYSFS_EDGE             "/sys/class/gpio/gpio%d/edge"
#define GPIO_SYSFS_VALUE            "/sys/class/gpio/gpio%d/value"


int gpio_init(gpio_t* gpio, int channel, int direction)
{
    char buffer[256];
    int fd = -1;

    if((fd = open(GPIO_SYSFS_EXPORT, O_WRONLY)) < 0)
    {
       rep_err("Error exporting gpio\n");
       return -1;
    }

    int len = sprintf(buffer, "%d", channel);
    write(fd, buffer, len);
    close(fd);

    len = sprintf(buffer, GPIO_SYSFS_DIRECTION, channel);
    if((fd = open(buffer, O_WRONLY)) < 0)
    { 
       rep_err("Error setting direction of gpio (%d)\n", channel);
       return -1;
    }
    if(direction)
       write(fd, "out", 4);
    else
       write(fd, "in", 3);
    close(fd);

    if(!direction) {
      sprintf(buffer, GPIO_SYSFS_EDGE, channel);
      if((fd = open(buffer, O_WRONLY)) < 0)
      { 
         rep_err("Error setting edge gpio\n");
         return -1;
      }
      write(fd, "rising", 7);
      close(fd);
    }

    sprintf(buffer, GPIO_SYSFS_VALUE, channel);

    int flag = O_RDONLY;
    if(direction)
        flag = O_RDWR;

    if((gpio->fd = open(buffer, flag)) < 0)
    { 
       rep_err("Error opening gpio %d\n", channel);
       return -1;
    }

    gpio->channel = channel;
    gpio->direction = direction;
    return 0;
}


int gpio_read(gpio_t* gpio)
{
    char buffer[3];
    read(gpio->fd, buffer, 3);
    lseek(gpio->fd, 0, SEEK_SET);
    if(buffer[0]=='0')
        return 0;
    else
        return 1;
}

int gpio_write(gpio_t* gpio, int value)
{
    ssize_t size = 2, wsize = 0;
    if(!gpio->direction)
        return -1;

    if(value){
        wsize = write(gpio->fd, "1", size);
    }
    else {
        wsize = write(gpio->fd, "0", size);
    }

    if(size == wsize)
        return 0;

    return -1;
}

int gpio_wait_interrupt(gpio_t* gpio)
{
    struct pollfd pfd;
    if(gpio->direction)
        return -1;

    pfd.fd =  gpio->fd;
    pfd.events = POLLPRI;
    pfd.revents = 0;

    int r = poll(&pfd, 1, -1);
    gpio_read(gpio); // consume

    return r;
}