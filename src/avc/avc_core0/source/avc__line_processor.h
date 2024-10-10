#include "stdint.h"

#ifndef AVC__LINE_PROCESSOR_H_
#define AVC__LINE_PROCESSOR_H_


void avc__convert_rgb565_to_y8(uint16_t  * input,
							   uint8_t * output,
							   uint32_t  length);

void avc__convert_rgb565_to_r5(uint16_t  * input,
							   uint8_t * output,
							   uint32_t  length);


uint8_t avc__find_uint8_max(uint8_t  * input,
				      uint32_t  length);

uint8_t avc__find_uint8_min(uint8_t  * input,
				      uint32_t  length);


#endif /* AVC__LINE_PROCESSOR_H_ */
