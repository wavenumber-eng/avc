#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "eGFX.h"
#include "Sprites_8BPP_XRGB222.h"

#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif


int main(int argc, char *argv[])
{
	eGFX_InitDriver(NULL);

  	while (!ProcessSDL_Events()) 
  	{
		SDL_Delay(24);

		eGFX_DrawStringColored(&eGFX_BackBuffer[0], "NEOS", 32, 13, &FONT_5_7_1BPP,0x03);
		eGFX_DrawStringColored(&eGFX_BackBuffer[0], "NEOS", 32, 30, &FONT_5_7_1BPP, 0x0C);
		eGFX_DrawStringColored(&eGFX_BackBuffer[0], "NEOS", 32, 50, &FONT_5_7_1BPP, 0x30);
		eGFX_DrawStringColored(&eGFX_BackBuffer[0], "NEOS", 32, 70, &FONT_5_7_1BPP, 0x3F);

		eGFX_DrawStringColored(&eGFX_BackBuffer[0], "NEOS", 32, 80, &FONT_5_7_1BPP, 0x01F);


		eGFX_Blit(&eGFX_BackBuffer[0],0,0,&Sprite_8BPP_XRGB222_neos_icon);
		eGFX_DrawLine(&eGFX_BackBuffer[0], 0, 0, 100, 100, 3);
		eGFX_Dump(&eGFX_BackBuffer[0]);			
    }

	eGFX_DeInitDriver();
	
    return 0;
}
