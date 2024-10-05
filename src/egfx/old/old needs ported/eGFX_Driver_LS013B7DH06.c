#include <eGFX_Driver.h>
#include "chip.h"
#include <cr_section_macros.h>
#include "eGFX.h"
#include "eGFX_DataTypes.h"
#include "MemorySectionConfig.h"


eGFX_ImagePlane eGFX_BackBuffer;


__BSS(UI_BACKBUFFER_LOCATION) static uint16_t FrameBuffer[eGFX_CALCULATE_3BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X,eGFX_PHYSICAL_SCREEN_SIZE_Y)];

uint8_t FrameInversionFlag = 0x00;



#define MEM_LCD_CLK_PORT		 3,3
#define MEM_LCD_DATA_PORT		  3,7

#define MEM_LCD_DISP_PORT		 3,4
#define MEM_LCD_DISP_GPIO_PORT   1,14

#define MEM_LCD_CS_PORT		     3,5
#define MEM_LCD_CS_GPIO_PORT     1,15

#define MEM_LCD_COM_PORT		 4,0
#define MEM_LCD_COM_GPIO_PORT    2,0


//P3_3 is CLK
//P3_4 is DSP
//P3_5 is SSEL
//P3_7    is DATA
//P4_0 is EXT_COM
#define MEM_LCD_CS_ACTIVE  		    Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,MEM_LCD_CS_GPIO_PORT)
#define MEM_LCD_CS_INACTIVE		    Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,MEM_LCD_CS_GPIO_PORT)

#define MEM_LCD_COM_ACTIVE  		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,MEM_LCD_COM_GPIO_PORT)
#define MEM_LCD_COM_INACTIVE		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,MEM_LCD_COM_GPIO_PORT)

#define MEM_LCD_DISP_ACTIVE  		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,MEM_LCD_DISP_GPIO_PORT)
#define MEM_LCD_DISP_INACTIVE		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,MEM_LCD_DISP_GPIO_PORT)


const unsigned char bitReverse[] =
{
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};


void MemLCD_8BitTransfer(uint8_t Data)
{

	LPC_SPI->DR = Data;

	while((LPC_SPI->SR & 1<<7) == 0)
	{
		//Wait for the SPIF flag
	}

	Data = LPC_SPI->DR; //We need this to clear SPIF
}


void MemLCD_8BitTransfer_Reversed(uint8_t Data)
{

	MemLCD_8BitTransfer(bitReverse[Data]);

}

void MemoryLCD_DataMaintain()
{
    FrameInversionFlag ^= 0x02; // Toggle bit 2 to keep display happy

    MEM_LCD_CS_ACTIVE;
    MemLCD_8BitTransfer_Reversed(FrameInversionFlag);
    MemLCD_8BitTransfer_Reversed(0);
    MEM_LCD_CS_ACTIVE;



}

void MemoryLCD_Clear()
{

	FrameInversionFlag ^= 0x02; // Toggle bit 2 to keep display happy
    MEM_LCD_CS_ACTIVE;
    MemLCD_8BitTransfer_Reversed(FrameInversionFlag | (1<<2));
    MemLCD_8BitTransfer_Reversed(0);
    MEM_LCD_CS_INACTIVE;

}


void eGFX_InitDriver()
{
	eGFX_ImagePlaneInit(&eGFX_BackBuffer,(uint8_t *)FrameBuffer,
    					eGFX_PHYSICAL_SCREEN_SIZE_X,
						eGFX_PHYSICAL_SCREEN_SIZE_Y,
						eGFX_IMAGE_PLANE_3BPP_RGB);

	Chip_SCU_PinMuxSet(MEM_LCD_CLK_PORT,SCU_MODE_FUNC1 | SCU_PINIO_FAST);  //Clock is hardware SPI
	Chip_SCU_PinMuxSet(MEM_LCD_DATA_PORT,SCU_MODE_FUNC1 | SCU_PINIO_FAST);  //DATA is hardware SPI

	Chip_SCU_PinMuxSet(MEM_LCD_DISP_PORT,SCU_MODE_FUNC0 | SCU_PINIO_FAST);  //GPIO
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,MEM_LCD_DISP_GPIO_PORT);

	Chip_SCU_PinMuxSet(MEM_LCD_CS_PORT,SCU_MODE_FUNC0 | SCU_PINIO_FAST);  //GPIO
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,MEM_LCD_CS_GPIO_PORT);

	Chip_SCU_PinMuxSet(MEM_LCD_COM_PORT,SCU_MODE_FUNC0 | SCU_PINIO_FAST);  //GPIO
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,MEM_LCD_COM_GPIO_PORT);


	MEM_LCD_DISP_ACTIVE;

	Chip_Clock_Enable(CLK_SPI);
	//setup SPI0
	MEM_LCD_CS_INACTIVE;

	LPC_SPI->CR = 1<<5;  //Master Mode

    Chip_SPI_SetBitRate(LPC_SPI,2500000);

	MemoryLCD_Clear();
}

int q = 0;

//This will flag the M0 to dump the image buffer
void eGFX_Dump(eGFX_ImagePlane *Image)
{

	FrameInversionFlag ^= 0x02; // Toggle bit 2 to keep display happy

	MEM_LCD_CS_ACTIVE;

	for(int i=0; i<128 ; i++)
	{

		MemLCD_8BitTransfer_Reversed(FrameInversionFlag | (1<<0));
		MemLCD_8BitTransfer_Reversed(i+1);

			for(int j=0;j<48;j++)
			{
				MemLCD_8BitTransfer_Reversed(Image->Data[i*48 + j]);
			}
	}

	 MemLCD_8BitTransfer(0);
     MemLCD_8BitTransfer(0);
     MEM_LCD_CS_INACTIVE;

}

