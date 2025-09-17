#include "e.h"

#ifndef AVC__IO_H_
#define AVC__IO_H_


#include "avc__master_config.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "board.h"
#include "fsl_device_registers.h"
#include "fsl_mailbox.h"
#include "fsl_gpio.h"
#include "fsl_lpflexcomm.h"
#include "fsl_lpuart.h"
#include "ipc.h"
#include "fsl_lpspi_edma.h"

#include "e.h"
#include "button.h"

#include "st7789.h"
#include "lpspi1.h"
#include "eGFX_Driver_ER-TFT020-3.h"
#include "assets/Sprites_16BPP_RGB565.h"

#include "avc__adc.h"
#include "avc__motor_control.h"
#include "avc__servo_control.h"

#include "bv_camera__interface.h"



#define IN_PORT             3
#define LEFT_BTN_PIN        21
#define RIGHT_BTN_PIN       19
#define CENTER_BTN_PIN      17
#define TEST_SW_PIN         18

extern byte_queue_t UART4_TX_Q;
extern byte_queue_t UART4_RX_Q;

extern button_t left_btn, right_btn, center_btn;

void avc__init();

#endif /* AVC__IO_H_ */
