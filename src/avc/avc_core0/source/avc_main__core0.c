

#include "avc__io.h"
#include "avc__line_processor.h"

#define CAMERA_WIDTH	160
#define CAMERA_HEIGHT	120

#define LINE_TO_PROCESS	CAMERA_HEIGHT/2

volatile uint16_t bat;
volatile uint32_t test;
int8_t left_intensity, right_intensity, servo_position;
bool testmode__motors_enable = 0;

uint8_t pot_text [64];

uint16_t * camera_frame;


uint8_t line_buffer__red[CAMERA_WIDTH];


extern uint16_t g_camera_buffer[320 *240];
extern void eGFX_DumpRaw(uint8_t *buffer);
extern void eGFX_line_double(uint8_t *buffer,uint16_t lines);
eGFX_ImagePlane CP;


int main(void)
{

	avc__init();

	CP.Data = (uint8_t *)&g_camera_buffer[0];

    while (1)
    {
      //  test = avc_ipc.core1_counter;

       // bat = avc__read_battery_voltage();

        // Test mode enable
        if(GPIO_PinRead(GPIO3, TEST_SW_PIN) == 0)
        {   
            uint8_t alpha, gamma, beta;

            if(img_ready)
            {
            	img_ready=0;
            	eGFX_DumpRaw((uint8_t *)&g_camera_buffer[0]);

            	//eGFX_line_double((uint8_t *)&g_camera_buffer[0],102);
                //eGFX_duplicate_and_dump(&CP);
            }

//            if(testmode__motors_enable == 1)
//            {
//                alpha = avc__read_alpha();
//                gamma = avc__read_gamma();
//                beta = avc__read_beta();
//
//                left_intensity = (2*(int16_t)alpha) - 100;
//                right_intensity = (2*(int16_t)gamma) - 100;
//                servo_position = (2*(int16_t)beta) - 100;
//
//                avc__set_motor_pwm(left_intensity, right_intensity);
//                avc__set_servo(servo_position);
//            }

#if 0
            if(avc__is_frame_ready())
            {
            	/*
            	 * Camera frame is a pointer to 16-bit
            	 * frame data in RGB565 format.
            	 *  You can use it like an array
            	 *  camera_frame[5] would be the 6th pixel on the 1st line
            	 *
            	 *  Since lines are 160 pixel long, you could do this:
            	 *
            	 *  camera_frame [5 + 2*CAMERA_WIDTH];
            	 *
            	 *  This would get you pixel 6 on the 3rd line
            	 */

            	camera_frame = avc__get_frame_data();

            	//Copy Line 60 to a line buffer for the red pixels
            	avc__convert_rgb565_to_r5(&camera_frame[60 * CAMERA_WIDTH],
            									line_buffer__red,
            									CAMERA_WIDTH);
            	/*
            	 * Render the line on the screen as little line segments
            	 *
            	 */
            	for(int i = 1; i<CAMERA_WIDTH ; i++)
            	{
            	       eGFX_DrawLine(&camera_image,
            	                 	  i, CAMERA_HEIGHT - line_buffer__red[i]*2,
            	                	  i-1,CAMERA_HEIGHT - line_buffer__red[i-1]*2 ,
            						  eGFX_COLOR_RGB888_TO_RGB565(0xff,0,0));
            	}


            	eGFX_DrawStringColored(&camera_image,
            							"TEST MODE",50,10,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));


                sprintf(pot_text, "alpha: %d%", left_intensity);
                eGFX_DrawStringColored(&camera_image,
            							pot_text,10,30,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

                sprintf(pot_text, "beta: %d%", servo_position);
                eGFX_DrawStringColored(&camera_image,
            							pot_text,10,40,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

                sprintf(pot_text, "gamma: %d%", right_intensity);
                eGFX_DrawStringColored(&camera_image,
            							pot_text,10,50,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

                if(button__is_active(&left_btn))
                {
                    eGFX_DrawStringColored(&camera_image,
            							"L_BTN",25,100,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
                }

                if(button__is_active(&right_btn))
                {
                    eGFX_DrawStringColored(&camera_image,
            							"R_BTN",105,100,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
                }

                if(button__up(&center_btn))
                {

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


                eGFX_duplicate_and_dump(&camera_image);

                avc__request_new_frame_for_display();
                request_frame_for_display = true;
            }
#endif
        }//end test most
        else
        {
        	//This is where we put code if we are not it test mode
        	  if(button__up(&center_btn))
        	  {
        		  //Start the car!
        	  }

        }
    }

    
}
