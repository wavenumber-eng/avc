#include "eGFX.h"
#include "eGFX_Driver_ER-TFT020-3.h"
#include "st7789.h"
#include "fsl_gpio.h"
#include "lpspi1.h"
#include "st7789.h"

#ifdef eGFX_DRIVER_ER_TFT020_3

eGFX_ImagePlane eGFX_BackBuffer[eGFX_NUM_BACKBUFFERS];

uint8_t BackBufferStore[eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X, eGFX_PHYSICAL_SCREEN_SIZE_Y)];

eGFX_VSyncCallback_t *VSyncCallback;

extern void ST7789_Initial(void);


void eGFX_InitDriver(eGFX_VSyncCallback_t VS)
{
    eGFX_ImagePlaneInit(&eGFX_BackBuffer[0],
                        BackBufferStore,
                        eGFX_PHYSICAL_SCREEN_SIZE_X,
                        eGFX_PHYSICAL_SCREEN_SIZE_Y,
                        eGFX_IMAGE_PLANE_16BPP);

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
    GPIO_PinWrite(GPIO4, 1, 1);

#if (defined(CONFIG_DISPLAY_ORIENTATION) && (CONFIG_DISPLAY_ORIENTATION == LANDSCAPE))
    LCD_SetPos(0, 319, 0, 239); // 320x240
#endif

    lpspi1_init(32);
    ST7789__display_img(buffer);
    GPIO_PinWrite(GPIO4, 1, 0);
}


void eGFX_duplicate_and_dump2(eGFX_ImagePlane *Image)
{
    uint16_t *original_row_buffer = (uint16_t *)Image->Data;
    uint16_t scaled_row_buff[eGFX_PHYSICAL_SCREEN_SIZE_X];
    uint32_t delta;

    lpspi1_init(8);
    GPIO_PinWrite(GPIO4, 1, 1);

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
        ST7789__display_row(scaled_row_buff, eGFX_PHYSICAL_SCREEN_SIZE_X);
        ST7789__display_row(scaled_row_buff, eGFX_PHYSICAL_SCREEN_SIZE_X);
    }
    GPIO_PinWrite(GPIO4, 1, 0);

}

uint16_t scaled_row_buff_storage1[eGFX_PHYSICAL_SCREEN_SIZE_X*2];
uint16_t scaled_row_buff_storage2[eGFX_PHYSICAL_SCREEN_SIZE_X*2];

uint8_t buff_index = 0;

uint16_t *scaled_row_buff;

void eGFX_duplicate_and_dump(eGFX_ImagePlane *Image)
{
    uint16_t *original_row_buffer = (uint16_t *)Image->Data;

    uint32_t delta;

    GPIO_PinWrite(GPIO4, 1, 1);

    lpspi1_init(8);


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
            scaled_row_buff[2*i] = original_row_buffer[i + delta];
            scaled_row_buff[2*i + 1] = original_row_buffer[i + delta];


            scaled_row_buff[2*i + 320] = original_row_buffer[i + delta];
            scaled_row_buff[2*i + 1 + 320] = original_row_buffer[i + delta];

        }

        delta +=160;

        ST7789__display_row(scaled_row_buff, eGFX_PHYSICAL_SCREEN_SIZE_X*2);

        if(scaled_row_buff == &scaled_row_buff_storage2[0])
        {
        	scaled_row_buff = &scaled_row_buff_storage1[0];
        }
        else
        {
        	scaled_row_buff = &scaled_row_buff_storage2[0];
        }

   }

    GPIO_PinWrite(GPIO4, 1, 0);
}


void eGFX_SetBacklight(uint8_t BacklightValue)
{
}

#endif
