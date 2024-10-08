#include "avc__motor_control.h"
#include "fsl_pwm.h"
#include "fsl_port.h"

#define PWM_SRC_CLK_FREQ CLOCK_GetFreq(kCLOCK_BusClk)
#define PWM_FREQUENCY (5000UL)
#define PERIOD_PULSE_CNT (PWM_SRC_CLK_FREQ / PWM_FREQUENCY)
#define MOTOR_DC_CURVE_SLOPE (65535 / 200)

motor_ctrl_info_t motor_ctrl_inst;

/**
 * @brief Initialize the PWM module for motor control, but all PWM outputs as 0.
 * If you want to turn on the PWM outputs, call avc__enable_motor_control function.
 */
void avc__motor_control_init()
{
    pwm_config_t pwmConfig;
    pwm_fault_param_t faultConfig;
    pwm_signal_param_t pwmSignal_diff[2];
    pwm_signal_param_t pwmSignal_single;
    uint16_t deadTimeVal;

    // Enable PWM1 SUB Clock
    SYSCON->PWM1SUBCTL |= (SYSCON_PWM1SUBCTL_CLK0_EN_MASK | SYSCON_PWM1SUBCTL_CLK1_EN_MASK);

    // ******************** PWM1 CONFIGURATION ********************

    PWM_GetDefaultConfig(&pwmConfig);
    pwmConfig.reloadLogic = kPWM_ReloadPwmFullCycle;
    pwmConfig.pairOperation = kPWM_ComplementaryPwmA;
    pwmConfig.enableDebugMode = true;

    /* Initialize submodule 0 */
    PWM_Init(PWM1, kPWM_Module_0, &pwmConfig);
    PWM_Init(PWM1, kPWM_Module_1, &pwmConfig);

    PWM_FaultDefaultConfig(&faultConfig);
    faultConfig.faultLevel = 1;

    PWM_SetupFaults(PWM1, kPWM_Fault_0, &faultConfig);
    PWM_SetupFaults(PWM1, kPWM_Fault_1, &faultConfig);
    PWM_SetupFaults(PWM1, kPWM_Fault_2, &faultConfig);
    PWM_SetupFaults(PWM1, kPWM_Fault_3, &faultConfig);

    // ******************** CONFIGURATION BY SUBMODULE ********************
    deadTimeVal = ((uint64_t)PWM_SRC_CLK_FREQ * 650) / 1000000000; // deadtime to about 650ns

    pwmSignal_diff[0].pwmChannel = kPWM_PwmA;
    pwmSignal_diff[0].level = kPWM_HighTrue;
    pwmSignal_diff[0].dutyCyclePercent = 50;
    pwmSignal_diff[0].deadtimeValue = deadTimeVal;
    pwmSignal_diff[0].faultState = kPWM_PwmFaultState0;
    pwmSignal_diff[0].pwmchannelenable = true;

    pwmSignal_diff[1].pwmChannel = kPWM_PwmB;
    pwmSignal_diff[1].level = kPWM_HighTrue;
    pwmSignal_diff[1].dutyCyclePercent = 50;
    pwmSignal_diff[1].deadtimeValue = deadTimeVal;
    pwmSignal_diff[1].faultState = kPWM_PwmFaultState0;
    pwmSignal_diff[1].pwmchannelenable = true;

    /*********** PWMA_SM0 - phase A, configuration, setup 2 channel ************/
    PWM_SetupPwm(PWM1, kPWM_Module_0, pwmSignal_diff, 2, kPWM_SignedCenterAligned, PWM_FREQUENCY,
                 PWM_SRC_CLK_FREQ);

    /*********** PWMA_SM1 - phase A, configuration, setup 2 channel ************/
    PWM_SetupPwm(PWM1, kPWM_Module_1, pwmSignal_diff, 2, kPWM_SignedCenterAligned, PWM_FREQUENCY,
                 PWM_SRC_CLK_FREQ);

    motor_ctrl_inst.invert_right_motor_direction = 1;

    avc__set_motor_pwm(0, 0);

    avc__disable_motor_control();
}

/**
 * @brief Connect the PWM module to the output pins,
 *
 */
void avc__enable_motor_control()
{
    if(motor_ctrl_inst.outputs_enable == 0)
    {
        PORT_SetPinMux(PORT2, 3U, kPORT_MuxAlt5); /* PORT2_2 (pin H3) is configured as PWM1_A2 */
        PORT_SetPinMux(PORT2, 4U, kPORT_MuxAlt5); /* PORT2_4 (pin K3) is configured as PWM1_A1 */
        PORT_SetPinMux(PORT2, 5U, kPORT_MuxAlt5); /* PORT2_5 (pin K1) is configured as PWM1_B1 */
        PORT_SetPinMux(PORT2, 6U, kPORT_MuxAlt5); /* PORT2_6 (pin K2) is configured as PWM1_A0 */
        PORT_SetPinMux(PORT2, 7U, kPORT_MuxAlt5); /* PORT2_7 (pin L2) is configured as PWM1_B0 */

        avc__set_motor_pwm(0, 0);
        PWM_StartTimer(PWM1, kPWM_Control_Module_0 | kPWM_Control_Module_1);
        motor_ctrl_inst.outputs_enable = 1;
    }
    
}

/**
 * @brief Disconnect the PWM module from the output pins
 *
 */
void avc__disable_motor_control()
{
        avc__set_motor_pwm(0, 0);
        //PORT_SetPinMux(PORT2, 3U, kPORT_MuxAsGpio); /* PORT2_2 disconnected from PWM1_A2 */
        PORT_SetPinMux(PORT2, 4U, kPORT_MuxAsGpio); /* PORT2_4 disconnected from PWM1_A1 */
        PORT_SetPinMux(PORT2, 5U, kPORT_MuxAsGpio); /* PORT2_5 disconnected from PWM1_B1 */
        PORT_SetPinMux(PORT2, 6U, kPORT_MuxAsGpio); /* PORT2_6 disconnected from PWM1_A0 */
        PORT_SetPinMux(PORT2, 7U, kPORT_MuxAsGpio); /* PORT2_7 disconnected from PWM1_B0 */
        motor_ctrl_inst.outputs_enable = 0;
}

/**
 * @brief Set "left" DC to PWM1_0, and "right" DC to PWM1_1
 *
 * @param left Value between [-100, 100] that refers to the left motor intensity
 * @param right Value between [-100, 100] that refers to the right motor intensity
 */
void avc__set_motor_pwm(int8_t left, int8_t right)
{
    if (motor_ctrl_inst.outputs_enable == 0)
    {
        return;
    }

    // Just execute the code if the intensity is different
    if (left != motor_ctrl_inst.left_intensity)
    {
        // Assuming PWM1_0 is connected to left motor
        PWM_UpdatePwmPeriodAndDutycycle(PWM1,
                                        kPWM_Module_0,
                                        kPWM_PwmA,
                                        kPWM_EdgeAligned,
                                        PERIOD_PULSE_CNT,
                                        avc__dc_to_counts(left, motor_ctrl_inst.invert_left_motor_direction));

        motor_ctrl_inst.left_intensity = left;
        PWM_SetPwmLdok(PWM1, kPWM_Control_Module_0, true);
    }

    if (right != motor_ctrl_inst.right_intensity)
    {
        // Assuming PWM1_1 is connected to right motor
        PWM_UpdatePwmPeriodAndDutycycle(PWM1,
                                    kPWM_Module_1,
                                    kPWM_PwmA,
                                    kPWM_EdgeAligned,
                                    PERIOD_PULSE_CNT,
                                    avc__dc_to_counts(right, motor_ctrl_inst.invert_right_motor_direction));

        motor_ctrl_inst.right_intensity = right;
        PWM_SetPwmLdok(PWM1, kPWM_Control_Module_1, true);
    }
    
}

// Return the number of counts to get the desired PWM duty cycle
// 0 means DC = 0%, 65535 means DC = 100%
// I don't really like it, but it is how the library works
uint16_t avc__dc_to_counts(int8_t new_dc, bool inverted_direction)
{
    uint16_t counts;
    counts = (65535 * ((int32_t)new_dc + 100)) / 200;

    if (inverted_direction)
    {
        counts = 65535 - counts;
    }
    return counts;
}
