#include "avc__line_processor.h"
#include "stdint.h"
#include "eGFX.h"


/*
 * Convert RGB565 Color to grayscale
 *
 */
void avc__convert_rgb565_to_y8(uint16_t  * input,
							   uint8_t * output,
							   uint32_t  length)
{
	for(int i=0;i<length;i++)
	{
		output[i] = eGFX_RGB565_TO_Y8(input[i]);
	}

}

void avc__convert_rgb565_to_r5(uint16_t  * input,
							   uint8_t * output,
							   uint32_t  length)
{
	for(int i=0;i<length;i++)
	{
		output[i] = eGFX_RGB565_TO_R5(input[i]);
	}
}


uint8_t avc__find_uint8_max(uint8_t  * input,
				      uint32_t  length)
{
	uint8_t max =0;

	for(int i=0;i<length;i++)
	{
		if(input[i] > max)
		{
			max = input[i];
		}
	}

	return max;
}

uint8_t avc__find_uint8_min(uint8_t  * input,
				      uint32_t  length)
{
	uint8_t min = 0xFF;

	for(int i=0;i<length;i++)
	{
		if(input[i] < min)
		{
			min = input[i];
		}
	}

	return min;
}




