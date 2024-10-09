

#include "avc__io.h"



volatile uint16_t bat;
volatile uint32_t test;
int8_t left_intensity, right_intensity, servo_position;
bool testmode__motors_enable = 0;
uint8_t pot_text [64];
uint8_t alpha, gamma, beta;

int32_t current_speed_left = 0;
int32_t current_speed_right = 0;


int main(void)
{

	avc__init();

    while (1)
    {

        bat = avc__read_battery_voltage();
        alpha = avc__read_alpha();
        gamma = avc__read_gamma();
        beta = avc__read_beta();


        // Test mode enable
        if(GPIO_PinRead(GPIO3, TEST_SW_PIN) == 0)
        {   

            left_intensity = (2*(int16_t)alpha) - 100;
            right_intensity = (2*(int16_t)gamma) - 100;
            servo_position = (2*(int16_t)beta) - 100;

            if(testmode__motors_enable == 1)
            {

                if(current_speed_left<left_intensity)
                {
                	current_speed_left++;
                }
                else if(current_speed_left>left_intensity)
                {
                	current_speed_left--;
                }

                if(current_speed_right<right_intensity)
                {
                	current_speed_right++;
                }
                else if(current_speed_right>right_intensity)
                {
                	current_speed_right--;
                }

                avc__set_motor_pwm(current_speed_left, current_speed_right);
                avc__set_servo(servo_position);
            }
            else
            {
            	current_speed_left = 0;
            	current_speed_right = 0;
            	 avc__set_motor_pwm(current_speed_left, current_speed_right);
            }

            if((request_frame_for_display == false)  && (mem_transfer_done == true))
            {
            	eGFX_DrawStringColored(&camera_image,
            							"TEST MODE",50,10,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));


                sprintf(pot_text, "alpha: %d%", left_intensity);
                eGFX_DrawStringColored(&camera_image,
            							pot_text,10,50,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

                sprintf(pot_text, "beta: %d%", servo_position);
                eGFX_DrawStringColored(&camera_image,
            							pot_text,10,70,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

                sprintf(pot_text, "gamma: %d%", right_intensity);
                eGFX_DrawStringColored(&camera_image,
            							pot_text,10,60,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));


                if(button__hal_read_port_pin(IN_PORT, LEFT_BTN_PIN) == 0)
                {
                    eGFX_DrawStringColored(&camera_image,
            							"L_BTN",25,100,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
                }

                if(button__hal_read_port_pin(IN_PORT, RIGHT_BTN_PIN) == 0)
                {
                    eGFX_DrawStringColored(&camera_image,
            							"R_BTN",105,100,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
                }

                if(button__hal_read_port_pin(IN_PORT, CENTER_BTN_PIN) == 0)
                {
                    eGFX_DrawStringColored(&camera_image,
            							"C_BTN",65,100,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
                }

                //if(button__down(&center_btn))
                if(button__up(&center_btn))
                {
                    button__reset_state(&center_btn);

                    if(testmode__motors_enable == 0)
                    {
                        testmode__motors_enable = 1;
                        avc__enable_motor_control();
                    }

                    else if(testmode__motors_enable == 1)
                    {
                        testmode__motors_enable = 0;
                        avc__disable_motor_control();
                    }
                }

                if(testmode__motors_enable == 1)
                {
                    eGFX_DrawStringColored(&camera_image,
            							"Motors active",40,20,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
                }
                else 
                {
                    eGFX_DrawStringColored(&camera_image,
            							"Motors inactive",40,20,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
                }


                #if CONFIG__OV7670_IS_160x120 == 1
                    eGFX_duplicate_and_dump(&camera_image);
                #else
                    eGFX_Dump(&camera_image);
                #endif
                request_frame_for_display = true;
            }

        }
    }
    
}
