
#include "stdint.h"
#include "stdbool.h"

#ifndef __BUTTON_H
#define __BUTTON_H


#define BUTTON_POLARITY_LOW_ACTIVE                          0
#define BUTTON_POLARITY_HIGH_ACTIVE                         1

#define BUTTON_PRESSED                                      1
#define BUTTON_NOT_PRESSED                                  0


typedef struct
{
    volatile bool down;   				    //Flag to indicate button was pressed down
    volatile bool up;       				//Flag to indicate button was release
    uint8_t io_port;       					//GPIO port of the button
    uint8_t io_bit;        					//GPIO bit of the button
    volatile uint32_t   hold_time;          //Number of ticks the butt was pressed for
    uint32_t  debounce_time_ms;            //Number of ticks for the debounce time
    volatile uint32_t debounce_timer;      //used internally for the debouncer alogirhtm
    volatile uint8_t state;                 //State of the debounce algorithm
    uint8_t  polarity;                      //is the button active high (>0) or active low (0)
} button_t;

//This is the HAL for the button.  It must be implemented by the user
//Needs to read the port/bit and return 1 (pin high) or 0 (pin low)
uint32_t button__hal_read_port_pin(uint8_t io_port, uint8_t io_bit); 

void button__init(button_t *B,
                 uint8_t io_port,
                 uint8_t io_bit,
                 uint8_t Polarity,
                 uint8_t debounce_time_ms
                 );

// Computes the Debounce algorithm. Must be called periodically
// process time is the interval in milliseconds between calls to this function
void button__process(button_t *B , uint32_t process_time_ms);

//Indicates that the button is currently being help down
uint32_t button__is_active(button_t *B);

//Reads the button down flag.  Flag will be cleared after read
uint32_t button__down(button_t *B);

//Reads the button up flag.  Flag will be cleared after read
//Returns the number of ticks the button was held for
uint32_t button__up(button_t *B);

//Set the button down flag.  This is used for programmatic button presses
void button__programmatic_down(button_t *B);

//Set the button up flag.  This is used for programmatic button presses
void button__programmatic_up(button_t *B);

//returns the current hold time from the debounce algorithm
uint32_t button__get_current_hold_time(button_t *B);

//resets the button state and waits for a release (button up but generates no up or down code)
void button__reset_state (button_t *B);

//Call periodically to process a button
void Button_Process(button_t *B);

//return to get the instantaneous state of the button through the hal layer
uint32_t button__hal(button_t *B);

#endif
