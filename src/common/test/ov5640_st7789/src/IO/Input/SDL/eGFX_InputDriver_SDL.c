#include "eGFX.h"
#include "eGFX_InputDriver_SDL.h"

eGFX_Button MyButtons[eGFX_NUM_BUTTONS];

eGFX_Encoder MyEncoders[eGFX_NUM_ENCODERS];

void eGFX_InputInit()
{
	 eGFX_InitButton( &MyButtons[WHEEL_BUTTON],
			 0,
			 0	,
			eGFX_BUTTON_POLARITY_LOW_ACTIVE,
			eGFX_BUTTON_DEFAULT_DEBOUNCE_TICKS,
	                "WHEEL_BUTTON");

	 eGFX_InitEncoder(&MyEncoders[0],
						 0,
						 1,
						 0,
						 2,
						 eGFX_BUTTON_POLARITY_LOW_ACTIVE,
						"WHEEL_ENCODER");

}


uint32_t eGFX_InputHAL_ReadPortPin(uint8_t Port, uint8_t Pin)
{
       
}

void eGFX_InputProcess(uint32_t mSecTicks)
{
        for(int i=0;i<eGFX_NUM_BUTTONS;i++)
        {
                eGFX_ProcessButton(&MyButtons[i]);
        }

        for(int i=0;i<eGFX_NUM_ENCODERS;i++)
        {
                eGFX_ProcessEncoder(&MyEncoders[i]);
        }
}

void eGFX_ClearAllInputActivity()
{
        for(int i=0;i<eGFX_NUM_BUTTONS;i++)
        {
                eGFX_Button_ResetState(&MyButtons[i]);
        }

}