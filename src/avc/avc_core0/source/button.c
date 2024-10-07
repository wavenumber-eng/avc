#include "button.h"
#include "fsl_gpio.h"
#include "MCXN947_cm33_core0.h"

typedef enum
{
    
    BUTTON_STATE_WAIT_FOR_PRESS                        =     0,
    BUTTON_STATE_WAIT_FOR_WAIT_FOR_PRESS_STABLE        =     1,
    BUTTON_STATE_WAIT_FOR_RELEASE                     =      2,
    BUTTON_STATE_WAIT_FOR_STABLE                       =     3,
    BUTTON_STATE_WAIT_FOR_STABLE_GENERATE_NO_CODES    =      4

} button_state_t;

#ifndef NULL
    #define NULL 0
#endif


void button__init(button_t *B,
                  uint8_t io_port,
                  uint8_t bit,
                  uint8_t polarity,
                  uint8_t debounce_time_ms
                  )
{
    if(B!=NULL)
    {

        B->io_port = io_port;
        B->io_bit = bit;
        B->polarity = polarity;
        B->debounce_time_ms = debounce_time_ms;
        B->down = 0;
        B->up = 0;
        B->hold_time = 0;

        B->state = BUTTON_STATE_WAIT_FOR_PRESS;
        B->debounce_timer = 0;
    }
}
 
uint32_t button__hal(button_t *B)
{
   uint32_t output;
   uint32_t p;
     
   output = button__hal_read_port_pin(B->io_port,B->io_bit); 

   if(B->polarity == BUTTON_POLARITY_LOW_ACTIVE)
    {
        if(output > 0)
        {
            p = BUTTON_NOT_PRESSED;
        }
        else
        {
            p = BUTTON_PRESSED;
        }
    }
    else
    {
        if(output == 0)
        {
            p = BUTTON_NOT_PRESSED;
        }
        else
        {
            p = BUTTON_PRESSED;
        }
    }

  return p;

}

void button__process(button_t *B, uint32_t process_time_ms)
{

    switch(B->state)
    {
        default:
        case BUTTON_STATE_WAIT_FOR_PRESS:

            if(button__hal(B) == BUTTON_PRESSED)
            {
                B->state = BUTTON_STATE_WAIT_FOR_WAIT_FOR_PRESS_STABLE;
                B->debounce_timer = 0;
            }

            break;

        case BUTTON_STATE_WAIT_FOR_WAIT_FOR_PRESS_STABLE:

            if(button__hal(B) == BUTTON_PRESSED)
            {
                B->debounce_timer += process_time_ms;

                if(B->debounce_timer > B->debounce_time_ms)
                {
                    B->state = BUTTON_STATE_WAIT_FOR_RELEASE;
                    B->hold_time = B->debounce_timer;
                    B->down = true;
                }
            }

            else
            {
                B->state = BUTTON_STATE_WAIT_FOR_PRESS;
            }

            break;

        case BUTTON_STATE_WAIT_FOR_RELEASE:

            if(button__hal(B) == BUTTON_PRESSED)
            {
                if(B->hold_time<0xFFFFFFFF)
                    B->hold_time += process_time_ms;
            }

            else
            {
            
                B->state = BUTTON_STATE_WAIT_FOR_STABLE;
                B->debounce_timer = 0;
            }

            break;

        case BUTTON_STATE_WAIT_FOR_STABLE:

            if(button__hal(B) == BUTTON_NOT_PRESSED)
            {

                B->debounce_timer+= process_time_ms;

                if(B->debounce_timer > B->debounce_time_ms)
                {
                    B->state = BUTTON_STATE_WAIT_FOR_PRESS;
                    B->up = true;

                }
            }

            else
            {
                B->debounce_timer = 0;
            }

            break;

        case BUTTON_STATE_WAIT_FOR_STABLE_GENERATE_NO_CODES:

                if(button__hal(B) == BUTTON_NOT_PRESSED)
                {
                    B->debounce_timer += process_time_ms;

                    if(B->debounce_timer > B->debounce_time_ms)
                    {
                        B->state = BUTTON_STATE_WAIT_FOR_PRESS;
                    }
                }
                else
                {
                    B->debounce_timer = 0;
                }
            break;


    }

}

uint32_t button__is_active(button_t *B)
{
    if(B->state == BUTTON_STATE_WAIT_FOR_RELEASE)
        return true;
    else
        return false;
}

uint32_t button__down(button_t *B)
{
    uint32_t P = 0;

    if(B->down == true)
    {
        P = true;
        B->down = false;
    }

    return P;
}

uint32_t button__up(button_t *B)
{
    uint32_t P = 0;

    if(B->up == true)
    {
        P = B->hold_time;
        B->up = false;
    }
   
    return P;
}

void button__programmatic_down(button_t *B)
{
    B->state = BUTTON_STATE_WAIT_FOR_RELEASE;
    B->hold_time = 0;
    B->down = true;

}

void button__programmatic_up(button_t *B)
{
    B->state = BUTTON_STATE_WAIT_FOR_PRESS;
    B->up = true;
}

uint32_t button__get_current_hold_time(button_t *B)
{
    uint32_t P = 0;

    if(B->state == BUTTON_STATE_WAIT_FOR_RELEASE)
    {
            P = B->hold_time;
    }

    return P;
}

void button__reset_state(button_t *B)
{
        B->state = BUTTON_STATE_WAIT_FOR_STABLE_GENERATE_NO_CODES;
        B->up = 0;
        B->down = 0;
        B->hold_time = 0;
}


// Probably a brute implementation, but not to modify a lot of code
uint32_t button__hal_read_port_pin(uint8_t io_port, uint8_t io_bit){

    if(io_port == 3)
        return  GPIO_PinRead(GPIO3, io_bit);

    return 0;
}
