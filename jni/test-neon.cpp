/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//#include <cerrno>
//#include <cstddef>
#include <stdio.h>
#include <sys/time.h> 

#include <arm_neon.h>

#define   __NEON__
#define   __NEON_ASM__

#define N   (64*3)
uint8_t dest[N], src[N];

void reference_convert (uint8_t * __restrict dest, uint8_t * __restrict src, int n)    
{    
  int i;    

  for (i=0; i<n; i++)    
  {    
    int r = *src++; // load red    
    int g = *src++; // load green    
    int b = *src++; // load blue     
   
    // build weighted average:    
    int y = (r*77)+(g*151)+(b*28);    
   
    // undo the scale by 256 and write to memory:    
    *dest++ = (y>>8);    
  }    
}   

#ifdef __NEON__
void neon_convert (uint8_t * __restrict dest, uint8_t * __restrict src, int n)    
{    
  int i;    
  uint8x8_t rfac = vdup_n_u8 (77);       // 转换权值  R    
  uint8x8_t gfac = vdup_n_u8 (151);    // 转换权值  G    
  uint8x8_t bfac = vdup_n_u8 (28);      // 转换权值  B    
  n/=8;    
   
  for (i=0; i<n; i++)    
  {    
    uint16x8_t  temp;    
    uint8x8x3_t rgb  = vld3_u8 (src);    
    uint8x8_t result;    
   
    temp = vmull_u8 (rgb.val[0],      rfac);       // vmull_u8 每个字节（8bit）对应相乘，结果为每个单位2字节（16bit）    
    temp = vmlal_u8 (temp,rgb.val[1], gfac);  // 每个比特对应相乘并加上    
    temp = vmlal_u8 (temp,rgb.val[2], bfac);    
   
    result = vshrn_n_u16 (temp, 8);  // 全部移位8位    
    vst1_u8 (dest, result);   // 转存运算结果    
    src  += 8*3;     
    dest += 8;    
  }    
}    
#endif

//extern void convert_asm_neon (uint8_t * dest, uint8_t * src, int n) __asm__("convert_asm_neon");
extern "C" void convert_asm_neon (uint8_t * dest, uint8_t * src, int n);

int main(void)
{
	struct timeval tpstart,tpend;
	float timeuse; 
	int i;

	for(i=0; i<N; i++)
	{
		src[i] = i;
	}

	gettimeofday(&tpstart,NULL); 
	reference_convert(dest, src, N/3);
	gettimeofday(&tpend,NULL); 
	timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
	timeuse/=1000; 
	printf("C version Time:%fms\n",timeuse); 

#ifdef __NEON__
	gettimeofday(&tpstart,NULL); 
	neon_convert(dest, src, N/3);
	gettimeofday(&tpend,NULL); 
	timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
	timeuse/=1000; 
	printf("Neon version Time:%fms\n",timeuse); 
#endif

#ifdef __NEON_ASM__
	gettimeofday(&tpstart,NULL); 
	convert_asm_neon(dest, src, N/3);
	gettimeofday(&tpend,NULL); 
	timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec; 
	timeuse/=1000; 
	printf("ASM version Time:%fms\n",timeuse); 
#endif

    	return 0;
}
