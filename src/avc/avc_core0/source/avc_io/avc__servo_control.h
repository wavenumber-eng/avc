#ifndef AVC__SERVO_CONTROL_H_
#define AVC__SERVO_CONTROL_H_

#include "stdint.h"

extern void avc__servo_control_init();

//position is -1.0f to 1.0f
void avc__set_servo(float position);

#endif
