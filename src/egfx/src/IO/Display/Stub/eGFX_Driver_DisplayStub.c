#include "../../eGFX.h"

#ifdef eGFX_DRIVER_STUB

eGFX_ImagePlane eGFX_BackBuffer;

uint8_t BackBufferStore[eGFX_CALCULATE_4BPP_IMAGE_STORAGE_SPACE_SIZE(eGFX_PHYSICAL_SCREEN_SIZE_X,eGFX_PHYSICAL_SCREEN_SIZE_Y)];

eGFX_VSyncCallback_t *VSyncCallback;

void eGFX_InitDriver(eGFX_VSyncCallback_t VS)
{
    eGFX_ImagePlaneInit(&eGFX_BackBuffer,
                        BackBufferStore,
                        eGFX_PHYSICAL_SCREEN_SIZE_X,
                        eGFX_PHYSICAL_SCREEN_SIZE_Y,
                        eGFX_IMAGE_PLANE_4BPP);


    VSyncCallback = VS;
  
}

void eGFX_Dump(eGFX_ImagePlane *Image)
{

    if (VSyncCallback != NULL)
	{
		VSyncCallback(Image);
	}

}

void eGFX_SetBacklight(uint8_t BacklightValue)
{



}

#endif
