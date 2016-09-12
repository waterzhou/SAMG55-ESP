/**
 * \file
 *
 * \brief Cyclic Redundancy Check.
 *
 * Copyright (c) 2012-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "stdint.h"
#include "crc32.h"
/*CRC32 Table*/
static uint32_t crc32_table[256];
static uint32_t crc_initialized = 0;

uint32_t is_crc32_init(void)
{
	return crc_initialized;
}

/* CCITT8023, Polynominal 0x04C11DB7 */
void init_crc32_table(void)
{
	uint32_t   i,j;
	uint32_t   crc;
	for(i = 0; i < 256; i++) {
		crc = i;
		for(j = 0; j < 8; j++) {
			if(crc & 1) {
				crc = (crc >> 1) ^ 0xEDB88320;
			}
			else {
				crc = crc >> 1;
			}
		}
		crc32_table[i] = crc;
	}
	crc_initialized = 1;
}

/* Calculate CRC value */
uint32_t crc32( uint8_t *buf, uint32_t len)
{
	uint32_t ret = 0xFFFFFFFF;
	uint32_t   i;
	for(i = 0; i < len;i++)
	{
		ret = crc32_table[((ret & 0xFF) ^ buf[i])] ^ (ret >> 8);
	}
	ret = ~ret;
	return ret;
}

/*crc8*/
static uint8_t calcrc_1byte(uint8_t abyte)
{
	uint8_t i,crc_1byte;
	crc_1byte = 0;

	for(i = 0; i < 8; i++){
		if(((crc_1byte^abyte)&0x01)){
			crc_1byte ^= 0x18;
			crc_1byte >>= 1;
			crc_1byte |= 0x80;
		}else
			crc_1byte >>= 1;

		abyte >>= 1;

	}

	return crc_1byte;
}


#if 0
uint8_t crc8(uint8_t *p,uint8_t len)
{
	uint8_t crc=0;
	while(len--) //len为总共要校验的字节数
	{
		crc = calcrc_1byte(crc ^ *p++);
	}
	return crc;  //若最终返回的crc为0，则数据传输正确

}
#endif


const uint8_t crctable[16] =  {0,0x07,0x0E,0x09, 0x1c,0x1b,0x12,0x15, 0x38,0x3F,0x36,0x31, 0x24,0x23,0x2A,0x2D};
const uint8_t crctable2[16] = {0,0x70,0xE0,0x90, 0xC1,0xB1,0x21,0x51, 0x83,0xF3,0x63,0x13, 0x42,0x32,0xA2,0xD2};

uint8_t FastCRC(uint8_t LastCRC, uint8_t newbyte)
{
	uint8_t index;
	index = newbyte;
	index ^= LastCRC;
	index >>= 4;
	LastCRC &= 0x0F;
	LastCRC ^= crctable2[index];
	index = LastCRC;
	index ^= newbyte;
	index &= 0x0F;
	LastCRC &= 0xF0;
	LastCRC ^= crctable[index];
	return(LastCRC);
}

/* calculate crc8 */
uint8_t crc8(uint8_t *p,uint8_t len)
{
	uint8_t crc_value = 0;
	uint8_t i;
	
	for(i=0;i<len;i++){
		crc_value = FastCRC(crc_value,p[i]);
	}
	
	return crc_value;
}


