/*
 * GFX_Driver_SSD1351_LPC11U68.h
 *
 *  Created on: Oct 25, 2014
 *      Author: Alexander Hiam <alex@graycat.io>
 *
 * An driver for the eGFX library for using the SSD1351 OLED controller with
 * the NXP LPC11U68.
 */

#ifndef GFX_DRIVER_SSD1351_LPC11U68_H_
#define GFX_DRIVER_SSD1351_LPC11U68_H_

#include "chip.h"
#include "eGFX_DataTypes.h"

#define	SSD1351_BLACK   0x0000
#define	SSD1351_RED		0x00F8
#define	SSD1351_GREEN	0xE007
#define	SSD1351_BLUE	0x1F00
#define SSD1351_ORANGE	0x00FC
#define SSD1351_PURPLE	0x1958
#define SSD1351_YELLOW	0xE0FF
#define SSD1351_WHITE   0xFFFF
#define SSD1351_GRAY	0x1084



//LCD GFX Driver Configuration.
// The Symbols below must be defined for the GFX library to work.
#define eGFX_DRIVER_PRESENT	//This is must be define for the main GFX library to compile
#define eGFX_PHYSICAL_SCREEN_SIZE_X	((uint16_t) 128)
#define eGFX_PHYSICAL_SCREEN_SIZE_Y	((uint16_t) 96)


//These are the prototypes for the GFX HAL
extern void	eGFX_InitDriver();
extern void	eGFX_Dump(eGFX_ImagePlane *Image);

//A Driver *Must* have a backbuffer exposed
extern eGFX_ImagePlane eGFX_BackBuffer;

#endif
