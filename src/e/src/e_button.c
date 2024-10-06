#include "e_button.h"

typedef enum
{
    
    BUTTON_STATE__WAIT_FOR_PRESS                        =     0,
    BUTTON_STATE__WAIT_FOR_WAIT_FOR_PRESS_STABLE        =     1,
    BUTTON_STATE__WAIT_FOR_RELEASE                     =      2,
    BUTTON_STATE__WAIT_FOR_STABLE                       =     3,
    BUTTON_STATE__WAIT_FOR_STABLE_GENERATE_NO_CODES    =      4

} e_button_state_t;

#ifndef NULL
    #define NULL 0
#endif

void e_button__init(e_button_t *button,
                  uint8_t io_port,
                  uint8_t bit,
                  uint8_t polarity,
                  uint8_t debounce_time_ms
                  )
{
    if(button!=NULL)
    {

        button->io_port = io_port;
        button->io_bit = bit;
        button->polarity = polarity;
        button->debounce_time_ms = debounce_time_ms;
        button->down = 0;
        button->up = 0;
        button->hold_time = 0;

        button->state = BUTTON_STATE__WAIT_FOR_PRESS;
        button->debounce_timer = 0;
    }
}
 
uint32_t e_button__hal(e_button_t *button)
{
   uint32_t output;
   uint32_t p;
     
   output = e_button__hal_read_port_pin(button->io_port,button->io_bit); 

   if(button->polarity == E_BUTTON__POLARITY_LOW_ACTIVE)
    {
        if(output > 0)
        {
            p = E_BUTTON__NOT_PRESSED;
        }
        else
        {
            p = E_BUTTON__PRESSED;
        }
    }
    else
    {
        if(output == 0)
        {
            p = E_BUTTON__NOT_PRESSED;
        }
        else
        {
            p = E_BUTTON__PRESSED;
        }
    }

  return p;

}

void e_button__crunch(e_button_t *button, uint32_t process_time_ms)
{

    switch(button->state)
    {
        default:
        case BUTTON_STATE__WAIT_FOR_PRESS:

            if(e_button__hal(button) == E_BUTTON__PRESSED)
            {
                button->state = BUTTON_STATE__WAIT_FOR_WAIT_FOR_PRESS_STABLE;
                button->debounce_timer = 0;
            }

            break;

        case BUTTON_STATE__WAIT_FOR_WAIT_FOR_PRESS_STABLE:

            if(e_button__hal(button) == E_BUTTON__PRESSED)
            {
                button->debounce_timer += process_time_ms;

                if(button->debounce_timer > button->debounce_time_ms)
                {
                    button->state = BUTTON_STATE__WAIT_FOR_RELEASE;
                    button->hold_time = button->debounce_timer;
                    button->down = true;
                }
            }

            else
            {
                button->state = BUTTON_STATE__WAIT_FOR_PRESS;
            }

            break;

        case BUTTON_STATE__WAIT_FOR_RELEASE:

            if(e_button__hal(button) == E_BUTTON__PRESSED)
            {
                if(button->hold_time<0xFFFFFFFF)
                    button->hold_time += process_time_ms;
            }

            else
            {
                button->state = BUTTON_STATE__WAIT_FOR_STABLE;
                button->debounce_timer = 0;
            }

            break;

        case BUTTON_STATE__WAIT_FOR_STABLE:

            if(e_button__hal(button) == E_BUTTON__NOT_PRESSED)
            {

                button->debounce_timer+= process_time_ms;

                if(button->debounce_timer > button->debounce_time_ms)
                {
                    button->state = BUTTON_STATE__WAIT_FOR_PRESS;
                    button->up = true;

                }
            }

            else
            {
                button->debounce_timer = 0;
            }

            break;

        case BUTTON_STATE__WAIT_FOR_STABLE_GENERATE_NO_CODES:

                if(e_button__hal(button) == E_BUTTON__NOT_PRESSED)
                {
                    button->debounce_timer += process_time_ms;

                    if(button->debounce_timer > button->debounce_time_ms)
                    {
                        button->state = BUTTON_STATE__WAIT_FOR_PRESS;
                    }
                }
                else
                {
                    button->debounce_timer = 0;
                }
            break;


    }

}

uint32_t e_button__is_active(e_button_t *button)
{
    if(button->state == BUTTON_STATE__WAIT_FOR_RELEASE)
        return true;
    else
        return false;
}

uint32_t e_button__down(e_button_t *button)
{
    uint32_t p = 0;

    if(button->down == true)
    {
        p = true;
        button->down = false;
    }

    return p;
}

uint32_t e_button__up(e_button_t *B)
{
    uint32_t p = 0;

    if(B->up == true)
    {
        p = B->hold_time;
        B->up = false;
    }
   
    return p;
}

void e_button__programmatic_down(e_button_t *button)
{
    button->state = BUTTON_STATE__WAIT_FOR_RELEASE;
    button->hold_time = 0;
    button->down = true;

}

void e_button__programmatic_up(e_button_t *p)
{
    p->state = BUTTON_STATE__WAIT_FOR_PRESS;
    p->up = true;
}

uint32_t e_button__get_current_hold_time(e_button_t *button)
{
    uint32_t p = 0;

    if(button->state == BUTTON_STATE__WAIT_FOR_RELEASE)
    {
            p = button->hold_time;
    }

    return p;
}

void e_button__reset_state(e_button_t *button)
{
        button->state = BUTTON_STATE__WAIT_FOR_STABLE_GENERATE_NO_CODES;
        button->up = 0;
        button->down = 0;
        button->hold_time = 0;
}



