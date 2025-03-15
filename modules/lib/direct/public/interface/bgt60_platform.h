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

#ifndef BGT60_PLATFORM_H_
#define BGT60_PLATFORM_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t bgt60_platform_init();
extern int32_t bgt60_platform_deinit();

extern int32_t bgt60_platform_spi_init(void);

extern int32_t bgt60_platform_spi_transfer(uint8_t *tx_data, uint8_t *rx_data, uint32_t bytes);

extern void bgt60_platform_reset(void);

extern int32_t bgt60_platform_wait_interrupt(void);
#ifdef __cplusplus
}
#endif

#endif