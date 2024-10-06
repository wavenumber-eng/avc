#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "eGFX.h"
#include "Sprites_32BPP_XRGB888.h"

#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

int main(int argc, char *argv[])
{
	

	eGFX_InitDriver(NULL);

	uint32_t* raw_image_array;
	raw_image_array = (uint32_t*)(eGFX_BackBuffer[0].Data);

  	while (!ProcessSDL_Events()) 
  	{
		SDL_Delay(24);

	//	eGFX_DrawStringColored(&eGFX_BackBuffer[0], "NEOS", 32, 13, &FONT_5_7_1BPP,0x9F1f000);
			
		
		//eGFX_Blit(&eGFX_BackBuffer[0],0,0,&Sprite_32BPP_XRGB888_neos_icon);

		//eGFX_DrawLine(&eGFX_BackBuffer[0], 0, 0, 32, 32, 255);

		raw_image_array[0] = 255;

		eGFX_PutPixel(&eGFX_BackBuffer[0], 5, 5, 255 << 8);

		eGFX_Dump(&eGFX_BackBuffer[0]);			
    }

	eGFX_DeInitDriver();
	
    return 0;
}
