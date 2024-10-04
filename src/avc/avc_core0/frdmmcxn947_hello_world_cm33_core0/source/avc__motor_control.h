#ifndef AVC__MOTOR_CONTROL_H_
#define AVC__MOTOR_CONTROL_H_

#include "stdint.h"
#include "stdbool.h"

typedef struct
{
    bool invert_left_motor_direction;
    bool invert_right_motor_direction;
    bool outputs_enable;

    int8_t left_intensity;
    int8_t right_intensity;

} motor_ctrl_info_t;


extern void avc__motor_control_init();
extern void avc__set_motor_pwm(int8_t left, int8_t right);
extern void avc__enable_motor_control();
extern void avc__disable_motor_control();
uint16_t avc__dc_to_counts(int8_t new_dc, bool inverted_direction);

#endif