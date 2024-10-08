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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <stdlib.h>
#include <string.h>

#include "ifxBase/Vector.h"
#include "ifxBase/Complex.h"
#include "ifxBase/Mem.h"
#include "ifxBase/Error.h"
#include "ifxBase/Defines.h"
#include "ifxBase/internal/Macros.h"
#include "ifxBase/internal/Util.h"

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

#define IFX_COMPARE_FUNCTION(FN_NAME, TYPE)          \
    int FN_NAME(void* h, uint32_t ia, uint32_t ib) { \
        TYPE a = ((TYPE*)h)[ia];                     \
        TYPE b = ((TYPE*)h)[ib];                     \
        if(a > b) return 1;                          \
        if(a < b) return -1;                         \
        return 0;                                    \
    }

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/

/**
 * @brief Structure used to store vector allocation parameters
 */
typedef struct Vector_alloc_params
{
    const size_t length;
    const size_t elem_size;
    const size_t data_offset;
    size_t alloc_size;
} vector_alloc_params;

/*
==============================================================================
   4. LOCAL DATA
==============================================================================
*/

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

static void ssort(void* handle,
                  uint32_t count,
                  int (*compare)(void* handle, uint32_t ia, uint32_t ib),
                  ifx_Vector_Sort_Order_t order,
                  uint32_t* array);

static IFX_COMPARE_FUNCTION(float_compare, ifx_Float_t);


/**
 * @brief Computes number of higher and lower elements in array for given position.
 *        It takes into consideration element on index pick_pos_offsetted+offset 
 *        as a base for comparison.
 * 
 * Special cases in counting elements:<br>
 * 1) If value is equal and is before pick_pos_offsetted -1 is added.<br/>
 * 2) If value is equal and is after pick_pos_offsetted -1 is added.<br/>
 * Important is that this function return 0 if is median and number of elements is odd
 * -1 or 1 if number of elements is even for values that directly betwean.
 * 
 * /code 
 * Example
 *   input = {3, 2, 1, 7}
 * for pick_pos_offsetted = 1 (value = 2); offset = 0; number_of_el = 4
 *   3, 7 > 2 so +2
 *   1 < 2 so -1
 * Result is +2-1=1
 * /endcode
 * 
 * @param [in]     input            buffer that is used to compute rank
 * @param [in]     offset           start position of computation
 * @param [in]     number_of_el     number of elements to compute rank
 * 
 * @param [in]     pick_pos_offsetted       on this position (offset+pick_pos_offsetted) 
 *                                          is element to compute rank (it is excluded
 *                                          from rank)
 * @retval a rank value
 */
static int median_rank(
    const ifx_Vector_R_t* input, uint32_t offset, 
    uint32_t number_of_el, uint32_t pick_pos_offsetted);

/**
 * @brief Computes the required memory size (in bytes) for vector and checks for overflows
 *
 * The function computes the required size of memory size in bytes
 * alloc_size = length * elem_size for a vector. elem_size is the size
 * of one element of the vector and is typically either sizeof(ifx_Float_t) for
 * a real vector or sizeof(ifx_Complex_t) for a complex vector. data_offset is
 * a size_t offset for data allocation and is typically ALIGN(sizeof(ifx_Vector_R_t))
 * or ALIGN(sizeof(ifx_Vector_C_t))
 *
 *
 * @param [in] length      length of the vector
 * @param [in] elem_size   size of one element of the vector
 * @param [in] data_offset offset for data allocation in memory
 * @param [out] alloc_size size of memory to allocate for the vector in bytes
 * @retval true     if an overflow occurs during multiplication or addition
 * @retval false    if no overflow occurs
 */
static bool vector_alloc_size_overflow(size_t length, size_t elem_size, size_t data_offset, size_t* alloc_size);

/*
==============================================================================
   6. LOCAL FUNCTIONS
==============================================================================
*/

static void ssort(void* handle,
                  uint32_t count,
                  int (*compare)(void* handle, uint32_t ia, uint32_t ib),
                  ifx_Vector_Sort_Order_t order,
                  uint32_t* array)
{
    uint32_t gaps[] = {701, 301, 132, 57, 23, 10, 4, 1}; //tbd
    uint32_t num_gaps = sizeof(gaps) / sizeof(gaps[0]);
    int ord = order == IFX_SORT_DESCENDING ? -1 : 1;

    for (uint32_t i = 0; i < count; i++)
    {
        array[i] = i;
    }

    for (uint32_t g = 0; g < num_gaps; g++)
    {
        uint32_t gap = gaps[g];

        if (gap > count)
        {
            continue;
        }

        for (uint32_t i = gap; i < count; i++ )
        {
            uint32_t temp = array[i];
            uint32_t j;

            for (j = i; j >= gap && (compare(handle, array[j - gap], temp) == ord); j -= gap)
            {
                array[j] = array[j - gap];
            }

            array[j] = temp;
        }
    }
}

static bool vector_alloc_size_overflow(size_t length, size_t elem_size, size_t data_offset, size_t* alloc_size)
{
    if (ifx_util_overflow_mul_size_t(length, elem_size, alloc_size))
        return true;
    return ifx_util_overflow_add_size_t(*alloc_size, data_offset, alloc_size);
}

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

void ifx_vec_init_r(ifx_Vector_R_t* vector,
                    ifx_Float_t* d,
                    uint32_t length)
{
    IFX_ERR_BRK_NULL(vector);
    vector->d = d;
    vector->len = length;
    vector->stride = 1;
    vector->owns_d = (d != NULL);
}

//----------------------------------------------------------------------------

void ifx_vec_init_c(ifx_Vector_C_t* vector,
                    ifx_Complex_t* d,
                    uint32_t length)
{
    IFX_ERR_BRK_NULL(vector);
    vector->d = d;
    vector->len = length;
    vector->stride = 1;
    vector->owns_d = (d != NULL);
}

//----------------------------------------------------------------------------

void ifx_vec_rawview_r(ifx_Vector_R_t* vector,
                       ifx_Float_t* d,
                       uint32_t length,
                       uint32_t stride)
{
    IFX_ERR_BRK_NULL(vector);
    vector->d = d;
    vector->len = length;
    vector->stride = stride;
    vector->owns_d = 0;
}

//----------------------------------------------------------------------------

void ifx_vec_rawview_c(ifx_Vector_C_t* vector,
                       ifx_Complex_t* d,
                       uint32_t length,
                       uint32_t stride)
{
    IFX_ERR_BRK_NULL(vector);
    vector->d = d;
    vector->len = length;
    vector->stride = stride;
    vector->owns_d = 0;
}

//----------------------------------------------------------------------------

void ifx_vec_view_r(ifx_Vector_R_t* vector,
                    ifx_Vector_R_t* source,
                    uint32_t offset,
                    uint32_t length,
                    uint32_t spacing)
{
    IFX_VEC_BRF_VEC_BOUNDS(source, offset);
    IFX_ERR_BRF_COND(spacing < 1, IFX_ERROR_ARGUMENT_INVALID);
    IFX_ERR_BRF_COND(length == 0, IFX_ERROR_ARGUMENT_INVALID);
    // source must have length of at least min_length
    const uint32_t min_length = offset + (length - 1) * spacing + 1;
    IFX_ERR_BRF_COND(vLen(source) < min_length, IFX_ERROR_ARGUMENT_OUT_OF_BOUNDS);

    vector->d = &vAt(source, offset);
    vector->len = length;
    vector->stride = source->stride * spacing;
    vector->owns_d = 0;

    return;
fail:
    vector->d = NULL;
    vector->len = 0;
    vector->stride = 0;
    vector->owns_d = 0;
}

//----------------------------------------------------------------------------

void ifx_vec_view_c(ifx_Vector_C_t* vector,
                    ifx_Vector_C_t* source,
                    uint32_t offset,
                    uint32_t length,
                    uint32_t spacing)
{
    IFX_VEC_BRF_VEC_BOUNDS(source, offset);
    IFX_ERR_BRF_COND(length == 0, IFX_ERROR_ARGUMENT_INVALID);
    IFX_ERR_BRF_COND(spacing < 1, IFX_ERROR_ARGUMENT_INVALID);
    // source must have length of at least min_length
    const uint32_t min_length = offset + (length - 1) * spacing + 1;
    IFX_ERR_BRF_COND(vLen(source) < min_length, IFX_ERROR_ARGUMENT_OUT_OF_BOUNDS);

    vector->d = &vAt(source, offset);
    vector->len = length;
    vector->stride = source->stride * spacing;
    vector->owns_d = 0;

    return;
fail:
    vector->d = NULL;
    vector->len = 0;
    vector->stride = 0;
    vector->owns_d = 0;
}

//----------------------------------------------------------------------------

ifx_Vector_R_t* ifx_vec_create_r(uint32_t length)
{
    ifx_Vector_R_t* vec = NULL;
    size_t data_offset = ALIGN(sizeof(ifx_Vector_R_t));
    size_t alloc_size = 0;
    bool overflow = vector_alloc_size_overflow(length, sizeof(ifx_Float_t), data_offset, &alloc_size);
    IFX_ERR_BRV_COND(overflow, IFX_ERROR_MEMORY_ALLOCATION_FAILED, NULL);

    void* mem = ifx_mem_aligned_alloc(alloc_size, MEMORY_ALIGNMENT);
    IFX_ERR_BRN_MEMALLOC(mem);
    vec = mem;
    ifx_Float_t* data = (void*)((uint8_t*)mem + data_offset);
    memset(data, 0, alloc_size - data_offset);

    ifx_vec_rawview_r(vec, data, length, 1);

    return vec;
}

//----------------------------------------------------------------------------

ifx_Vector_C_t* ifx_vec_create_c(uint32_t length)
{
    ifx_Vector_C_t* vec = NULL;
    size_t data_offset = ALIGN(sizeof(ifx_Vector_C_t));
    size_t alloc_size = 0;
    bool overflow = vector_alloc_size_overflow(length, sizeof(ifx_Complex_t), data_offset, &alloc_size);
    IFX_ERR_BRV_COND(overflow, IFX_ERROR_MEMORY_ALLOCATION_FAILED, NULL);

    void* mem = ifx_mem_aligned_alloc(alloc_size, MEMORY_ALIGNMENT);
    IFX_ERR_BRN_MEMALLOC(mem);
    vec = mem;
    ifx_Complex_t* data = (void*)((uint8_t*)mem + data_offset);
    memset(data, 0, alloc_size - data_offset);

    ifx_vec_rawview_c(vec, data, length, 1);

    return vec;
}

//----------------------------------------------------------------------------

ifx_Vector_R_t* ifx_vec_clone_r(const ifx_Vector_R_t* vector)
{
    IFX_ERR_BRN_NULL(vector);

    ifx_Vector_R_t* result = ifx_vec_create_r(vLen(vector));

    if (result != NULL)
    {
        ifx_vec_copy_r(vector, result);
    }

    return result;
}

//----------------------------------------------------------------------------

ifx_Vector_C_t* ifx_vec_clone_c(const ifx_Vector_C_t* vector)
{
    IFX_ERR_BRN_NULL(vector);

    ifx_Vector_C_t* result = ifx_vec_create_c(vLen(vector));

    if (result != NULL)
    {
        ifx_vec_copy_c(vector, result);
    }

    return result;
}

//----------------------------------------------------------------------------

void ifx_vec_deinit_r(ifx_Vector_R_t* vector)
{
    IFX_ERR_BRK_NULL(vector);

    if (vector->owns_d)
    {
        ifx_mem_aligned_free(vector->d);
    }

    vector->d = NULL;
    vector->len = 0;
    vector->stride = 0;
    vector->owns_d = 0;
}

//----------------------------------------------------------------------------

void ifx_vec_deinit_c(ifx_Vector_C_t* vector)
{
    IFX_ERR_BRK_NULL(vector);

    if (vector->owns_d)
    {
        ifx_mem_aligned_free(vector->d);
    }

    vector->d = NULL;
    vector->len = 0;
    vector->stride = 0;
    vector->owns_d = 0;
}

//----------------------------------------------------------------------------

void ifx_vec_destroy_r(ifx_Vector_R_t* vector)
{
    if (!vector)
    {
        return;
    }

    ifx_vec_deinit_r(vector);
    ifx_mem_aligned_free(vector);
}

//----------------------------------------------------------------------------

void ifx_vec_destroy_c(ifx_Vector_C_t* vector)
{
    if (!vector)
    {
        return;
    }

    ifx_vec_deinit_c(vector);
    ifx_mem_aligned_free(vector);
}

//----------------------------------------------------------------------------

void ifx_vec_blit_r(const ifx_Vector_R_t* vector,
                    uint32_t offset,
                    uint32_t length,
                    uint32_t target_offset,
                    ifx_Vector_R_t* target)
{
    IFX_VEC_BRK_VALID(vector);
    IFX_VEC_BRK_VALID(target);
    IFX_ERR_BRK_COND(length + offset > vLen(vector), IFX_ERROR_ARGUMENT_OUT_OF_BOUNDS);
    IFX_ERR_BRK_COND(length + target_offset > vLen(target), IFX_ERROR_ARGUMENT_OUT_OF_BOUNDS);

    const uint32_t vstride = vStride(vector);
    const uint32_t tstride = vStride(target);
    const ifx_Float_t* vdata = &vAt(vector, offset);
    ifx_Float_t* tdata = &vAt(target, target_offset);

    for (uint32_t i = 0; i < length; i++)
    {
        *tdata = *vdata;
        vdata += vstride;
        tdata += tstride;
    }
}

//----------------------------------------------------------------------------

void ifx_vec_blit_c(const ifx_Vector_C_t* vector,
                    uint32_t offset,
                    uint32_t length,
                    uint32_t target_offset,
                    ifx_Vector_C_t* target)
{
    IFX_VEC_BRK_VALID(vector);
    IFX_VEC_BRK_VALID(target);
    IFX_ERR_BRK_COND(length + offset > vLen(vector), IFX_ERROR_ARGUMENT_OUT_OF_BOUNDS);
    IFX_ERR_BRK_COND(length + target_offset > vLen(target), IFX_ERROR_ARGUMENT_OUT_OF_BOUNDS);

    const uint32_t vstride = vStride(vector);
    const uint32_t tstride = vStride(target);
    const ifx_Complex_t* vdata = &vAt(vector, offset);
    ifx_Complex_t* tdata = &vAt(target, target_offset);

    for (uint32_t i = 0; i < length; i++)
    {
        *tdata = *vdata;
        vdata += vstride;
        tdata += tstride;
    }
}

//----------------------------------------------------------------------------

void ifx_vec_copy_r(const ifx_Vector_R_t* vector,
                    ifx_Vector_R_t* target)
{
    IFX_VEC_BRK_VALID(vector);
    IFX_VEC_BRK_VALID(target);
    IFX_VEC_BRK_DIM(vector, target);
    ifx_vec_blit_r(vector, 0, vLen(vector), 0, target);
}

//----------------------------------------------------------------------------

void ifx_vec_copy_c(const ifx_Vector_C_t* vector,
                    ifx_Vector_C_t* target)
{
    IFX_VEC_BRK_VALID(vector);
    IFX_VEC_BRK_VALID(target);
    IFX_VEC_BRK_DIM(vector, target);
    ifx_vec_blit_c(vector, 0, vLen(vector), 0, target);
}

//----------------------------------------------------------------------------

void ifx_vec_copyshift_r(const ifx_Vector_R_t* vector,
                         uint32_t shift,
                         ifx_Vector_R_t* target)
{
    IFX_VEC_BRK_VALID(vector);
    IFX_VEC_BRK_VALID(target);
    IFX_VEC_BRK_DIM(vector, target);

    const uint32_t length = vLen(vector);

    if (vector != target) //simple copying of segments from source to target vectors
    {
        const uint32_t n2 = length - shift;
        ifx_vec_blit_r(vector, 0, n2, shift, target);
        ifx_vec_blit_r(vector, n2, shift, 0, target);
    }
    else //in-place swapping operation within a single vector using just one temp variable
         //algo reference: http://www.eis.mdx.ac.uk/staffpages/r_bornat/oldteaching/I2A/slides%209%20circshift.pdf
    {
        uint32_t m = 0;
        uint32_t count = 0;

        for (m = 0, count = 0; count != length; m++)
        {
            ifx_Float_t t = vAt(vector, m);
            uint32_t i;
            uint32_t j;

            for (i = m, j = m + shift;
                    j != m;
                    i = j, j = (j + shift < length) ? (j + shift) : (j + shift - length), count++)
            {
                vAt(vector, i) = vAt(vector, j);
            }

            vAt(vector, i) = t;
            count++;
        }
    }
}

//----------------------------------------------------------------------------

void ifx_vec_copyshift_c(const ifx_Vector_C_t* vector,
                         uint32_t shift,
                         ifx_Vector_C_t* target)
{
    IFX_VEC_BRK_VALID(vector);
    IFX_VEC_BRK_VALID(target);
    IFX_VEC_BRK_DIM(vector, target);

    const uint32_t length = vLen(vector);
    const uint32_t n2 = length - shift;

    if (vector != target) //simple copying of segments from source to target vectors
    {
        ifx_vec_blit_c(vector, 0, n2, shift, target);
        ifx_vec_blit_c(vector, n2, shift, 0, target);
    }
    else //in-place swapping operation within a single vector using just one temp variable
        //algo reference: http://www.eis.mdx.ac.uk/staffpages/r_bornat/oldteaching/I2A/slides%209%20circshift.pdf
    {
        uint32_t m = 0;
        uint32_t count = 0;

        for (m = 0, count = 0; count != length; m++)
        {
            ifx_Complex_t t;
            t = vAt(vector, m);
            uint32_t i;
            uint32_t j;

            for (i = m, j = m + shift;
                    j != m;
                    i = j, j = (j + shift < length) ? (j + shift) : (j + shift - length), count++)
            {
                vAt(vector, i) = vAt(vector, j);
            }

            vAt(vector, i) = t;
            count++;
        }
    }
}

//----------------------------------------------------------------------------

void ifx_vec_shift_r(ifx_Vector_R_t* vector,
                     uint32_t shift)
{
    IFX_VEC_BRK_VALID(vector);

    if (shift == 0)
    {
        return;
    }

    ifx_vec_copyshift_r(vector, shift, vector);
}

//----------------------------------------------------------------------------

void ifx_vec_shift_c(ifx_Vector_C_t* vector,
                     uint32_t shift)
{
    IFX_VEC_BRK_VALID(vector);

    if (shift == 0)
    {
        return;
    }

    ifx_vec_copyshift_c(vector, shift, vector);
}

//----------------------------------------------------------------------------

void ifx_vec_setall_r(ifx_Vector_R_t* vector,
                      ifx_Float_t value)
{
    IFX_VEC_BRK_VALID(vector);
    const uint32_t len = vLen(vector);

    for (uint32_t i = 0; i < len; i++)
    {
        vAt(vector, i) = value;
    }
}

//----------------------------------------------------------------------------

void ifx_vec_setall_c(ifx_Vector_C_t* vector,
                      ifx_Complex_t value)
{
    IFX_VEC_BRK_VALID(vector);
    const uint32_t len = vLen(vector);

    for (uint32_t i = 0; i < len; i++)
    {
        vAt(vector, i) = value;
    }
}

//----------------------------------------------------------------------------

void ifx_vec_complex_c(const ifx_Vector_R_t* input_real,
    const ifx_Vector_R_t* input_imag,
    ifx_Vector_C_t* output)
{
    IFX_VEC_BRK_VALID(input_real);
    IFX_VEC_BRK_VALID(input_imag);
    IFX_VEC_BRK_VALID(output);

    IFX_VEC_BRK_DIM(input_real, input_imag);
    IFX_VEC_BRK_DIM(input_real, output);

    const uint32_t len = vLen(input_real);
    for (uint32_t i = 0; i < len; i++)
    {
        IFX_COMPLEX_SET(vAt(output,i), vAt(input_real, i), vAt(input_imag, i));
    }
}

//----------------------------------------------------------------------------

void ifx_vec_set_range_r(ifx_Vector_R_t* vector,
                      uint32_t offset,
                      uint32_t length,
                      ifx_Float_t value)
{
    IFX_VEC_BRK_VALID(vector);
    IFX_VEC_BRK_VEC_BOUNDS(vector, offset);
    IFX_ERR_BRK_COND(length + offset > vLen(vector), IFX_ERROR_ARGUMENT_OUT_OF_BOUNDS);

    for (uint32_t i = offset; i < length; i++)
    {
        vAt(vector, i) = value;
    }
}

//----------------------------------------------------------------------------

void ifx_vec_set_range_c(ifx_Vector_C_t* vector,
                      uint32_t offset,
                      uint32_t length,
                      ifx_Complex_t value)
{
    IFX_VEC_BRK_VALID(vector);
    IFX_VEC_BRK_VEC_BOUNDS(vector, offset);
    IFX_ERR_BRK_COND(length + offset > vLen(vector), IFX_ERROR_ARGUMENT_OUT_OF_BOUNDS);

    for (uint32_t i = offset; i < length; i++)
    {
        vAt(vector, i) = value;
    }
}

//----------------------------------------------------------------------------

void ifx_vec_setat_r(ifx_Vector_R_t* vector,
                     uint32_t idx,
                     ifx_Float_t value)
{
    IFX_VEC_BRK_VALID(vector);
    IFX_VEC_BRK_VEC_BOUNDS(vector, idx);

    vAt(vector, idx) = value;
}

//----------------------------------------------------------------------------

void ifx_vec_setat_c(ifx_Vector_C_t* vector,
                     uint32_t idx,
                     ifx_Complex_t value)
{
    IFX_VEC_BRK_VALID(vector);
    IFX_VEC_BRK_VEC_BOUNDS(vector, idx);

    vAt(vector, idx) = value;
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_vec_sum_r(const ifx_Vector_R_t* vector)
{
    IFX_VEC_BRV_VALID(vector, 0);

    /* Use the Kahan summation algorithm to reduce numerical error.
     *
     * The algorithm is taken from Wikipedia, see
     * https://en.wikipedia.org/wiki/Kahan_summation_algorithm.
     */
    ifx_Float_t sum = 0;
    ifx_Float_t c = 0; /* running compensation for lost low-order bits */

    for (uint32_t i = 0; i < vLen(vector); i++)
    {
        const ifx_Float_t y = vAt(vector,i) - c;
        const ifx_Float_t t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }

    return sum;
}

//----------------------------------------------------------------------------

ifx_Complex_t ifx_vec_sum_c(const ifx_Vector_C_t* vector)
{
    ifx_Complex_t sum;
    IFX_COMPLEX_SET(sum, 0, 0);

    IFX_VEC_BRV_VALID(vector, sum);

    const uint32_t length = vLen(vector);

    for (uint32_t i = 0; i < length; i++)
    {
        sum = ifx_complex_add(sum, vAt(vector, i));
    }

    return sum;
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_vec_sqsum_r(const ifx_Vector_R_t* vector)
{
    IFX_VEC_BRV_VALID(vector, 0);

    ifx_Float_t result = 0.f;
    const uint32_t length = vLen(vector);

    for (uint32_t i = 0; i < length; i++)
    {
        const ifx_Float_t val = vAt(vector, i);
        result += val * val;
    }

    return result;
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_vec_sqsum_c(const ifx_Vector_C_t* vector)
{
    IFX_VEC_BRV_VALID(vector, 0);

    ifx_Float_t result = 0.f;
    const uint32_t length = vLen(vector);

    for (uint32_t i = 0; i < length; i++)
    {
        result = result + ifx_complex_sqnorm(vAt(vector, i));
    }

    return result;
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_vec_maxabs_r(const ifx_Vector_R_t* vector)
{
    IFX_VEC_BRV_VALID(vector, 0);

    const uint32_t length = vLen(vector);
    ifx_Float_t result = FABS(vAt(vector, 0));

    for (uint32_t i = 1; i < length; i++)
    {
        const ifx_Float_t val = FABS(vAt(vector, i));

        if (val > result)
        {
            result = val;
        }
    }

    return result;
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_vec_maxabs_c(const ifx_Vector_C_t* vector)
{
    IFX_VEC_BRV_VALID(vector, 0);

    const uint32_t length = vLen(vector);
    ifx_Float_t max = ifx_complex_sqnorm(vAt(vector, 0));

    for (uint32_t i = 1; i < length; i++)
    {
        const ifx_Float_t val = ifx_complex_sqnorm(vAt(vector, i));

        if (val > max)
        {
            max = val;
        }
    }

    ifx_Float_t result = SQRT(max);

    return result;
}

//----------------------------------------------------------------------------

uint32_t ifx_vec_max_idx_r(const ifx_Vector_R_t* vector)
{
    IFX_VEC_BRV_VALID(vector, 0);

    uint32_t index = 0;
    ifx_Float_t max = (vAt(vector, 0));

    for (uint32_t i = 1; i < vLen(vector); i++)
    {
        const ifx_Float_t val = (vAt(vector, i));

        if (val > max)
        {
            max = val;
            index = i;
        }
    }

    return index;
}
//----------------------------------------------------------------------------

uint32_t ifx_vec_min_idx_r(const ifx_Vector_R_t* vector)
{
    IFX_VEC_BRV_VALID(vector, 0);

    uint32_t index = 0;
    ifx_Float_t min = vAt(vector, 0);

    for (uint32_t i = 1; i < vLen(vector); i++)
    {
        const ifx_Float_t val = vAt(vector, i);

        if (val < min)
        {
            min = val;
            index = i;
        }
    }

    return index;
}
//----------------------------------------------------------------------------

uint32_t ifx_vec_max_idx_c(const ifx_Vector_C_t* vector)
{
    IFX_VEC_BRV_VALID(vector, 0);

    uint32_t index = 0;
    ifx_Float_t max = ifx_complex_sqnorm(vAt(vector, 0));

    for (uint32_t i = 1; i < vLen(vector); i++)
    {
        const ifx_Float_t val = ifx_complex_sqnorm(vAt(vector, i));

        if (val > max)
        {
            max = val;
            index = i;
        }
    }

    return index;
}

//----------------------------------------------------------------------------

void ifx_vec_add_r(const ifx_Vector_R_t* v1,
                   const ifx_Vector_R_t* v2,
                   ifx_Vector_R_t* result)
{
    IFX_VEC_BRK_VALID(v1);
    IFX_VEC_BRK_VALID(v2);
    IFX_VEC_BRK_VALID(result);
    IFX_VEC_BRK_DIM(v1, v2);
    IFX_VEC_BRK_DIM(v1, result);

    for (uint32_t i = 0; i < vLen(v1); ++i)
    {
        vAt(result, i) = vAt(v1, i) + vAt(v2, i);
    }
}

//----------------------------------------------------------------------------

void ifx_vec_add_c(const ifx_Vector_C_t* v1,
                   const ifx_Vector_C_t* v2,
                   ifx_Vector_C_t* result)
{
    IFX_VEC_BRK_VALID(v1);
    IFX_VEC_BRK_VALID(v2);
    IFX_VEC_BRK_VALID(result);
    IFX_VEC_BRK_DIM(v1, v2);
    IFX_VEC_BRK_DIM(v1, result);

    for (uint32_t i = 0; i < vLen(v1); ++i)
    {
        vAt(result, i) = ifx_complex_add(vAt(v1, i), vAt(v2, i));
    }
}

//----------------------------------------------------------------------------

void ifx_vec_sub_r(const ifx_Vector_R_t* v1,
                   const ifx_Vector_R_t* v2,
                   ifx_Vector_R_t* result)
{
    IFX_VEC_BRK_VALID(v1);
    IFX_VEC_BRK_VALID(v2);
    IFX_VEC_BRK_VALID(result);
    IFX_VEC_BRK_DIM(v1, v2);
    IFX_VEC_BRK_DIM(v1, result);

    for (uint32_t i = 0; i < vLen(v1); ++i)
    {
        vAt(result, i) = vAt(v1, i) - vAt(v2, i);
    }
}

//----------------------------------------------------------------------------

void ifx_vec_sub_c(const ifx_Vector_C_t* v1,
                   const ifx_Vector_C_t* v2,
                   ifx_Vector_C_t* result)
{
    IFX_VEC_BRK_VALID(v1);
    IFX_VEC_BRK_VALID(v2);
    IFX_VEC_BRK_VALID(result);
    IFX_VEC_BRK_DIM(v1, v2);
    IFX_VEC_BRK_DIM(v1, result);

    for (uint32_t i = 0; i < vLen(v1); ++i)
    {
        vAt(result, i) = ifx_complex_sub(vAt(v1, i), vAt(v2, i));
    }
}

//----------------------------------------------------------------------------

void ifx_vec_mul_r(const ifx_Vector_R_t* v1,
                   const ifx_Vector_R_t* v2,
                   ifx_Vector_R_t* result)
{
    IFX_VEC_BRK_VALID(v1);
    IFX_VEC_BRK_VALID(v2);
    IFX_VEC_BRK_VALID(result);
    IFX_VEC_BRK_DIM(v1, v2);
    IFX_VEC_BRK_DIM(v1, result);

    for (uint32_t i = 0; i < vLen(v1); ++i)
    {
        vAt(result, i) = vAt(v1, i) * vAt(v2, i);
    }
}

//----------------------------------------------------------------------------

void ifx_vec_mul_c(const ifx_Vector_C_t* v1,
                   const ifx_Vector_C_t* v2,
                   ifx_Vector_C_t* result)
{
    IFX_VEC_BRK_VALID(v1);
    IFX_VEC_BRK_VALID(v2);
    IFX_VEC_BRK_VALID(result);
    IFX_VEC_BRK_DIM(v1, v2);
    IFX_VEC_BRK_DIM(v1, result);

    for (uint32_t i = 0; i < vLen(v1); ++i)
    {
        vAt(result, i) = ifx_complex_mul(vAt(v1, i), vAt(v2, i));
    }
}

//----------------------------------------------------------------------------

void ifx_vec_mul_cr(const ifx_Vector_C_t* v1,
                    const ifx_Vector_R_t* v2,
                    ifx_Vector_C_t* result)
{
    IFX_VEC_BRK_VALID(v1);
    IFX_VEC_BRK_VALID(v2);
    IFX_VEC_BRK_VALID(result);
    IFX_VEC_BRK_DIM(v1, v2);
    IFX_VEC_BRK_DIM(v1, result);

    for (uint32_t i = 0; i < vLen(v1); ++i)
    {
        vAt(result, i) = ifx_complex_mul_real(vAt(v1, i), vAt(v2, i));
    }
}

//----------------------------------------------------------------------------

void ifx_vec_abs_r(const ifx_Vector_R_t* input,
                   ifx_Vector_R_t* output)
{
    IFX_VEC_BRK_DIM(input, output);

    for (uint32_t i = 0; i < vLen(input); ++i)
    {
        vAt(output, i) = FABS(vAt(input, i));
    }
}

//----------------------------------------------------------------------------

void ifx_vec_abs_c(const ifx_Vector_C_t* input,
                   ifx_Vector_R_t* output)
{
    IFX_VEC_BRK_DIM(input, output);

    for (uint32_t i = 0; i < vLen(input); ++i)
    {
        vAt(output, i) = ifx_complex_abs(vAt(input, i));
    }
}

//----------------------------------------------------------------------------

void ifx_vec_flip_r(const ifx_Vector_R_t* input,
    ifx_Vector_R_t* output)
{
    IFX_VEC_BRK_DIM(input, output);
    uint32_t start_read = vLen(input) - 1;
    for (uint32_t i = 0; i < vLen(input); ++i)
    {
        vAt(output, i) = vAt(input, start_read--);
    }
}

//----------------------------------------------------------------------------

void ifx_vec_sub_rs(const ifx_Vector_R_t* input,
                    ifx_Float_t scalar_value,
                    ifx_Vector_R_t* output)
{
    IFX_VEC_BRK_VALID(input);
    IFX_VEC_BRK_VALID(output);
    IFX_VEC_BRK_DIM(input, output);

    for (uint32_t i = 0; i < vLen(input); ++i)
    {
        vAt(output, i) = vAt(input, i) - scalar_value;
    }
}

//----------------------------------------------------------------------------

void ifx_vec_sub_cs(const ifx_Vector_C_t* input,
                    ifx_Complex_t scalar_value,
                    ifx_Vector_C_t* output)
{
    IFX_VEC_BRK_VALID(input);
    IFX_VEC_BRK_VALID(output);
    IFX_VEC_BRK_DIM(input, output);

    for (uint32_t i = 0; i < vLen(input); ++i)
    {
        vAt(output, i) = ifx_complex_sub(vAt(input, i), scalar_value);
    }
}

//----------------------------------------------------------------------------

void ifx_vec_scale_r(const ifx_Vector_R_t* input,
                     ifx_Float_t scale,
                     ifx_Vector_R_t* output)
{
    IFX_VEC_BRK_VALID(input);
    IFX_VEC_BRK_VALID(output);
    IFX_VEC_BRK_DIM(input, output);

    for (uint32_t i = 0; i < vLen(input); ++i)
    {
        vAt(output, i) = vAt(input, i) * scale;
    }
}

//----------------------------------------------------------------------------

void ifx_vec_scale_rc(const ifx_Vector_R_t* input,
                      ifx_Complex_t scale,
                      ifx_Vector_C_t* output)
{
    IFX_VEC_BRK_VALID(input);
    IFX_VEC_BRK_VALID(output);
    IFX_VEC_BRK_DIM(input, output);

    for (uint32_t i = 0; i < vLen(input); ++i)
    {
        vAt(output, i) = ifx_complex_mul_real(scale, vAt(input, i));
    }
}

//----------------------------------------------------------------------------

void ifx_vec_scale_c(const ifx_Vector_C_t* input,
                     ifx_Complex_t scale,
                     ifx_Vector_C_t* output)
{
    IFX_VEC_BRK_VALID(input);
    IFX_VEC_BRK_VALID(output);
    IFX_VEC_BRK_DIM(input, output);

    for (uint32_t i = 0; i < vLen(input); ++i)
    {
        vAt(output, i) = ifx_complex_mul(vAt(input, i), scale);
    }
}

//----------------------------------------------------------------------------

void ifx_vec_scale_cr(const ifx_Vector_C_t* input,
                      ifx_Float_t scale,
                      ifx_Vector_C_t* output)
{
    IFX_VEC_BRK_VALID(input);
    IFX_VEC_BRK_VALID(output);
    IFX_VEC_BRK_DIM(input, output);

    for (uint32_t i = 0; i < vLen(input); ++i)
    {
        vAt(output, i) = ifx_complex_mul_real(vAt(input, i), scale);
    }
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_vec_distance_r(const ifx_Vector_R_t* v1,
                               const ifx_Vector_R_t* v2)
{
    IFX_VEC_BRV_VALID(v1, 0.0f);
    IFX_VEC_BRV_VALID(v2, 0.0f);
    IFX_VEC_BRV_DIM(v1, v2, 0.0f);

    ifx_Float_t acc = 0.0f;
    ifx_Float_t result = 0.0f;

    for (uint32_t i = 0; i < vLen(v1); ++i)
    {
        acc += (vAt(v1, i) - vAt(v2, i)) * (vAt(v1, i) - vAt(v2, i));
    }

    result = SQRT(acc);

    return result;
}

//----------------------------------------------------------------------------

void ifx_vec_isort_r(const ifx_Vector_R_t* input,
                     ifx_Vector_Sort_Order_t order,
                     uint32_t* sorted_idxs)
{
    IFX_VEC_BRK_VALID(input);

    ssort(vDat(input), vLen(input), float_compare, order, sorted_idxs);
}

//----------------------------------------------------------------------------

void ifx_vec_mac_r(const ifx_Vector_R_t* v1,
                   const ifx_Vector_R_t* v2,
                   const ifx_Float_t scale,
                   ifx_Vector_R_t* result)
{
    IFX_VEC_BRK_VALID(v1);
    IFX_VEC_BRK_VALID(v2);
    IFX_VEC_BRK_VALID(result);
    IFX_VEC_BRK_DIM(v1, v2);
    IFX_VEC_BRK_DIM(v1, result);

    for (uint32_t i = 0; i < vLen(v1); ++i)
    {
        vAt(result, i) = vAt(v1, i) + (scale * vAt(v2, i));
    }
}

//----------------------------------------------------------------------------

void ifx_vec_mac_c(const ifx_Vector_C_t* v1,
                   const ifx_Vector_C_t* v2,
                   const ifx_Complex_t scale,
                   ifx_Vector_C_t* result)
{
    IFX_VEC_BRK_VALID(v1);
    IFX_VEC_BRK_VALID(v2);
    IFX_VEC_BRK_VALID(result);
    IFX_VEC_BRK_DIM(v1, v2);
    IFX_VEC_BRK_DIM(v1, result);

    for (uint32_t i = 0; i < vLen(v1); ++i)
    {
        vAt(result, i) = ifx_complex_add(vAt(v1, i), ifx_complex_mul(vAt(v2, i), scale));
    }
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_vec_mean_r(const ifx_Vector_R_t* vector)
{
    IFX_VEC_BRV_VALID(vector, 0);
    ifx_Float_t result = 0.0f;

    const float len = (float)(vLen(vector) ? vLen(vector) : 1);
    result = ifx_vec_sum_r(vector) / len;

    return result;
}

//----------------------------------------------------------------------------

ifx_Complex_t ifx_vec_mean_c(const ifx_Vector_C_t* vector)
{
    ifx_Complex_t zero;
    ifx_Complex_t result;
    IFX_COMPLEX_SET(zero, 0, 0);

    IFX_VEC_BRV_VALID(vector, zero);

    const float len = (float)(vLen(vector) ? vLen(vector) : 1);
    result = ifx_complex_div_real(ifx_vec_sum_c(vector), len);

    return result;
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_vec_max_r(const ifx_Vector_R_t* vector)
{
    IFX_VEC_BRV_VALID(vector, 0);

    ifx_Float_t result = vAt(vector, 0);

    for (uint32_t i = 0; i < vLen(vector); ++i)
    {
        const ifx_Float_t val = vAt(vector, i);

        if (val > result)
        {
            result = val;
        }
    }

    return result;
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_vec_var_r(const ifx_Vector_R_t* vector)
{
    IFX_VEC_BRV_VALID(vector, 0);

    const ifx_Float_t mean = ifx_vec_mean_r(vector);
    ifx_Float_t acc = 0;
    ifx_Float_t result;

    for (uint32_t i = 0; i < vLen(vector); ++i)
    {
        acc += (vAt(vector, i) - mean) * (vAt(vector, i) - mean);
    }
    result = acc / vLen(vector);

    return result;
}

//----------------------------------------------------------------------------

uint32_t ifx_vec_local_maxima(const ifx_Vector_R_t* vector, 
                              ifx_Float_t threshold,
                              uint32_t num_maxima,
                              uint32_t* maxima_idxs)
{
    IFX_VEC_BRV_VALID(vector, 0);
    uint32_t count = 0;

    for (uint32_t i = 1; i < vLen(vector)-1; ++i)
    {
        if (vAt(vector, i) > vAt(vector, (size_t)i-1) && vAt(vector, i) >= vAt(vector, (size_t)i+1))
        {
            if (vAt(vector, i) > threshold)
            {
                maxima_idxs[count] = i;
                count++;

                if (count >= num_maxima)
                {
                    break;
                }
            }
        }
    }

    return count;
}

//----------------------------------------------------------------------------

void ifx_vec_clear_r(ifx_Vector_R_t* vector)
{
    IFX_ERR_BRK_NULL(vector);

    if (vStride(vector) > 1)
    {
        for (uint32_t i = 0; i < vLen(vector); i++)
        {
            vAt(vector, i) = 0;
        }
    }
    else
    {
        memset(vDat(vector), 0, sizeof(ifx_Float_t) * vLen(vector));  
    }
}

//----------------------------------------------------------------------------

void ifx_vec_clear_c(ifx_Vector_C_t* vector)
{
    IFX_ERR_BRK_NULL(vector);

    if (vStride(vector) > 1)
    {
        for (uint32_t i = 0; i < vLen(vector); ++i)
        {
            vAt(vector, i) = ifx_complex_zero;
        }
    }
    else
    {
		memset(vDat(vector), 0, sizeof(ifx_Complex_t) * vLen(vector));
    }
}

//----------------------------------------------------------------------------

void ifx_vec_linspace_r(const ifx_Float_t start,
                        const ifx_Float_t end,
                        ifx_Vector_R_t* output)
{
    IFX_ERR_BRK_NULL(output);

    // compute increment
    ifx_Float_t delta = (end - start) / vLen(output);

    // populate linspace values
    vAt(output, 0) = start;
    for (uint32_t i = 1; i < vLen(output); i++) {
        vAt(output, i) = vAt(output, i - 1) + delta;
    }
}

//----------------------------------------------------------------------------

static int median_rank(
        const ifx_Vector_R_t* input, uint32_t offset, 
        uint32_t number_of_el, uint32_t pick_pos_offsetted)
{
    int res = 0;
    ifx_Float_t val = vAt(input, pick_pos_offsetted+offset);
    for(uint32_t i = 0; i < pick_pos_offsetted; i++)
    {
        if(val >= vAt(input,i+offset)) //There is difference if you are before or after index (==)
        { 
            res++;
        } else {
            res--;
        }
    }
    for(uint32_t i = pick_pos_offsetted + 1; i < number_of_el; i++)
    {
        if(val > vAt(input,i+offset)) //There is difference if you are before or after index (==)
        {   
            res++;
        } else {
            res--;
        }
    }
    return res;
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_vect_median_range_r(const ifx_Vector_R_t* input, uint32_t offset, uint32_t length)
{
    IFX_ERR_BRV_NULL(input, IFX_NAN);
    IFX_ERR_BRV_ARGUMENT(vLen(input) < length + offset, IFX_NAN); 
    
    ifx_Float_t upper_limit = IFX_INF_POS;
    ifx_Float_t lower_limit = IFX_INF_NEG;
    
    const int32_t undef_idx = -1;   
    int32_t prev_idx = undef_idx;
    
    for(int32_t idx = 0; idx < (int32_t)length; idx++)
    {
        ifx_Float_t val = vAt(input, idx + offset);
        if(val>upper_limit || val<lower_limit)
        {
            continue;
        }
        int rank = median_rank(input, offset, length, idx);
        if(rank == 0)
        {
            return val;
        } 
        else if(rank == 1 || rank == -1) //Only happens for length n%2==0
        {
            if(prev_idx != undef_idx)
            {
                ifx_Float_t val_prev = vAt(input, prev_idx + offset);        
                return (val+val_prev)/2;
            }
            prev_idx = idx;
        }

        if(rank >= 0)
        {
            upper_limit = val;
        } else {
            lower_limit = val;
        }
    }
    return IFX_NAN;
}

//----------------------------------------------------------------------------

ifx_Float_t ifx_vect_median_r(const ifx_Vector_R_t* input){
    return ifx_vect_median_range_r(input, 0, vLen(input));
}

//----------------------------------------------------------------------------

bool ifx_vec_is_zero_r(ifx_Vector_R_t* vector)
{
    IFX_ERR_BRV_NULL(vector, false);

    for (uint32_t i = 1; i < vLen(vector); i++) {
        if (vAt(vector, i) != 0)
            return false;
    }

    return true;
}