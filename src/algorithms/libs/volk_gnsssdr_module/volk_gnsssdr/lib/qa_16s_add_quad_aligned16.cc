/* Copyright (C) 2010-2015 (see AUTHORS file for a list of contributors)
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
 */

#include <volk_gnsssdr/volk_gnsssdr.h>
#include <qa_16s_add_quad_aligned16.h>
#include <volk_gnsssdr/volk_gnsssdr_16s_add_quad_aligned16.h>
#include <cstdlib>
#include <ctime>
//test for sse2

#ifndef LV_HAVE_SSE2

void qa_16s_add_quad_aligned16::t1()
{
    printf("sse2 not available... no test performed\n");
}

#else

void qa_16s_add_quad_aligned16::t1()
{
    volk_gnsssdr_environment_init();
    clock_t start, end;
    double total;
    const int vlen = 3200;
    const int ITERS = 100000;
    __VOLK_ATTR_ALIGNED(16) short input0[vlen];
    __VOLK_ATTR_ALIGNED(16) short input1[vlen];
    __VOLK_ATTR_ALIGNED(16) short input2[vlen];
    __VOLK_ATTR_ALIGNED(16) short input3[vlen];
    __VOLK_ATTR_ALIGNED(16) short input4[vlen];

    __VOLK_ATTR_ALIGNED(16) short output0[vlen];
    __VOLK_ATTR_ALIGNED(16) short output1[vlen];
    __VOLK_ATTR_ALIGNED(16) short output2[vlen];
    __VOLK_ATTR_ALIGNED(16) short output3[vlen];
    __VOLK_ATTR_ALIGNED(16) short output01[vlen];
    __VOLK_ATTR_ALIGNED(16) short output11[vlen];
    __VOLK_ATTR_ALIGNED(16) short output21[vlen];
    __VOLK_ATTR_ALIGNED(16) short output31[vlen];

    for(int i = 0; i < vlen; ++i)
        {
            short plus0 = ((short) (rand() - (RAND_MAX/2))) >> 2;
            short minus0 = ((short) (rand() - (RAND_MAX/2))) >> 2;
            short plus1 = ((short) (rand() - (RAND_MAX/2))) >> 2;
            short minus1 = ((short) (rand() - (RAND_MAX/2))) >> 2;
            short plus2 = ((short) (rand() - (RAND_MAX/2))) >> 2;
            short minus2 = ((short) (rand() - (RAND_MAX/2))) >> 2;
            short plus3 = ((short) (rand() - (RAND_MAX/2))) >> 2;
            short minus3 = ((short) (rand() - (RAND_MAX/2))) >> 2;
            short plus4 = ((short) (rand() - (RAND_MAX/2))) >> 2;
            short minus4 = ((short) (rand() - (RAND_MAX/2))) >> 2;

            input0[i] = plus0 - minus0;
            input1[i] = plus1 - minus1;
            input2[i] = plus2 - minus2;
            input3[i] = plus3 - minus3;
            input4[i] = plus4 - minus4;

        }
    printf("16s_add_quad_aligned\n");

    start = clock();
    for(int count = 0; count < ITERS; ++count)
        {
            volk_gnsssdr_16s_add_quad_aligned16_manual(output0, output1, output2, output3, input0, input1, input2, input3, input4, vlen << 1 , "generic");
        }
    end = clock();
    total = (double)(end-start)/(double)CLOCKS_PER_SEC;
    printf("generic_time: %f\n", total);
    start = clock();
    for(int count = 0; count < ITERS; ++count)
        {
            volk_gnsssdr_16s_add_quad_aligned16_manual(output01, output11, output21, output31, input0, input1, input2, input3, input4, vlen << 1 , "sse2");
        }
    end = clock();
    total = (double)(end-start)/(double)CLOCKS_PER_SEC;
    printf("sse2_time: %f\n", total);
    for(int i = 0; i < 1; ++i)
        {
            //printf("inputs: %d, %d\n", input0[i*2], input0[i*2 + 1]);
            //printf("generic... %d, ssse3... %d\n", output0[i], output1[i]);
        }

    for(int i = 0; i < vlen; ++i)
        {
            //printf("%d...%d\n", output0[i], output01[i]);
            CPPUNIT_ASSERT_EQUAL(output0[i], output01[i]);
            CPPUNIT_ASSERT_EQUAL(output1[i], output11[i]);
            CPPUNIT_ASSERT_EQUAL(output2[i], output21[i]);
            CPPUNIT_ASSERT_EQUAL(output3[i], output31[i]);
        }
}

#endif
