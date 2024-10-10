#ifndef AVC__CAMERA_INTERFACE_H_
#define AVC__CAMERA_INTERFACE_H_

#include "stdbool.h"
#include "eGFX_Driver_ER-TFT020-3.h"

/* Symbols to be used with GPIO driver */
#define BOARD_INITCAMERAPINS_CAM_RST_GPIO GPIO1                /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITCAMERAPINS_CAM_RST_GPIO_PIN 19U              /*!<@brief GPIO pin number */
#define BOARD_INITCAMERAPINS_CAM_RST_GPIO_PIN_MASK (1U << 19U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITCAMERAPINS_CAM_RST_PORT PORT1                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITCAMERAPINS_CAM_RST_PIN 19U                   /*!<@brief PORT pin number */
#define BOARD_INITCAMERAPINS_CAM_RST_PIN_MASK (1U << 19U)      /*!<@brief PORT pin mask */
                                                               /* @} */

/* Symbols to be used with GPIO driver */
#define BOARD_INITCAMERAPINS_CAM_PDWN_GPIO GPIO1                /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITCAMERAPINS_CAM_PDWN_GPIO_PIN 18U              /*!<@brief GPIO pin number */
#define BOARD_INITCAMERAPINS_CAM_PDWN_GPIO_PIN_MASK (1U << 18U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITCAMERAPINS_CAM_PDWN_PORT PORT1                /*!<@brief PORT peripheral base pointer */
#define BOARD_INITCAMERAPINS_CAM_PDWN_PIN 18U                   /*!<@brief PORT pin number */
#define BOARD_INITCAMERAPINS_CAM_PDWN_PIN_MASK (1U << 18U)      /*!<@brief PORT pin mask */
          
          
static void DEMO_InitCamera(void);
static void DEMO_InitSmartDma(void);
static void SDMA_CompleteCallback(void *param);

void camera__pull_reset_pin(bool pullUp);
void camera__pull_power_pin(bool pullUp);

extern void avc__camera_interface_init();

bool    avc__is_frame_ready();
void 	avc__request_new_frame_for_display();
uint16_t * avc__get_frame_data();

extern volatile bool cam_data_rdy;
extern eGFX_ImagePlane camera_image;
#endif
