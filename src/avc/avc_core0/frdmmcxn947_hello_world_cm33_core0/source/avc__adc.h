#ifndef AVC__ADC_H_
#define AVC__ADC_H_

#include "inttypes.h"

typedef enum{
    BATT_ADC_CH,
    ALPHA_ADC_CH,
    BETTA_ADC_CH,
    GAMMA_ADC_CH,
    NUM_ADC_CH
} adc_channels_e;

extern void avc__adc_init();


extern uint8_t avc__read_alpha();
extern uint8_t avc__read_beta();
extern uint8_t avc__read_gamma();

extern uint16_t avc__read_battery_voltage();

#endif