#ifndef AVC__CAMERA_INTERFACE_H_
#define AVC__CAMERA_INTERFACE_H_

#include "stdbool.h"

static void DEMO_InitCamera(void);
static void DEMO_InitSmartDma(void);
static void SDMA_CompleteCallback(void *param);

void camera__pull_reset_pin(bool pullUp);
void camera__pull_power_pin(bool pullUp);

extern void avc__camera_interface_init();

extern volatile bool cam_data_rdy;
#endif