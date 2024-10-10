#include "eGFX.h"

#ifndef eGFX_DISPLAY_DRIVER_PRESENT

#ifndef  eGFX_DRIVER_ER_TFT020_3
#define  eGFX_DRIVER_ER_TFT020_3

  /*
      Required symbols for any driver
  */

//#define eGFX_PHYSICAL_SCREEN_SIZE_X     ((uint16_t) 240)
//#define eGFX_PHYSICAL_SCREEN_SIZE_Y     ((uint16_t) 320)
#define eGFX_PHYSICAL_SCREEN_SIZE_X     ((uint16_t) 320)
#define eGFX_PHYSICAL_SCREEN_SIZE_Y     ((uint16_t) 240)

#define eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE   eGFX_IMAGE_PLANE_16BPP_RGB565


  /*
      eGFX_InitDriver() should do any hardware related init for the display and
      setup eGFX_BackBuffer
  */

extern void  eGFX_InitDriver(eGFX_VSyncCallback_t VS);

/*
     eGFX_Dump() should dump an ImagePlane to the physical screen.
*/

extern void eGFX_Dump(eGFX_ImagePlane *Image);
extern void eGFX_duplicate_and_dump(eGFX_ImagePlane *Image);
  /*
      A driver should expose at least one back buffer that is the physical screen size and have
      a matching color space.
  */

 
#define eGFX_NUM_BACKBUFFERS	1

extern eGFX_ImagePlane eGFX_BackBuffer[eGFX_NUM_BACKBUFFERS];

  /*
   *  Integer value 0 (off) to 100 (full on)
   *
   */

  extern void eGFX_SetBacklight(uint8_t BacklightValue);

  #endif

#endif
