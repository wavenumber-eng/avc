

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


extern uint16_t g_camera_buffer[320 *200];

extern void eGFX_line_double(uint8_t *buffer,uint16_t lines);
eGFX_ImagePlane CP;

volatile bool next_frame_ready;
volatile uint16_t *next_frame_buf;
//irq context


eGFX_ImagePlane top_info;
eGFX_ImagePlane camera_view;

uint8_t top_info_buffer[eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(320,40)];

void avc__next_frame(uint16_t *buf)
{
	next_frame_ready = true;
	next_frame_buf = buf;
	camera_view.Data = (uint8_t *)buf;
}

int main(void)
{

	avc__init();


	top_info.Data = top_info_buffer;
	top_info.SizeX = 320;
	top_info.SizeY = 40;
	top_info.Type = eGFX_IMAGE_PLANE_16BPP_RGB565;


	camera_view.SizeX = 320;
	camera_view.SizeY = 200;
	camera_view.Type = eGFX_IMAGE_PLANE_16BPP_RGB565;
	camera_view.Data = (uint8_t *)next_frame_buf;



    while (1)
    {
    	uint8_t alpha, gamma, beta;

        bat = avc__read_battery_voltage();

        alpha = avc__read_alpha();
        gamma = avc__read_gamma();
        beta = avc__read_beta();
        // Test mode enable
        if(GPIO_PinRead(GPIO3, TEST_SW_PIN) == 0)
        {   



            if(next_frame_ready)
            {
            	next_frame_ready=0;


                if(testmode__motors_enable == 1)
                {
                    alpha = avc__read_alpha();
                    gamma = avc__read_gamma();
                    beta = avc__read_beta();

                    left_intensity = (2*(int16_t)alpha) - 100;
                    right_intensity = (2*(int16_t)gamma) - 100;
                    servo_position = (2*(int16_t)beta) - 100;

                    avc__set_motor_pwm(left_intensity, right_intensity);
                    avc__set_servo(servo_position);
                }



            	eGFX_ImagePlane_Clear(&top_info);


              	eGFX_DrawStringColored(&top_info,
                    							"TEST MODE",100,0,
        										&FONT_10_14_1BPP,
        										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

                sprintf(pot_text, "alpha: %d%", left_intensity);
                eGFX_DrawStringColored(&top_info,
            							pot_text,0,20,
										&FONT_10_14_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

                sprintf(pot_text, "beta: %d%", servo_position);
                eGFX_DrawStringColored(&top_info,
                						pot_text,100,20,
										&FONT_10_14_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

                sprintf(pot_text, "gamma: %d%", right_intensity);
                eGFX_DrawStringColored(&top_info,
                						pot_text,200,20,
										&FONT_10_14_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));


                if(button__is_active(&left_btn))
                {
                    eGFX_DrawStringColored(&camera_view,
            							"L_BTN",25,100,
										&FONT_5_7_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
                }

                if(button__is_active(&right_btn))
                {
                    eGFX_DrawStringColored(&camera_view,
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



            	eGFX_DumpRaw((uint8_t *)top_info.Data,
                					  320*40*2,
                					  0,
                					  319,
                					  0,
                					  39);


            	eGFX_DumpRaw((uint8_t *)next_frame_buf,
            					  320*200*2,
            					  0,
            					  319,
            					  40,
            					  39+200);

            	//eGFX_line_double((uint8_t *)&g_camera_buffer[0],102);
                //eGFX_duplicate_and_dump(&CP);
            }


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
