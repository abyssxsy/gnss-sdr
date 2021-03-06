/*!
 * \file volk_gnsssdr_8ic_x5_cw_epl_corr_32fc_x3.h
 * \brief Volk protokernel: performs the carrier wipe-off mixing and the Early, Prompt, and Late correlation with 16 bits vectors, and accumulates the results into float32.
 * \authors <ul>
 *          <li> Andres Cecilia, 2014. a.cecilia.luque(at)gmail.com
 *          </ul>
 *
 * Volk protokernel that performs the carrier wipe-off mixing and the
 * Early, Prompt, and Late correlation with 16 bits vectors (8 bits the
 * real part and 8 bits the imaginary part), and accumulates the result 
 * in 32 bits single point values, returning float32 values:
 * - The carrier wipe-off is done by multiplying the input signal by the
 * carrier (multiplication of 16 bits vectors) It returns the input
 * signal in base band (BB)
 * - Early values are calculated by multiplying the input signal in BB by the
 * early code (multiplication of 16 bits vectors), accumulating the results into float32 values
 * - Prompt values are calculated by multiplying the input signal in BB by the
 * prompt code (multiplication of 16 bits vectors), accumulating the results into float32 values
 * - Late values are calculated by multiplying the input signal in BB by the
 * late code (multiplication of 16 bits vectors), accumulating the results into float32 values
 *
 * -------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2015  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *          Satellite Systems receiver
 *
 * This file is part of GNSS-SDR.
 *
 * GNSS-SDR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNSS-SDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNSS-SDR. If not, see <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------
 */

#ifndef INCLUDED_gnsssdr_volk_gnsssdr_8ic_x5_cw_epl_corr_32fc_x3_u_H
#define INCLUDED_gnsssdr_volk_gnsssdr_8ic_x5_cw_epl_corr_32fc_x3_u_H

#include <inttypes.h>
#include <stdio.h>
#include <volk_gnsssdr/volk_gnsssdr_complex.h>
#include <float.h>
#include <string.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
#include "CommonMacros/CommonMacros_8ic_cw_epl_corr_32fc.h"
#include "CommonMacros/CommonMacros.h"

/*!
 \brief Performs the carrier wipe-off mixing and the Early, Prompt, and Late correlation
 \param input The input signal input
 \param carrier The carrier signal input
 \param E_code Early PRN code replica input
 \param P_code Early PRN code replica input
 \param L_code Early PRN code replica input
 \param E_out Early correlation output
 \param P_out Early correlation output
 \param L_out Early correlation output
 \param num_points The number of complex values in vectors
 */
static inline void volk_gnsssdr_8ic_x5_cw_epl_corr_32fc_x3_u_sse4_1(lv_32fc_t* E_out, lv_32fc_t* P_out, lv_32fc_t* L_out, const lv_8sc_t* input, const lv_8sc_t* carrier, const lv_8sc_t* E_code, const lv_8sc_t* P_code, const lv_8sc_t* L_code, unsigned int num_points)
{
    const unsigned int sse_iters = num_points / 8;

    __m128i x, y, real_bb_signal_sample, imag_bb_signal_sample;
    __m128i mult1, realx, imagx, realy, imagy, realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, output, real_output, imag_output;

    __m128 E_code_acc, P_code_acc, L_code_acc;
    __m128i input_i_1, input_i_2, output_i32, output_i32_1, output_i32_2;
    __m128 output_ps;

    const lv_8sc_t* input_ptr = input;
    const lv_8sc_t* carrier_ptr = carrier;

    const lv_8sc_t* E_code_ptr = E_code;
    lv_32fc_t* E_out_ptr = E_out;
    const lv_8sc_t* L_code_ptr = L_code;
    lv_32fc_t* L_out_ptr = L_out;
    const lv_8sc_t* P_code_ptr = P_code;
    lv_32fc_t* P_out_ptr = P_out;

    *E_out_ptr = 0;
    *P_out_ptr = 0;
    *L_out_ptr = 0;

    mult1 = _mm_set_epi8(0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255);

    E_code_acc = _mm_setzero_ps();
    L_code_acc = _mm_setzero_ps();
    P_code_acc = _mm_setzero_ps();

    if (sse_iters>0)
        {
            for(int number = 0;number < sse_iters; number++)
                {
                    //Perform the carrier wipe-off
                    x = _mm_lddqu_si128((__m128i*)input_ptr);
                    y = _mm_lddqu_si128((__m128i*)carrier_ptr);

                    CM_8IC_REARRANGE_VECTOR_INTO_REAL_IMAG_16IC_X2_U_SSE2(x, mult1, realx, imagx)
                    CM_8IC_REARRANGE_VECTOR_INTO_REAL_IMAG_16IC_X2_U_SSE2(y, mult1, realy, imagy)

                    CM_16IC_X4_SCALAR_PRODUCT_16IC_X2_U_SSE2(realx, imagx, realy, imagy, realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_bb_signal_sample, imag_bb_signal_sample)

                    //Get early values
                    y = _mm_lddqu_si128((__m128i*)E_code_ptr);

                    CM_8IC_X2_CW_CORR_32FC_X2_U_SSE4_1(y, mult1, realy, imagy, real_bb_signal_sample, imag_bb_signal_sample,realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_output, imag_output, input_i_1, input_i_2, output_i32, output_i32_1, output_i32_2, output_ps)

                    E_code_acc = _mm_add_ps (E_code_acc, output_ps);

                    //Get prompt values
                    y = _mm_lddqu_si128((__m128i*)P_code_ptr);

                    CM_8IC_X2_CW_CORR_32FC_X2_U_SSE4_1(y, mult1, realy, imagy, real_bb_signal_sample, imag_bb_signal_sample,realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_output, imag_output, input_i_1, input_i_2, output_i32, output_i32_1, output_i32_2, output_ps)

                    P_code_acc = _mm_add_ps (P_code_acc, output_ps);

                    //Get late values
                    y = _mm_lddqu_si128((__m128i*)L_code_ptr);

                    CM_8IC_X2_CW_CORR_32FC_X2_U_SSE4_1(y, mult1, realy, imagy, real_bb_signal_sample, imag_bb_signal_sample,realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_output, imag_output, input_i_1, input_i_2, output_i32, output_i32_1, output_i32_2, output_ps)

                    L_code_acc = _mm_add_ps (L_code_acc, output_ps);

                    input_ptr += 8;
                    carrier_ptr += 8;
                    E_code_ptr += 8;
                    P_code_ptr += 8;
                    L_code_ptr += 8;
                }

            __VOLK_ATTR_ALIGNED(16) lv_32fc_t E_dotProductVector[2];
            __VOLK_ATTR_ALIGNED(16) lv_32fc_t P_dotProductVector[2];
            __VOLK_ATTR_ALIGNED(16) lv_32fc_t L_dotProductVector[2];

            _mm_storeu_ps((float*)E_dotProductVector,E_code_acc); // Store the results back into the dot product vector
            _mm_storeu_ps((float*)P_dotProductVector,P_code_acc); // Store the results back into the dot product vector
            _mm_storeu_ps((float*)L_dotProductVector,L_code_acc); // Store the results back into the dot product vector

            for (int i = 0; i<2; ++i)
                {
                    *E_out_ptr += E_dotProductVector[i];
                    *P_out_ptr += P_dotProductVector[i];
                    *L_out_ptr += L_dotProductVector[i];
                }
        }

    lv_8sc_t bb_signal_sample;
    for(int i=0; i < num_points%8; ++i)
        {
            //Perform the carrier wipe-off
            bb_signal_sample = (*input_ptr++) * (*carrier_ptr++);
            // Now get early, late, and prompt values for each
            *E_out_ptr += (lv_32fc_t) (bb_signal_sample * (*E_code_ptr++));
            *P_out_ptr += (lv_32fc_t) (bb_signal_sample * (*P_code_ptr++));
            *L_out_ptr += (lv_32fc_t) (bb_signal_sample * (*L_code_ptr++));
        }
}
#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
#include "CommonMacros/CommonMacros_8ic_cw_epl_corr_32fc.h"
#include "CommonMacros/CommonMacros.h"
/*!
 \brief Performs the carrier wipe-off mixing and the Early, Prompt, and Late correlation
 \param input The input signal input
 \param carrier The carrier signal input
 \param E_code Early PRN code replica input
 \param P_code Early PRN code replica input
 \param L_code Early PRN code replica input
 \param E_out Early correlation output
 \param P_out Early correlation output
 \param L_out Early correlation output
 \param num_points The number of complex values in vectors
 */
static inline void volk_gnsssdr_8ic_x5_cw_epl_corr_32fc_x3_u_sse2(lv_32fc_t* E_out, lv_32fc_t* P_out, lv_32fc_t* L_out, const lv_8sc_t* input, const lv_8sc_t* carrier, const lv_8sc_t* E_code, const lv_8sc_t* P_code, const lv_8sc_t* L_code, unsigned int num_points)
{
    const unsigned int sse_iters = num_points / 8;

    __m128i x, y, real_bb_signal_sample, imag_bb_signal_sample;
    __m128i mult1, realx, imagx, realy, imagy, realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, output, real_output, imag_output;

    __m128 E_code_acc, P_code_acc, L_code_acc;
    __m128i input_i_1, input_i_2, output_i32;
    __m128 output_ps_1, output_ps_2;

    const lv_8sc_t* input_ptr = input;
    const lv_8sc_t* carrier_ptr = carrier;

    const lv_8sc_t* E_code_ptr = E_code;
    lv_32fc_t* E_out_ptr = E_out;
    const lv_8sc_t* L_code_ptr = L_code;
    lv_32fc_t* L_out_ptr = L_out;
    const lv_8sc_t* P_code_ptr = P_code;
    lv_32fc_t* P_out_ptr = P_out;

    *E_out_ptr = 0;
    *P_out_ptr = 0;
    *L_out_ptr = 0;

    mult1 = _mm_set_epi8(0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255);

    E_code_acc = _mm_setzero_ps();
    L_code_acc = _mm_setzero_ps();
    P_code_acc = _mm_setzero_ps();

    if (sse_iters>0)
        {
            for(unsigned int number = 0;number < sse_iters; number++)
                {
                    //Perform the carrier wipe-off
                    x = _mm_loadu_si128((__m128i*)input_ptr);
                    y = _mm_loadu_si128((__m128i*)carrier_ptr);

                    CM_8IC_REARRANGE_VECTOR_INTO_REAL_IMAG_16IC_X2_U_SSE2(x, mult1, realx, imagx)
                    CM_8IC_REARRANGE_VECTOR_INTO_REAL_IMAG_16IC_X2_U_SSE2(y, mult1, realy, imagy)

                    CM_16IC_X4_SCALAR_PRODUCT_16IC_X2_U_SSE2(realx, imagx, realy, imagy, realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_bb_signal_sample, imag_bb_signal_sample)

                    //Get early values
                    y = _mm_loadu_si128((__m128i*)E_code_ptr);

                    CM_8IC_X2_CW_CORR_32FC_X2_U_SSE2(y, mult1, realy, imagy, real_bb_signal_sample, imag_bb_signal_sample,realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_output, imag_output, input_i_1, input_i_2, output_i32, output_ps_1, output_ps_2)

                    E_code_acc = _mm_add_ps (E_code_acc, output_ps_1);
                    E_code_acc = _mm_add_ps (E_code_acc, output_ps_2);

                    //Get prompt values
                    y = _mm_loadu_si128((__m128i*)P_code_ptr);

                    CM_8IC_X2_CW_CORR_32FC_X2_U_SSE2(y, mult1, realy, imagy, real_bb_signal_sample, imag_bb_signal_sample,realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_output, imag_output, input_i_1, input_i_2, output_i32, output_ps_1, output_ps_2)

                    P_code_acc = _mm_add_ps (P_code_acc, output_ps_1);
                    P_code_acc = _mm_add_ps (P_code_acc, output_ps_2);

                    //Get late values
                    y = _mm_loadu_si128((__m128i*)L_code_ptr);

                    CM_8IC_X2_CW_CORR_32FC_X2_U_SSE2(y, mult1, realy, imagy, real_bb_signal_sample, imag_bb_signal_sample,realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_output, imag_output, input_i_1, input_i_2, output_i32, output_ps_1, output_ps_2)

                    L_code_acc = _mm_add_ps (L_code_acc, output_ps_1);
                    L_code_acc = _mm_add_ps (L_code_acc, output_ps_2);

                    input_ptr += 8;
                    carrier_ptr += 8;
                    E_code_ptr += 8;
                    P_code_ptr += 8;
                    L_code_ptr += 8;
                }

            __VOLK_ATTR_ALIGNED(16) lv_32fc_t E_dotProductVector[2];
            __VOLK_ATTR_ALIGNED(16) lv_32fc_t P_dotProductVector[2];
            __VOLK_ATTR_ALIGNED(16) lv_32fc_t L_dotProductVector[2];

            _mm_storeu_ps((float*)E_dotProductVector,E_code_acc); // Store the results back into the dot product vector
            _mm_storeu_ps((float*)P_dotProductVector,P_code_acc); // Store the results back into the dot product vector
            _mm_storeu_ps((float*)L_dotProductVector,L_code_acc); // Store the results back into the dot product vector

            for (unsigned int i = 0; i<2; ++i)
                {
                    *E_out_ptr += E_dotProductVector[i];
                    *P_out_ptr += P_dotProductVector[i];
                    *L_out_ptr += L_dotProductVector[i];
                }
        }

    lv_8sc_t bb_signal_sample;
    for(unsigned int i=0; i < num_points%8; ++i)
        {
            //Perform the carrier wipe-off
            bb_signal_sample = (*input_ptr++) * (*carrier_ptr++);
            // Now get early, late, and prompt values for each
            *E_out_ptr += (lv_32fc_t) (bb_signal_sample * (*E_code_ptr++));
            *P_out_ptr += (lv_32fc_t) (bb_signal_sample * (*P_code_ptr++));
            *L_out_ptr += (lv_32fc_t) (bb_signal_sample * (*L_code_ptr++));
        }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_GENERIC
/*!
 \brief Performs the carrier wipe-off mixing and the Early, Prompt, and Late correlation
 \param input The input signal input
 \param carrier The carrier signal input
 \param E_code Early PRN code replica input
 \param P_code Early PRN code replica input
 \param L_code Early PRN code replica input
 \param E_out Early correlation output
 \param P_out Early correlation output
 \param L_out Early correlation output
 \param num_points The number of complex values in vectors
 */
static inline void volk_gnsssdr_8ic_x5_cw_epl_corr_32fc_x3_generic(lv_32fc_t* E_out, lv_32fc_t* P_out, lv_32fc_t* L_out, const lv_8sc_t* input, const lv_8sc_t* carrier, const lv_8sc_t* E_code, const lv_8sc_t* P_code, const lv_8sc_t* L_code, unsigned int num_points)
{
    lv_8sc_t bb_signal_sample;

    bb_signal_sample = lv_cmake(0, 0);

    *E_out = 0;
    *P_out = 0;
    *L_out = 0;
    // perform Early, Prompt and Late correlation
    for(unsigned int i=0; i < num_points; ++i)
        {
            //Perform the carrier wipe-off
            bb_signal_sample = input[i] * carrier[i];
            // Now get early, late, and prompt values for each
            *E_out += (lv_32fc_t) (bb_signal_sample * E_code[i]);
            *P_out += (lv_32fc_t) (bb_signal_sample * P_code[i]);
            *L_out += (lv_32fc_t) (bb_signal_sample * L_code[i]);
        }
}

#endif /* LV_HAVE_GENERIC */

#endif /* INCLUDED_gnsssdr_volk_gnsssdr_8ic_x5_cw_epl_corr_32fc_x3_u_H */


#ifndef INCLUDED_gnsssdr_volk_gnsssdr_8ic_x5_cw_epl_corr_32fc_x3_a_H
#define INCLUDED_gnsssdr_volk_gnsssdr_8ic_x5_cw_epl_corr_32fc_x3_a_H

#include <inttypes.h>
#include <stdio.h>
#include <volk_gnsssdr/volk_gnsssdr_complex.h>
#include <float.h>
#include <string.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
#include "CommonMacros/CommonMacros_8ic_cw_epl_corr_32fc.h"
#include "CommonMacros/CommonMacros.h"
/*!
 \brief Performs the carrier wipe-off mixing and the Early, Prompt, and Late correlation
 \param input The input signal input
 \param carrier The carrier signal input
 \param E_code Early PRN code replica input
 \param P_code Early PRN code replica input
 \param L_code Early PRN code replica input
 \param E_out Early correlation output
 \param P_out Early correlation output
 \param L_out Early correlation output
 \param num_points The number of complex values in vectors
 */
static inline void volk_gnsssdr_8ic_x5_cw_epl_corr_32fc_x3_a_sse4_1(lv_32fc_t* E_out, lv_32fc_t* P_out, lv_32fc_t* L_out, const lv_8sc_t* input, const lv_8sc_t* carrier, const lv_8sc_t* E_code, const lv_8sc_t* P_code, const lv_8sc_t* L_code, unsigned int num_points)
{
    const unsigned int sse_iters = num_points / 8;

    __m128i x, y, real_bb_signal_sample, imag_bb_signal_sample;
    __m128i mult1, realx, imagx, realy, imagy, realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, output, real_output, imag_output;

    __m128 E_code_acc, P_code_acc, L_code_acc;
    __m128i input_i_1, input_i_2, output_i32, output_i32_1, output_i32_2;
    __m128 output_ps;

    const lv_8sc_t* input_ptr = input;
    const lv_8sc_t* carrier_ptr = carrier;

    const lv_8sc_t* E_code_ptr = E_code;
    lv_32fc_t* E_out_ptr = E_out;
    const lv_8sc_t* L_code_ptr = L_code;
    lv_32fc_t* L_out_ptr = L_out;
    const lv_8sc_t* P_code_ptr = P_code;
    lv_32fc_t* P_out_ptr = P_out;

    *E_out_ptr = 0;
    *P_out_ptr = 0;
    *L_out_ptr = 0;

    mult1 = _mm_set_epi8(0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255);

    E_code_acc = _mm_setzero_ps();
    L_code_acc = _mm_setzero_ps();
    P_code_acc = _mm_setzero_ps();

    if (sse_iters>0)
        {
            for(int number = 0;number < sse_iters; number++)
                {
                    //Perform the carrier wipe-off
                    x = _mm_load_si128((__m128i*)input_ptr);
                    y = _mm_load_si128((__m128i*)carrier_ptr);

                    CM_8IC_REARRANGE_VECTOR_INTO_REAL_IMAG_16IC_X2_U_SSE2(x, mult1, realx, imagx)
                    CM_8IC_REARRANGE_VECTOR_INTO_REAL_IMAG_16IC_X2_U_SSE2(y, mult1, realy, imagy)

                    CM_16IC_X4_SCALAR_PRODUCT_16IC_X2_U_SSE2(realx, imagx, realy, imagy, realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_bb_signal_sample, imag_bb_signal_sample)

                    //Get early values
                    y = _mm_load_si128((__m128i*)E_code_ptr);

                    CM_8IC_X2_CW_CORR_32FC_X2_U_SSE4_1(y, mult1, realy, imagy, real_bb_signal_sample, imag_bb_signal_sample,realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_output, imag_output, input_i_1, input_i_2, output_i32, output_i32_1, output_i32_2, output_ps)

                    E_code_acc = _mm_add_ps (E_code_acc, output_ps);

                    //Get prompt values
                    y = _mm_load_si128((__m128i*)P_code_ptr);

                    CM_8IC_X2_CW_CORR_32FC_X2_U_SSE4_1(y, mult1, realy, imagy, real_bb_signal_sample, imag_bb_signal_sample,realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_output, imag_output, input_i_1, input_i_2, output_i32, output_i32_1, output_i32_2, output_ps)

                    P_code_acc = _mm_add_ps (P_code_acc, output_ps);

                    //Get late values
                    y = _mm_load_si128((__m128i*)L_code_ptr);

                    CM_8IC_X2_CW_CORR_32FC_X2_U_SSE4_1(y, mult1, realy, imagy, real_bb_signal_sample, imag_bb_signal_sample,realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_output, imag_output, input_i_1, input_i_2, output_i32, output_i32_1, output_i32_2, output_ps)

                    L_code_acc = _mm_add_ps (L_code_acc, output_ps);

                    input_ptr += 8;
                    carrier_ptr += 8;
                    E_code_ptr += 8;
                    P_code_ptr += 8;
                    L_code_ptr += 8;
                }

            __VOLK_ATTR_ALIGNED(16) lv_32fc_t E_dotProductVector[2];
            __VOLK_ATTR_ALIGNED(16) lv_32fc_t P_dotProductVector[2];
            __VOLK_ATTR_ALIGNED(16) lv_32fc_t L_dotProductVector[2];

            _mm_store_ps((float*)E_dotProductVector,E_code_acc); // Store the results back into the dot product vector
            _mm_store_ps((float*)P_dotProductVector,P_code_acc); // Store the results back into the dot product vector
            _mm_store_ps((float*)L_dotProductVector,L_code_acc); // Store the results back into the dot product vector

            for (int i = 0; i<2; ++i)
                {
                    *E_out_ptr += E_dotProductVector[i];
                    *P_out_ptr += P_dotProductVector[i];
                    *L_out_ptr += L_dotProductVector[i];
                }
        }

    lv_8sc_t bb_signal_sample;
    for(int i=0; i < num_points%8; ++i)
        {
            //Perform the carrier wipe-off
            bb_signal_sample = (*input_ptr++) * (*carrier_ptr++);
            // Now get early, late, and prompt values for each
            *E_out_ptr += (lv_32fc_t) (bb_signal_sample * (*E_code_ptr++));
            *P_out_ptr += (lv_32fc_t) (bb_signal_sample * (*P_code_ptr++));
            *L_out_ptr += (lv_32fc_t) (bb_signal_sample * (*L_code_ptr++));
        }
}
#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
#include "CommonMacros/CommonMacros_8ic_cw_epl_corr_32fc.h"
#include "CommonMacros/CommonMacros.h"
/*!
 \brief Performs the carrier wipe-off mixing and the Early, Prompt, and Late correlation
 \param input The input signal input
 \param carrier The carrier signal input
 \param E_code Early PRN code replica input
 \param P_code Early PRN code replica input
 \param L_code Early PRN code replica input
 \param E_out Early correlation output
 \param P_out Early correlation output
 \param L_out Early correlation output
 \param num_points The number of complex values in vectors
 */
static inline void volk_gnsssdr_8ic_x5_cw_epl_corr_32fc_x3_a_sse2(lv_32fc_t* E_out, lv_32fc_t* P_out, lv_32fc_t* L_out, const lv_8sc_t* input, const lv_8sc_t* carrier, const lv_8sc_t* E_code, const lv_8sc_t* P_code, const lv_8sc_t* L_code, unsigned int num_points)
{
    const unsigned int sse_iters = num_points / 8;

    __m128i x, y, real_bb_signal_sample, imag_bb_signal_sample;
    __m128i mult1, realx, imagx, realy, imagy, realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, output, real_output, imag_output;

    __m128 E_code_acc, P_code_acc, L_code_acc;
    __m128i input_i_1, input_i_2, output_i32;
    __m128 output_ps_1, output_ps_2;

    const lv_8sc_t* input_ptr = input;
    const lv_8sc_t* carrier_ptr = carrier;

    const lv_8sc_t* E_code_ptr = E_code;
    lv_32fc_t* E_out_ptr = E_out;
    const lv_8sc_t* L_code_ptr = L_code;
    lv_32fc_t* L_out_ptr = L_out;
    const lv_8sc_t* P_code_ptr = P_code;
    lv_32fc_t* P_out_ptr = P_out;

    *E_out_ptr = 0;
    *P_out_ptr = 0;
    *L_out_ptr = 0;

    mult1 = _mm_set_epi8(0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255);

    E_code_acc = _mm_setzero_ps();
    L_code_acc = _mm_setzero_ps();
    P_code_acc = _mm_setzero_ps();

    if (sse_iters>0)
        {
            for(unsigned int number = 0;number < sse_iters; number++)
                {
                    //Perform the carrier wipe-off
                    x = _mm_load_si128((__m128i*)input_ptr);
                    y = _mm_load_si128((__m128i*)carrier_ptr);

                    CM_8IC_REARRANGE_VECTOR_INTO_REAL_IMAG_16IC_X2_U_SSE2(x, mult1, realx, imagx)
                    CM_8IC_REARRANGE_VECTOR_INTO_REAL_IMAG_16IC_X2_U_SSE2(y, mult1, realy, imagy)

                    CM_16IC_X4_SCALAR_PRODUCT_16IC_X2_U_SSE2(realx, imagx, realy, imagy, realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_bb_signal_sample, imag_bb_signal_sample)

                    //Get early values
                    y = _mm_load_si128((__m128i*)E_code_ptr);

                    CM_8IC_X2_CW_CORR_32FC_X2_U_SSE2(y, mult1, realy, imagy, real_bb_signal_sample, imag_bb_signal_sample,realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_output, imag_output, input_i_1, input_i_2, output_i32, output_ps_1, output_ps_2)

                    E_code_acc = _mm_add_ps (E_code_acc, output_ps_1);
                    E_code_acc = _mm_add_ps (E_code_acc, output_ps_2);

                    //Get prompt values
                    y = _mm_load_si128((__m128i*)P_code_ptr);

                    CM_8IC_X2_CW_CORR_32FC_X2_U_SSE2(y, mult1, realy, imagy, real_bb_signal_sample, imag_bb_signal_sample,realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_output, imag_output, input_i_1, input_i_2, output_i32, output_ps_1, output_ps_2)

                    P_code_acc = _mm_add_ps (P_code_acc, output_ps_1);
                    P_code_acc = _mm_add_ps (P_code_acc, output_ps_2);

                    //Get late values
                    y = _mm_load_si128((__m128i*)L_code_ptr);

                    CM_8IC_X2_CW_CORR_32FC_X2_U_SSE2(y, mult1, realy, imagy, real_bb_signal_sample, imag_bb_signal_sample,realx_mult_realy, imagx_mult_imagy, realx_mult_imagy, imagx_mult_realy, real_output, imag_output, input_i_1, input_i_2, output_i32, output_ps_1, output_ps_2)

                    L_code_acc = _mm_add_ps (L_code_acc, output_ps_1);
                    L_code_acc = _mm_add_ps (L_code_acc, output_ps_2);

                    input_ptr += 8;
                    carrier_ptr += 8;
                    E_code_ptr += 8;
                    P_code_ptr += 8;
                    L_code_ptr += 8;
                }

            __VOLK_ATTR_ALIGNED(16) lv_32fc_t E_dotProductVector[2];
            __VOLK_ATTR_ALIGNED(16) lv_32fc_t P_dotProductVector[2];
            __VOLK_ATTR_ALIGNED(16) lv_32fc_t L_dotProductVector[2];

            _mm_store_ps((float*)E_dotProductVector,E_code_acc); // Store the results back into the dot product vector
            _mm_store_ps((float*)P_dotProductVector,P_code_acc); // Store the results back into the dot product vector
            _mm_store_ps((float*)L_dotProductVector,L_code_acc); // Store the results back into the dot product vector

            for (unsigned int i = 0; i<2; ++i)
                {
                    *E_out_ptr += E_dotProductVector[i];
                    *P_out_ptr += P_dotProductVector[i];
                    *L_out_ptr += L_dotProductVector[i];
                }
        }

    lv_8sc_t bb_signal_sample;
    for(unsigned int i=0; i < num_points%8; ++i)
        {
            //Perform the carrier wipe-off
            bb_signal_sample = (*input_ptr++) * (*carrier_ptr++);
            // Now get early, late, and prompt values for each
            *E_out_ptr += (lv_32fc_t) (bb_signal_sample * (*E_code_ptr++));
            *P_out_ptr += (lv_32fc_t) (bb_signal_sample * (*P_code_ptr++));
            *L_out_ptr += (lv_32fc_t) (bb_signal_sample * (*L_code_ptr++));
        }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_GENERIC
/*!
 \brief Performs the carrier wipe-off mixing and the Early, Prompt, and Late correlation
 \param input The input signal input
 \param carrier The carrier signal input
 \param E_code Early PRN code replica input
 \param P_code Early PRN code replica input
 \param L_code Early PRN code replica input
 \param E_out Early correlation output
 \param P_out Early correlation output
 \param L_out Early correlation output
 \param num_points The number of complex values in vectors
 */
static inline void volk_gnsssdr_8ic_x5_cw_epl_corr_32fc_x3_a_generic(lv_32fc_t* E_out, lv_32fc_t* P_out, lv_32fc_t* L_out, const lv_8sc_t* input, const lv_8sc_t* carrier, const lv_8sc_t* E_code, const lv_8sc_t* P_code, const lv_8sc_t* L_code, unsigned int num_points)
{
    lv_8sc_t bb_signal_sample;

    bb_signal_sample = lv_cmake(0, 0);

    *E_out = 0;
    *P_out = 0;
    *L_out = 0;
    // perform Early, Prompt and Late correlation
    for(unsigned int i=0; i < num_points; ++i)
        {
            //Perform the carrier wipe-off
            bb_signal_sample = input[i] * carrier[i];
            // Now get early, late, and prompt values for each
            *E_out += (lv_32fc_t) (bb_signal_sample * E_code[i]);
            *P_out += (lv_32fc_t) (bb_signal_sample * P_code[i]);
            *L_out += (lv_32fc_t) (bb_signal_sample * L_code[i]);
        }
}

#endif /* LV_HAVE_GENERIC */
#endif /* INCLUDED_gnsssdr_volk_gnsssdr_8ic_x5_cw_epl_corr_32fc_x3_a_H */
