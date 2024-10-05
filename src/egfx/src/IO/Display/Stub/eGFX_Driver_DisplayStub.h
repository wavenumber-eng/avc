#include "../../eGFX.h"

#ifndef eGFX_DISPLAY_DRIVER_PRESENT

#ifndef eGFX_DRIVER_STUB
#define eGFX_DRIVER_STUB

  /*
      Required symbols for any driver
  */

#define eGFX_PHYSICAL_SCREEN_SIZE_X     ((uint16_t) 128)
#define eGFX_PHYSICAL_SCREEN_SIZE_Y     ((uint16_t) 64)

#define eGFX_DISPLAY_DRIVER_IMAGE_PLANE_TYPE   eGFX_IMAGE_PLANE_4BPP


  /*
      eGFX_InitDriver() should do any hardware related init for the display and
      setup eGFX_BackBuffer
  */

extern void  eGFX_InitDriver(eGFX_VSyncCallback_t VS));

/*
     eGFX_Dump() should dump an ImagePlane to the physical screen.
*/

  extern void eGFX_Dump(eGFX_ImagePlane *Image);

  /*
      A driver should expose at least one back buffer that is the physical screen size and have
      a matching color space.
  */

 
#define eGFX_NUM_BACKBUFFERS	2

extern eGFX_ImagePlane eGFX_BackBuffer[eGFX_NUM_BACKBUFFERS];

  /*
   *  Integer value 0 (off) to 100 (full on)
   *
   */

  extern void eGFX_SetBacklight(uint8_t BacklightValue);

  #endif

#endif
