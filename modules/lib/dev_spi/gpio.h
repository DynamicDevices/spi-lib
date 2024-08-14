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
 * @file gpio.h
 *
 * @brief GPIO interface
 * 
 */

#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef struct gpio_s
{
    int channel;
    int direction;
    int fd;
} gpio_t;


/**
* \brief Opens and configures gpio using sysfs.
*
* \param[out] gpio   Structure saving file descriptor and configuration.
* \param[in] channel  GPIO channel to open and configure.
* \param[in] direction The direction the channel is configured to. 1 is output, 0 is input.
*
* \return 0 on succses, -1 indicates error 
*/
int gpio_init(gpio_t* gpio, int channel, int direction);

/**
* \brief Read state of given gpio.
*
* \param[in] gpio   Structure saving file descriptor and configuration.
*
* \return 0 if state is low, 1 if state is high, negative number indicates error
*/
int gpio_read(gpio_t* gpio);

/**
* \brief Sets the state of the gpio to given value.
*
* \param[in] gpio   Structure saving file descriptor and configuration.
* \param[in] value  State the gpio is set to. 1 for high, 0 for low.
*
* \return 0 on success, -1 on errror
*/
int gpio_write(gpio_t* gpio, int value);

/**
* \brief Blocking function, waits for rising edge interrupt.
*
* \param[out] gpio   Structure saving file descriptor and configuration.
*
* \return 0 on succses, -1 indicates error 
*/
int gpio_wait_interrupt(gpio_t* gpio);
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif