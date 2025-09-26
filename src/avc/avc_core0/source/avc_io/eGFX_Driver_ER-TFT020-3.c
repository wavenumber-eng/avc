#include "eGFX.h"
#include "eGFX_Driver_ER-TFT020-3.h"
#include "st7789.h"
#include "fsl_gpio.h"
#include "lpspi1.h"
#include "st7789.h"

#ifdef eGFX_DRIVER_ER_TFT020_3

eGFX_VSyncCallback_t *VSyncCallback;

extern void ST7789_Initial(void);


void eGFX_InitDriver(eGFX_VSyncCallback_t VS)
{
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom1Clk, 1u);
    CLOCK_AttachClk(kPLL_DIV_to_FLEXCOMM1);

	lpspi1_init(8); // Initialize with 8-bit SPI transactions

    VSyncCallback = VS;

    ST7789_Initial();
 
#if (defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == PORTRAIT))
    LCD_SetPos(0, 239, 0, 319); // 320x240
#elif (defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == LANDSCAPE))
    LCD_SetPos(0, 319, 0, 239); // 320x240
#endif

}


void eGFX_Dump(eGFX_ImagePlane *Image)
{
    uint8_t *buffer = (uint8_t *)Image->Data;
    uint32_t packets;

    lpspi1_init(8);
  //  GPIO_PinWrite(GPIO4, 1, 1);

#if (defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == LANDSCAPE))
    LCD_SetPos(0, 319, 0, 239); // 320x240
#endif
    LCD_RS__SET;
    lpspi1_init(32);
    ST7789__display_img(buffer);
   // GPIO_PinWrite(GPIO4, 1, 0);
}

void eGFX_DumpRaw(uint8_t *buffer,
				  uint32_t length,
				  uint32_t x0,
				  uint32_t x1,
				  uint32_t y0,
				  uint32_t y1)
{

    lpspi1_init(8);

    LCD_SetPos(x0, x1, y0, y1); // 320x240

    lpspi1_init(32);
	LCD_RS__SET;
    uint32_t blk_size;
    while(length)
    {
    	if(length >= 8192)
    	{
    		blk_size = 8192;
    	}
    	else
    	{
    		blk_size = length;
    	}


        lpspi1_transfer_block(buffer, blk_size);

        buffer += blk_size;
        length-=blk_size;

    }

}




void eGFX_line_double(uint8_t *buffer,uint16_t lines)
{


    lpspi1_init(8);
   // GPIO_PinWrite(GPIO4, 1, 1);

#if (defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == LANDSCAPE))
    LCD_SetPos(0, 319, 0, 239); // 320x240
#endif

    lpspi1_init(32);

    for(uint32_t i = 0; i < lines; i++)
    {

        ST7789__display_row(&buffer[i * 640], eGFX_PHYSICAL_SCREEN_SIZE_X);
        ST7789__display_row(&buffer[i * 640], eGFX_PHYSICAL_SCREEN_SIZE_X);
     }
   // GPIO_PinWrite(GPIO4, 1, 0);

}


void eGFX_duplicate_and_dump2(eGFX_ImagePlane *Image)
{
    uint16_t *original_row_buffer = (uint16_t *)Image->Data;
    uint16_t scaled_row_buff[eGFX_PHYSICAL_SCREEN_SIZE_X];
    uint32_t delta;

    lpspi1_init(8);
   // GPIO_PinWrite(GPIO4, 1, 1);

#if (defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == LANDSCAPE))
    LCD_SetPos(0, 319, 0, 239); // 320x240
#endif

    lpspi1_init(32);

    for(uint32_t j = 0; j < 120; j++) 
    {
        delta = 160*j;
        for(uint32_t i = 0; i < 160; i++)
        {   
            scaled_row_buff[2*i] = original_row_buffer[i + delta];
            scaled_row_buff[2*i + 1] = original_row_buffer[i + delta];
        }
        ST7789__display_row((uint8_t * )scaled_row_buff, eGFX_PHYSICAL_SCREEN_SIZE_X);
        ST7789__display_row((uint8_t * )scaled_row_buff, eGFX_PHYSICAL_SCREEN_SIZE_X);
    }
   // GPIO_PinWrite(GPIO4, 1, 0);

}



uint16_t scaled_row_buff_storage1[eGFX_PHYSICAL_SCREEN_SIZE_X*2];
uint16_t scaled_row_buff_storage2[eGFX_PHYSICAL_SCREEN_SIZE_X*2];

uint8_t buff_index = 0;

uint16_t *scaled_row_buff;

uint16_t y8;
uint16_t y8_max;


void eGFX_duplicate_and_dump(eGFX_ImagePlane *Image)
{
    uint16_t *original_row_buffer = (uint16_t *)Image->Data;

    uint32_t delta;


    lpspi1_init(8);

    uint16_t temp;

#if (defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == LANDSCAPE))
    LCD_SetPos(0, 319, 0, 239); // 320x240
#endif

    lpspi1_init(32);

    delta = 0;
    buff_index = 0;
    scaled_row_buff = &scaled_row_buff_storage1[0];


    for(uint32_t j = 0; j < 120; j++)
    {

        for(uint32_t i = 0; i < 160; i++)
        {

         	temp = original_row_buffer[i + delta];

            scaled_row_buff[2*i] = temp;
            scaled_row_buff[2*i + 1] = temp;

            scaled_row_buff[2*i + 320] = temp;
            scaled_row_buff[2*i + 1 + 320] = temp;

        }

        delta +=160;

        ST7789__display_row((uint8_t * )scaled_row_buff, eGFX_PHYSICAL_SCREEN_SIZE_X*2);

        if(scaled_row_buff == &scaled_row_buff_storage2[0])
        {
        	scaled_row_buff = &scaled_row_buff_storage1[0];
        }
        else
        {
        	scaled_row_buff = &scaled_row_buff_storage2[0];
        }

   }

}


void eGFX_SetBacklight(uint8_t BacklightValue)
{
}

#endif
