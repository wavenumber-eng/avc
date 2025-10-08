#include "avc__adc.h"
#include "fsl_common.h"
#include "fsl_lpadc.h"
#include "fsl_vref.h"
#include "fsl_spc.h"
#include "fsl_debug_console.h"
#include "fsl_ctimer.h"
#include "fsl_inputmux.h"

// ******************* ADC0 required channels *******************
#define BATT_ADC_CHANNEL    0U
#define ALPHA_ADC_CHANNEL   14U
#define BETA_ADC_CHANNEL    10U
#define GAMMA_ADC_CHANNEL   13U

#define LPADC_VREF_SOURCE           kLPADC_ReferenceVoltageAlt3
#define VREF_BASE                   VREF0

#define ADC_MAX_VALUE   4095


// *************************** CTIMER ***************************
#define CTIMER          CTIMER0         /* Timer 0 */
#define CTIMER_MAT_OUT  kCTIMER_Match_3 /* Match output 0 */
#define CTIMER_EMT_OUT  (1u << kCTIMER_Match_3)
#define CTIMER_CLK_FREQ CLOCK_GetCTimerClkFreq(0U)



ctimer_config_t config;
ctimer_match_config_t matchConfig;
lpadc_conv_result_t mLpadcResultConfigStruct;

const uint32_t g_LpadcFullRange   = 4096U;
const uint32_t g_LpadcResultShift = 3U;

#define FILTER_SIZE 16
uint16_t adc_values [4][FILTER_SIZE];
uint32_t filter_idx[4];


void avc__adc_init()
{
    lpadc_config_t mLpadcConfigStruct;
    lpadc_conv_trigger_config_t mLpadcTriggerConfigStruct;
    lpadc_conv_command_config_t mLpadcCommandConfigStruct[4];
    vref_config_t vrefConfig;

    /* attach FRO HF to ADC0 */
    CLOCK_SetClkDiv(kCLOCK_DivAdc0Clk, 2U);
    CLOCK_AttachClk(kFRO_HF_to_ADC0);

    /* Use FRO HF clock for some of the Ctimers */
    CLOCK_SetClkDiv(kCLOCK_DivCtimer0Clk, 1u);
    CLOCK_AttachClk(kFRO_HF_to_CTIMER0);


     /* enable VREF */
    SPC_EnableActiveModeAnalogModules(SPC0, kSPC_controlVref);
    VREF_GetDefaultConfig(&vrefConfig);
    vrefConfig.bufferMode = kVREF_ModeBandgapOnly;
    VREF_Init(VREF_BASE, &vrefConfig);     // The VREF module is only used to supply the bias current for LPADC


    LPADC_GetDefaultConfig(&mLpadcConfigStruct);
    mLpadcConfigStruct.enableAnalogPreliminary = true;
    mLpadcConfigStruct.referenceVoltageSource = LPADC_VREF_SOURCE;
    mLpadcConfigStruct.conversionAverageMode = kLPADC_ConversionAverage1024;

    LPADC_Init(ADC0, &mLpadcConfigStruct);
    LPADC_DoOffsetCalibration(ADC0); /* Request offset calibration, automatic update OFSTRIM register. */
    LPADC_DoAutoCalibration(ADC0);


    /* Set conversion CMD configuration. */
    LPADC_GetDefaultConvCommandConfig(&mLpadcCommandConfigStruct[0]);
    LPADC_GetDefaultConvCommandConfig(&mLpadcCommandConfigStruct[1]);
    LPADC_GetDefaultConvCommandConfig(&mLpadcCommandConfigStruct[2]);
    LPADC_GetDefaultConvCommandConfig(&mLpadcCommandConfigStruct[3]);

    mLpadcCommandConfigStruct[BATT_ADC_CH].channelNumber = BATT_ADC_CHANNEL;
    mLpadcCommandConfigStruct[BATT_ADC_CH].chainedNextCommandNumber = 2;

    mLpadcCommandConfigStruct[ALPHA_ADC_CH].channelBNumber = ALPHA_ADC_CHANNEL;
    mLpadcCommandConfigStruct[ALPHA_ADC_CH].enableChannelB = 1;
    mLpadcCommandConfigStruct[ALPHA_ADC_CH].sampleChannelMode = kLPADC_SampleChannelSingleEndSideB;
    mLpadcCommandConfigStruct[ALPHA_ADC_CH].sampleTimeMode = kLPADC_SampleTimeADCK67;
    mLpadcCommandConfigStruct[ALPHA_ADC_CH].chainedNextCommandNumber = 3;

    mLpadcCommandConfigStruct[BETA_ADC_CH].channelNumber = BETA_ADC_CHANNEL;
    mLpadcCommandConfigStruct[BETA_ADC_CH].chainedNextCommandNumber = 4;

    mLpadcCommandConfigStruct[GAMMA_ADC_CH].channelNumber = GAMMA_ADC_CHANNEL;

    LPADC_SetConvCommandConfig(ADC0, 1, &mLpadcCommandConfigStruct[BATT_ADC_CH]);
    LPADC_SetConvCommandConfig(ADC0, 2, &mLpadcCommandConfigStruct[ALPHA_ADC_CH]);
    LPADC_SetConvCommandConfig(ADC0, 3, &mLpadcCommandConfigStruct[BETA_ADC_CH]);
    LPADC_SetConvCommandConfig(ADC0, 4, &mLpadcCommandConfigStruct[GAMMA_ADC_CH]);


    // Set trigger configuration. 
    LPADC_GetDefaultConvTriggerConfig(&mLpadcTriggerConfigStruct);
    mLpadcTriggerConfigStruct.targetCommandId       = 1;
    mLpadcTriggerConfigStruct.enableHardwareTrigger = 1;
    LPADC_SetConvTriggerConfig(ADC0, 0U, &mLpadcTriggerConfigStruct); 

    // ADC enable interrupts
    LPADC_EnableInterrupts(ADC0, kLPADC_FIFOWatermarkInterruptEnable);
    EnableIRQ(ADC0_IRQn);


    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(CTIMER, &config);

    matchConfig.enableCounterReset = true;
    matchConfig.enableCounterStop  = false;
    matchConfig.matchValue         = CTIMER_CLK_FREQ / 250 / 2;
    matchConfig.outControl         = kCTIMER_Output_Toggle;
    matchConfig.outPinInitState    = true;
    matchConfig.enableInterrupt    = false;
    CTIMER_SetupMatch(CTIMER, CTIMER_MAT_OUT, &matchConfig);
    CTIMER_StartTimer(CTIMER);

    INPUTMUX_Init(INPUTMUX);
    INPUTMUX_AttachSignal(INPUTMUX, 0, kINPUTMUX_Ctimer0M3ToAdc0Trigger);
}

#define ADC_NORMALIZE(x) (1.0f  - (((float)(x)/(float)ADC_MAX_VALUE)))


uint32_t filter_channel(uint32_t ch)
{
	uint32_t sum=0;
	for(int i=0;i<FILTER_SIZE;i++)
	{
		sum+=adc_values[ch][i];
	}

	return sum/FILTER_SIZE;
}
// Return a [0 to 1.0]
float avc__read_alpha()
{
    return ADC_NORMALIZE(filter_channel(ALPHA_ADC_CH));
}

// Return a [0 to 1.0]
float avc__read_beta()
{
	 return ADC_NORMALIZE(filter_channel(BETA_ADC_CH));
}

// Return a [0 to 1.0]
float avc__read_gamma()
{
	 return ADC_NORMALIZE(filter_channel(GAMMA_ADC_CH));
}

uint16_t avc__read_battery_voltage()
{
    uint16_t v_adc;
    uint16_t v_batt;

    v_adc = (filter_channel(BATT_ADC_CH)  *  330) / 4095;  // Adc voltage multiplied by 100
    v_batt = v_adc * 11;                                // Because of voltag divider

    return v_batt;
}


void ADC0_IRQHandler(void)
{
	uint32_t ch;
    if(LPADC_GetConvResult(ADC0, &mLpadcResultConfigStruct, 0))
    {
    	ch = mLpadcResultConfigStruct.commandIdSource - 1;
        adc_values[ch][filter_idx[ch]] = ((mLpadcResultConfigStruct.convValue) >> g_LpadcResultShift);
        filter_idx[ch]++;
        if(filter_idx[ch]>=FILTER_SIZE)
        	filter_idx[ch]=0;
    }

    SDK_ISR_EXIT_BARRIER;
}
