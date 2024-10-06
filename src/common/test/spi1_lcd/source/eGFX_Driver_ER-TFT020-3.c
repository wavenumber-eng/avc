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

void eGFX_SetBacklight(uint8_t BacklightValue)
{
}

#endif
