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

#ifndef DWT_H
#define DWT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 */
static inline void enable_cycle_counter(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL = 0;
  DWT->CYCCNT = 0; // reset the counter
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // enable the counter
}

/**
 *
 */
static inline uint32_t read_cycle_counter(void)
{
  return DWT->CYCCNT;
}


#ifdef __cplusplus
}
#endif

#endif

