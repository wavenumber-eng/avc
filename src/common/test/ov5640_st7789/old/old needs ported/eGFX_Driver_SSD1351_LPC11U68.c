/*
 * GFX_Driver_SSD1351_LPC11U68.c
 *
 *  Created on: Oct 25, 2014
 *      Author: Alexander Hiam <alex@graycat.io>
 *
 */

#include "chip.h"
#include "eGFX_DataTypes.h"
#include "GFX_Driver_SSD1351_LPC11U68.h"
#include "eGFX.h"


uint8_t BackBufferStore[eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(
						  eGFX_PHYSICAL_SCREEN_SIZE_X,
						  eGFX_PHYSICAL_SCREEN_SIZE_Y
						)];


#define LCD_CS_PORT  0
#define LCD_CS_PIN   7
#define LCD_DC_PORT  0
#define LCD_DC_PIN   6
#define LCD_RST_PORT 2
#define LCD_RST_PIN  15



#define SSD1351_CMD_SETCOLUMN 0x15
#define SSD1351_CMD_SETROW 0x75
#define SSD1351_CMD_WRITERAM 0x5C
#define SSD1351_CMD_READRAM 0x5D
#define SSD1351_CMD_SETREMAP 0xA0
#define SSD1351_CMD_STARTLINE 0xA1
#define SSD1351_CMD_DISPLAYOFFSET 0xA2
#define SSD1351_CMD_DISPLAYALLOFF 0xA4
#define SSD1351_CMD_DISPLAYALLON 0xA5
#define SSD1351_CMD_NORMALDISPLAY 0xA6
#define SSD1351_CMD_INVERTDISPLAY 0xA7
#define SSD1351_CMD_FUNCTIONSELECT 0xAB
#define SSD1351_CMD_DISPLAYOFF 0xAE
#define SSD1351_CMD_DISPLAYON 0xAF
#define SSD1351_CMD_PRECHARGE 0xB1
#define SSD1351_CMD_DISPLAYENHANCE 0xB2
#define SSD1351_CMD_CLOCKDIV 0xB3
#define SSD1351_CMD_SETVSL 0xB4
#define SSD1351_CMD_SETGPIO 0xB5
#define SSD1351_CMD_PRECHARGE2 0xB6
#define SSD1351_CMD_SETGRAY 0xB8
#define SSD1351_CMD_USELUT 0xB9
#define SSD1351_CMD_PRECHARGELEVEL 0xBB
#define SSD1351_CMD_VCOMH 0xBE
#define SSD1351_CMD_CONTRASTABC 0xC1
#define SSD1351_CMD_CONTRASTMASTER 0xC7
#define SSD1351_CMD_MUXRATIO 0xCA
#define SSD1351_CMD_COMMANDLOCK 0xFD
#define SSD1351_CMD_HORIZSCROLL 0x96
#define SSD1351_CMD_STOPSCROLL 0x9E
#define SSD1351_CMD_STARTSCROLL 0x9F

#define SSD1351_DC_DATA 1
#define SSD1351_DC_CMD  0


#define SSD1351_SPI_BITRATE 12000000
#define SSD1351_SPI_CLOCKMODE SSP_CLOCK_MODE0 // base=low, capture on rising
#define SSD1351_SPI_DATA_BITS SSP_BITS_8


void SSD1351_ConfigureSPI()
{
	Chip_SSP_Disable(LPC_SSP1);
	Chip_SSP_SetBitRate(LPC_SSP1, SSD1351_SPI_BITRATE);
	Chip_SSP_SetFormat(LPC_SSP1, SSD1351_SPI_DATA_BITS,
					   SSP_FRAMEFORMAT_SPI, SSD1351_SPI_CLOCKMODE);
	Chip_SSP_SetMaster(LPC_SSP1, 1);
	Chip_SSP_Enable(LPC_SSP1);
}



eGFX_PixelState rgbToPixel(uint8_t red, uint8_t green, uint8_t blue) {
	// Converts to 16-bit 565 color, with bytes reversed because the bytes are
	// stored LSB first in the uint16_t and therefore get TXed reversed by the
	// SSP.
	//     ggg  bbbbb  rrrrr  ggg
	//  low g bits          high g bits
	uint16_t color;
	color = green>>2;
	color <<= 5;
	color |= blue>>3;
	color <<= 5;
	color |= red>>3;
	color <<= 3;
	color |= green>>5;
	return color;
}

eGFX_PixelState fadePixel16bit(eGFX_PixelState pixel, eGFX_PixelState multiplier) {
	uint32_t red, green, blue;
	if ((!pixel) || (!multiplier)) return 0x00000000;
	if (multiplier == 0xff) return pixel;

	red = (((pixel>>3) & 0x1f) * multiplier)>>8;
	green = (((pixel & 0x7)<<3 | pixel>>13) * multiplier)>>8;
	blue = (((pixel>>8) & 0x1f) * multiplier)>>8;

	return rgbToPixel((uint8_t) red, (uint8_t) green, (uint8_t) blue);
	//return rgbToPixel((uint8_t) ((multiplier*red)>>8),
    //                  (uint8_t) ((multiplier*green)>>8),
    //                  (uint8_t) ((multiplier*blue)>>8));
}

void SSD1351_writeCommand(uint8_t command)
{
	Chip_GPIO_SetPinState(LPC_GPIO, LCD_DC_PORT, LCD_DC_PIN, SSD1351_DC_CMD);
	Chip_GPIO_SetPinState(LPC_GPIO, LCD_CS_PORT, LCD_CS_PIN, 0);
	Chip_SSP_WriteFrames_Blocking(LPC_SSP1, (uint8_t*) (&command), 1);
	Chip_GPIO_SetPinState(LPC_GPIO, LCD_CS_PORT, LCD_CS_PIN, 1);
}

void SSD1351_writeData(uint8_t data)
{
	Chip_GPIO_SetPinState(LPC_GPIO, LCD_DC_PORT, LCD_DC_PIN, SSD1351_DC_DATA);
	Chip_GPIO_SetPinState(LPC_GPIO, LCD_CS_PORT, LCD_CS_PIN, 0);
	Chip_SSP_WriteFrames_Blocking(LPC_SSP1, (uint8_t*) (&data), 1);
	Chip_GPIO_SetPinState(LPC_GPIO, LCD_CS_PORT, LCD_CS_PIN, 1);
}

void SSD1351_Init()
{

	SSD1351_ConfigureSPI();

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LCD_CS_PORT, LCD_CS_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LCD_DC_PORT, LCD_DC_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LCD_RST_PORT, LCD_RST_PIN);

	Chip_GPIO_SetPinState(LPC_GPIO, LCD_CS_PORT, LCD_CS_PIN, 0);

	Chip_GPIO_SetPinState(LPC_GPIO, LCD_RST_PORT, LCD_RST_PIN, 1);

	for(int i=0;i<10000;i++){}
	Chip_GPIO_SetPinState(LPC_GPIO, LCD_RST_PORT, LCD_RST_PIN, 0);

	for(int i=0;i<10000;i++){}
	Chip_GPIO_SetPinState(LPC_GPIO, LCD_RST_PORT, LCD_RST_PIN, 1);

	for(int i=0;i<10000;i++){}

	// Initialization Sequence
	SSD1351_writeCommand( SSD1351_CMD_COMMANDLOCK); // set command lock
	SSD1351_writeData( 0x12);
	SSD1351_writeCommand( SSD1351_CMD_COMMANDLOCK); // set command lock
	SSD1351_writeData( 0xB1);
	SSD1351_writeCommand( SSD1351_CMD_DISPLAYOFF); // 0xAE
	SSD1351_writeCommand( SSD1351_CMD_CLOCKDIV); // 0xB3
	SSD1351_writeCommand( 0xF1); // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
	SSD1351_writeCommand( SSD1351_CMD_MUXRATIO);
	SSD1351_writeData( 127);
	SSD1351_writeCommand( SSD1351_CMD_SETREMAP);
	SSD1351_writeData( 0x74);
	SSD1351_writeCommand( SSD1351_CMD_SETCOLUMN);
	SSD1351_writeData( 0x00);
	SSD1351_writeData( 0x7F);
	SSD1351_writeCommand( SSD1351_CMD_SETROW);
	SSD1351_writeData( 0x00);
	SSD1351_writeData( 0x7F);
	SSD1351_writeCommand( SSD1351_CMD_STARTLINE); // 0xA1
	SSD1351_writeData( 96);
	SSD1351_writeCommand( SSD1351_CMD_DISPLAYOFFSET); // 0xA2
	SSD1351_writeData( 0x0);
	SSD1351_writeCommand( SSD1351_CMD_SETGPIO);
	SSD1351_writeData( 0x00);
	SSD1351_writeCommand( SSD1351_CMD_FUNCTIONSELECT);
	SSD1351_writeData( 0x01); // internal (diode drop)
	SSD1351_writeCommand( SSD1351_CMD_PRECHARGE); // 0xB1
	SSD1351_writeCommand( 0x32);
	SSD1351_writeCommand( SSD1351_CMD_VCOMH); // 0xBE
	SSD1351_writeCommand( 0x05);
	SSD1351_writeCommand( SSD1351_CMD_NORMALDISPLAY); // 0xA6
	SSD1351_writeCommand( SSD1351_CMD_CONTRASTABC);
	SSD1351_writeData( 0xC8);
	SSD1351_writeData( 0x80);
	SSD1351_writeData( 0xC8);
	SSD1351_writeCommand( SSD1351_CMD_CONTRASTMASTER);
	SSD1351_writeData( 0x0F);
	SSD1351_writeCommand( SSD1351_CMD_SETVSL );
	SSD1351_writeData( 0xA0);
	SSD1351_writeData( 0xB5);
	SSD1351_writeData( 0x55);
	SSD1351_writeCommand( SSD1351_CMD_PRECHARGE2);
	SSD1351_writeData( 0x01);
	SSD1351_writeCommand( SSD1351_CMD_DISPLAYON);
}

eGFX_ImagePlane eGFX_BackBuffer;

void eGFX_InitDriver()
{
	SSD1351_Init();
	eGFX_ImagePlaneInit(&eGFX_BackBuffer, BackBufferStore, eGFX_PHYSICAL_SCREEN_SIZE_X,eGFX_PHYSICAL_SCREEN_SIZE_Y, eGFX_IMAGE_PLANE_16BPP_3553);

}

void eGFX_Dump(eGFX_ImagePlane *Image)
{
	uint32_t size;
	// Go to 0,0
	SSD1351_writeCommand(SSD1351_CMD_SETCOLUMN);
	SSD1351_writeData(0);
	SSD1351_writeData(eGFX_PHYSICAL_SCREEN_SIZE_X-1);
	SSD1351_writeCommand(SSD1351_CMD_SETROW);
	SSD1351_writeData(0);
	SSD1351_writeData(eGFX_PHYSICAL_SCREEN_SIZE_Y-1);

	SSD1351_writeCommand( SSD1351_CMD_WRITERAM);

	Chip_GPIO_SetPinState(LPC_GPIO, LCD_DC_PORT, LCD_DC_PIN, SSD1351_DC_DATA); // data mode
	Chip_GPIO_SetPinState(LPC_GPIO, LCD_CS_PORT, LCD_CS_PIN, 0);

	size = eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(
			eGFX_PHYSICAL_SCREEN_SIZE_X,
			eGFX_PHYSICAL_SCREEN_SIZE_Y
		);

	Chip_SSP_WriteFrames_Blocking(LPC_SSP1, (uint8_t*) (Image->Data), size);

	Chip_GPIO_SetPinState(LPC_GPIO, LCD_CS_PORT, LCD_CS_PIN, 1);
}




