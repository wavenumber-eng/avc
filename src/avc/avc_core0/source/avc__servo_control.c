#include "avc__servo_control.h"
#include "fsl_pwm.h"

#define PWM_SRC_CLK_FREQ            CLOCK_GetFreq(kCLOCK_BusClk)
#define SERVO_DC_CURVE_SLOPE        (65535/4000)

void avc__servo_control_init()
{
    pwm_config_t pwmConfig;
    pwm_signal_param_t pwmSignal_single;

    /* Enable PWM1 SUB Clockn */
    SYSCON->PWM1SUBCTL |= SYSCON_PWM1SUBCTL_CLK2_EN_MASK;

    PWM_GetDefaultConfig(&pwmConfig);
    pwmConfig.reloadLogic = kPWM_ReloadPwmFullCycle;
    pwmConfig.enableDebugMode = true;
    pwmConfig.prescale = kPWM_Prescale_Divide_64;
    pwmConfig.pairOperation   = kPWM_Independent;
    PWM_Init(PWM1, kPWM_Module_2, &pwmConfig);

    pwmSignal_single.pwmChannel       = kPWM_PwmB;
    pwmSignal_single.level            = kPWM_HighTrue;
    pwmSignal_single.dutyCyclePercent = 5; 
    pwmSignal_single.faultState       = kPWM_PwmFaultState0;
    pwmSignal_single.pwmchannelenable = true;   

    /*********** PWMA_SM2 - phase A, configuration, setup 1 channel ************/
    PWM_SetupPwm(PWM1, kPWM_Module_2, &pwmSignal_single, 1, kPWM_SignedCenterAligned, 50U,
                 PWM_SRC_CLK_FREQ);

    PWM_SetPwmLdok(PWM1, kPWM_Control_Module_2, true);
    PWM_StartTimer(PWM1, kPWM_Control_Module_2);

    avc__set_servo(0);

}

void avc__set_servo(int8_t position)
{
    uint16_t position_counts = (65535 * ((int32_t)position + 300)) / 4000;

    PWM_UpdatePwmDutycycleHighAccuracy(PWM1,
                                        kPWM_Module_2,
                                        kPWM_PwmB,
                                        kPWM_SignedCenterAligned,
                                        position_counts);

    PWM_SetPwmLdok(PWM1, kPWM_Control_Module_2, true);
}