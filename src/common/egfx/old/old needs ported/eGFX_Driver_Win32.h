#include "eGFX_DataTypes.h"

#ifndef _GFX_DRIVER_WIN32_H
#define _GFX_DRIVER_WIN32_H



#define eGFX_PHYSICAL_SCREEN_SIZE_X	((uint16_t) 256)	//This is the actual X and Y size of the physical screen in *pixels*
#define eGFX_PHYSICAL_SCREEN_SIZE_Y	((uint16_t) 64)


//These are the prototypes for the GFX HAL
extern void	eGFX_InitDriver();
extern void	eGFX_Dump(eGFX_ImagePlane *Image);

//A Driver *Must* have a backbuffer exposed
extern eGFX_ImagePlane eGFX_BackBuffer;


#endif
