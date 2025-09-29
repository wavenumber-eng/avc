

#include "avc__io.h"
#include "avc__line_processor.h"

#define CAMERA_WIDTH	160
#define CAMERA_HEIGHT	120

#define LINE_TO_PROCESS	CAMERA_HEIGHT/2

volatile uint16_t bat;
volatile uint32_t test;
float left_intensity, right_intensity, servo_position;
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

float alpha, gamma, beta;

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

        bat = avc__read_battery_voltage();


        //Alpha, beta and gamma will be -1.0f to 1.0f
        left_intensity = avc__read_alpha();
        right_intensity = avc__read_gamma();
        servo_position = avc__read_beta();

        // Test mode enable
        if(GPIO_PinRead(GPIO3, TEST_SW_PIN) == 0)
        {   



            if(next_frame_ready)
            {
            	next_frame_ready=0;




            	eGFX_ImagePlane_Clear(&top_info);



                sprintf(pot_text, "a:%.1f", left_intensity);
                eGFX_DrawStringColored(&top_info,
            							pot_text,0,20,
										&FONT_10_14_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

                sprintf(pot_text, "b:%.1f", servo_position);
                eGFX_DrawStringColored(&top_info,
                						pot_text,135,20,
										&FONT_10_14_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

                sprintf(pot_text, "g:%.1f", right_intensity);
                eGFX_DrawStringColored(&top_info,
                						pot_text,255,20,
										&FONT_10_14_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));


                if(button__is_active(&left_btn))
                {
                    eGFX_DrawStringColored(&top_info,
            							"L_BTN",0,0,
										&FONT_10_14_1BPP,
										eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
                }

                if(button__is_active(&right_btn))
                {
                    eGFX_DrawStringColored(&top_info,
            							"R_BTN",290,0,
										&FONT_10_14_1BPP,
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
                    avc__set_motor_pwm(left_intensity, right_intensity);
                    avc__set_servo(servo_position);
                   	eGFX_DrawStringColored(&top_info,
                                 			"MOTORS ON",100,0,
                     						&FONT_10_14_1BPP,
                     						eGFX_COLOR_RGB888_TO_RGB565(0xFF,0xFF,0));
                 }
                else
                {
                	avc__set_servo(0);
                  	eGFX_DrawStringColored(&top_info,
                                			"MOTORS OFF",100,0,
                    						&FONT_10_14_1BPP,
                    						eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
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
