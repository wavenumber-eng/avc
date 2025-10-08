

#include "avc__io.h"
#include "avc__line_processor.h"


volatile uint16_t bat;


float left_intensity;
float right_intensity;
float servo_position;

bool testmode__motors_enable = 0;

uint32_t line_to_process;

uint8_t pot_text[64];

volatile bool next_frame_ready;

float alpha, gamma, beta;

eGFX_ImagePlane top_info;

eGFX_ImagePlane camera_view;

hsl_t  line_hsl[CONFIG__CAMERA_RESOLUTION_X];

uint8_t black_filter[CONFIG__CAMERA_RESOLUTION_X];

uint8_t red_filter[CONFIG__CAMERA_RESOLUTION_X];

uint8_t top_info_buffer[eGFX_CALCULATE_16BPP_IMAGE_STORAGE_SPACE_SIZE(320,40)];

uint16_t *raw_camera_frame;

//Some variables for tracking execution time
uint32_t algorithm_timer;

uint32_t frame_timer;

bool pots_reset;

bool motors_on;

void avc__update_overlay();

/*
 * This is called by the camera interface when we get a new frame.
 * This executes in an interrupt context.
 */


void avc__next_frame(uint16_t *buf)
{
	next_frame_ready = true;

	raw_camera_frame = buf;

	//Update the image_plane structure for visualizing on the LCD;
	camera_view.Data = (uint8_t *)buf;

	frame_timer = CYCLE_COUNTER;

	//Reset the CPU Cycle counter to measure the next frame
	CYCLE_COUNTER = 0;

	algorithm_timer = 0;
}

int main(void)
{

	avc__init();

	//Initialize the ARM CPU cycle counter for measuring performance


	/*
	 * Initialize two image plane structures
	 *
	 * one for the graphic overlay at the top and the other for the camera data
	 */
	top_info.Data = top_info_buffer;
	top_info.SizeX = 320;
	top_info.SizeY = 40;
	top_info.Type = eGFX_IMAGE_PLANE_16BPP_RGB565;

	camera_view.SizeX = CONFIG__CAMERA_RESOLUTION_X;
	camera_view.SizeY = CONFIG__CAMERA_RESOLUTION_Y;
	camera_view.Type = eGFX_IMAGE_PLANE_16BPP_RGB565;



    while (1)
    {

        bat = avc__read_battery_voltage();
        //Alpha, beta and gamma will be -1.0f to 1.0f

        // Test mode enable
        if(GPIO_PinRead(GPIO3, TEST_SW_PIN) == 0)
        {   

            if(next_frame_ready)
            {
            	next_frame_ready=false;

            	   if(button__up(&center_btn))
            	   {
            		   pots_reset = true;

            	       if(testmode__motors_enable == 0)
            	       {
            	           testmode__motors_enable = 1;

            	       }

            	       else if(testmode__motors_enable == 1)
            	       {
            	           testmode__motors_enable = 0;
            	           avc__disable_motor_control();

            	       }
            	   }

            	   if(testmode__motors_enable == 1)
            	    {
            		   if(pots_reset == true)
            		   {
            			   motors_on = false;
            			   //make sure potentiometers are at midpoint before enabling motors
            			   if((avc__read_alpha() <= 0.55 && avc__read_alpha()> 0.45) &&
            			     (avc__read_beta() <= 0.55 && avc__read_beta()> 0.45) &&
							 (avc__read_gamma() <= 0.55 && avc__read_gamma()> 0.45))
            			   {
            				   pots_reset = false;
            				   avc__enable_motor_control();
            			   }
            		   }
            		   else
            		   {
            			   motors_on = true;
						   //rescale potentiometer 0-1.0f to -1.0f to 1.0f;
						   left_intensity = (avc__read_alpha() * 2.0f)-1.0;
						   right_intensity = (avc__read_gamma() * 2.0f)-1.0f;
						   servo_position = (avc__read_beta() * 2.0f)-1.0f;

						   avc__set_motor_pwm(left_intensity, right_intensity);
						   avc__set_servo(servo_position);
            		   }
            	    }
            	   else
            	   {
            		   //Use the alpha pot to select which line to process

            		   line_to_process = 50.0f + (avc__read_alpha()*150.0f);

            		   avc__set_servo(0);
            	   }

            	/*
            	 *raw_camera_frame is a pointer to 16-bit
            	 * frame data in RGB565 format.
            	 *  You can use it like an array
            	 *  raw_camera_frame[5] would be the 6th pixel on the 1st line
            	 *
            	 *  Since lines are 320 pixel long, you could do this:
            	 *
            	 *  raw_camera_frame [5 + 2*CONFIG__CAMERA_RESOLUTION_X];
            	 *
            	 *  This would get you pixel 6 on the 3rd line
            	 */

					uint32_t line = line_to_process;

					avc__convert_rgb565_to_hsl_struct(&raw_camera_frame[line*CONFIG__CAMERA_RESOLUTION_X],
													   line_hsl,
													   CONFIG__CAMERA_RESOLUTION_X);


					//filter to black and white using saturation and luminance
					for(int x=0;x<CONFIG__CAMERA_RESOLUTION_X;x++)
					{
							//threshold the luminance
							if(line_hsl[x].l > avc__read_beta())
							{
								//mark white as zero
								black_filter[x] = 0;

								if(line_hsl[x].s > .3)
								{
									if(line_hsl[x].h >330 || line_hsl[x].h <30)
									{
										red_filter[x] = 1;
									}
								}
								else
								{
									red_filter[x] = 0;
								}

							}
							else
							{
								//mark black as 1
								black_filter[x] = 1;
								red_filter[x]=0;
							}
					}



				for(int x=0;x<CONFIG__CAMERA_RESOLUTION_X;x++)
				{
					if(black_filter[x])
					{
						eGFX_DrawV_Line(&camera_view, line-2, line+2, x, eGFX_COLOR_RGB888_TO_RGB565(0,255,0));
					}
					else
					{
						eGFX_DrawV_Line(&camera_view ,  line-1, line+1, x , eGFX_COLOR_RGB888_TO_RGB565(255,255,255));

					}
				}


            	avc__update_overlay();


            	/*
            	 * Dump the overlay graphic to the LCD
            	 *
            	 */

            	eGFX_DumpRaw((uint8_t *)top_info.Data,
                					  320*40*2,
                					  0,
                					  319,
                					  0,
                					  39);

             	/*
                 * Dump the camera view data
                 *
                 */
            	eGFX_DumpRaw((uint8_t *)camera_view.Data,
            					  320*200*2,
            					  0,
            					  319,
            					  40,
            					  39+200);

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

void avc__update_overlay()
{
	eGFX_ImagePlane_Clear(&top_info);





   sprintf(pot_text, "a:%.2f", avc__read_alpha());

   eGFX_DrawStringColored(&top_info,
   						pot_text,0,20,
						&FONT_10_14_1BPP,
						eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

   sprintf(pot_text, "b:%.2f", avc__read_beta());
   eGFX_DrawStringColored(&top_info,
   						pot_text,120,20,
						&FONT_10_14_1BPP,
						eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));

   sprintf(pot_text, "g:%.2f", avc__read_gamma());
   eGFX_DrawStringColored(&top_info,
   						pot_text,255,20,
						&FONT_10_14_1BPP,
						eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));


   if(button__is_active(&left_btn))
   {
       eGFX_DrawStringColored(&camera_view,
   						"L_BTN",0,0,
						&FONT_10_14_1BPP,
						eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
   }

   if(button__is_active(&right_btn))
   {
       eGFX_DrawStringColored(&camera_view,
   						"R_BTN",290,0,
						&FONT_10_14_1BPP,
						eGFX_COLOR_RGB888_TO_RGB565(0,0xFF,0));
   }



   if(testmode__motors_enable == 1)
    {
	   if(motors_on)
	   {
      	eGFX_DrawStringColored(&top_info,
                    			"MOTORS ON",100,0,
        						&FONT_10_14_1BPP,
        						eGFX_COLOR_RGB888_TO_RGB565(0xFF,0,0));
	   }
	   else
	   {
	      	eGFX_DrawStringColored(&top_info,
	                    		"RESET POTS to 0.5",100,0,
	        					&FONT_10_14_1BPP,
	        					eGFX_COLOR_RGB888_TO_RGB565(0xFF,0xFF,0));
	   }
    }
   else
   {

     	eGFX_DrawStringColored(&top_info,
                   			"MOTORS OFF",100,0,
       						 &FONT_10_14_1BPP,
       						 eGFX_COLOR_RGB888_TO_RGB565(0,0x80,0));
   }


    //Use the CPU cycle counter to compute a % of CPU time per frame use for computations

	algorithm_timer = CYCLE_COUNTER;

    float cpu =  ((float)algorithm_timer / (float)frame_timer) * 100.0f;

	sprintf(pot_text, "CPU:%.1f%%", cpu) ;

	eGFX_DrawStringColored(&top_info,
	   						pot_text,3,3,
							&FONT_5_7_1BPP,
		 					eGFX_COLOR_RGB888_TO_RGB565(0,192,0));

}

