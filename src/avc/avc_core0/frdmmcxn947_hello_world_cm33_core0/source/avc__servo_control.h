#ifndef AVC__SERVO_CONTROL_H_
#define AVC__SERVO_CONTROL_H_

#include "stdint.h"

extern void avc__servo_control_init();
extern void avc__set_servo(int8_t position);

#endif