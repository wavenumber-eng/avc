#ifndef CAMERA_H
#define CAMERA_H

#include "fsl_common.h"


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


extern volatile uint8_t img_ready;

extern uint8_t bv_camera__init();

uint16_t * camera__get_buffer();

#endif
