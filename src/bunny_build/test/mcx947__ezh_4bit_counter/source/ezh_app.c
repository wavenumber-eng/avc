#include <ezh_app.h>
#include <stdbool.h>
#include <stdint.h>

#include "../../../src/bunny_build.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MCX_EZH_ARCH_B_CON_BASE 0x40033020
#define ARM2EZH      		(MCX_EZH_ARCH_B_CON_BASE   + 0x20)
#define EZH2ARM    			(MCX_EZH_ARCH_B_CON_BASE   + 0x24)

#define EZHBREAKADDR 		(MCX_EZH_ARCH_B_CON_BASE   + 0x10)
#define EZHBREAKVECT 		(MCX_EZH_ARCH_B_CON_BASE   + 0x14)
#define MCX_EZH_ARCH_B0     ((EZH_ARCH_B_CON_Type *)     MCX_EZH_ARCH_B_CON_BASE)  

#define EZH_CTR_b0      		 22         	/* GPIO22  us PIO2_2 on MCX */
#define EZH_CTR_b1      		 23         	/* GPIO23  us PIO2_3 on MCX */
#define EZH_CTR_b2      		 24         	/* GPIO24  us PIO2_4 on MCX */
#define EZH_CTR_b3      		 25         	/* GPIO25  us PIO2_5 on MCX */
#define EZH_FRQ_SEL              27             /* GPIO27  us PIO2_7 on MCX */

uint32_t __attribute__ ((section("for_ezh_ram"))) my_ezh_program[1024];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Code
 ******************************************************************************/

void ezh_4_bit_counter_app (void){
    E_NOP();
    E_PER_READ(R6, ARM2EZH); //Peripheral Read
    E_LSR(R6, R6, 2);        //remove bit 0:1
    E_LSL(R6, R6, 2);        //bit 0:1 now = 0
    E_LDR(SP, R6, 0);        //load stack pointer para.coprocessor_stack
	E_LDR(R7, R6, 1);        //R7 -> para.p_buffer

    //counter pins direction
	E_BSET_IMM(GPD, GPD, EZH_CTR_b0);
	E_BSET_IMM(GPD, GPD, EZH_CTR_b1);
    E_BSET_IMM(GPD, GPD, EZH_CTR_b2);
	E_BSET_IMM(GPD, GPD, EZH_CTR_b3);
    E_BCLR_IMM(GPD, GPD, EZH_FRQ_SEL);

    E_BSET_IMM(GPO, GPO, EZH_CTR_b0);
	E_BSET_IMM(GPO, GPO, EZH_CTR_b1);
    E_BSET_IMM(GPO, GPO, EZH_CTR_b2);
	E_BSET_IMM(GPO, GPO, EZH_CTR_b3);

    E_HEART_RYTHM_IMM(37493);

    E_LOAD_IMM(R0, 0);  // R0 has the counter value

E_LABEL("COUNTER_INC_LOOP");
    E_ADD_IMM(R0, R0, 1);               // R0 = R0 + 1
    E_AND_IMM(R0, R0, 0XF);             // R0 = R0 % 0b1111

    E_LSL(GPO, R0, 22);                 // Set output bits of counter

    E_BTST_IMMS(R1, GPI, EZH_FRQ_SEL);  //if(FRQ_SEL == 1) ctr_freq = 8000, else ctr_freq = 4000
    E_COND_ADD(PO, PC, PC, 4);
    E_HEART_RYTHM_IMM(37500);           // ctr_freq = 8000
    E_HEART_RYTHM_IMM(18750);           // ctr_freq = 4000

    E_WAIT_FOR_BEAT();
    E_GOSUB("COUNTER_INC_LOOP");
}


void EZH_cfgHandshake(bool _enable_handshake, bool _enable_event){
	int enable_handshake;
	int enable_event;
	enable_handshake = (_enable_handshake) ? 1 : 0;
	enable_event     = (_enable_event)     ? 1 : 0;
	MCX_EZH_ARCH_B0->EZHB_ARM2EZH |= (enable_handshake<<EZHB_HANDSHAKE_ENABLE) + (enable_event<<EZHB_HANDSHAKE_EVENT);
};

void EZH_Init(void *pPara){

    bunny_build(&my_ezh_program[0],
    	                sizeof(my_ezh_program),
						ezh_4_bit_counter_app
    	                );

	MCX_EZH_ARCH_B0->EZHB_CTRL |= (0xC0DE0000 | (1<<EZHB_ENABLE_GPISYNCH));
    MCX_EZH_ARCH_B0->EZHB_ARM2EZH = (uint32_t)pPara;
	EZH_cfgHandshake(true,false);
}

void EZH_boot(void * pProgram) {
	MCX_EZH_ARCH_B0->EZHB_BOOT = (uint32_t) pProgram;
	MCX_EZH_ARCH_B0->EZHB_CTRL = 0xC0DE0011 | (0<<EZHB_MASK_RESP) |(0<<EZHB_ENABLE_AHBBUF) ; // BOOT
};

void EZH_Start(void){
	EZH_boot(my_ezh_program);
}


void EZH_SetExternalFlag(uint8_t flag)
{
    volatile uint32_t ezh_ctrl = (MCX_EZH_ARCH_B0->EZHB_CTRL & 0x0000FFFF);
    if (flag == 0) {
        ezh_ctrl &= ~(1 << EZHB_EXTERNAL_FLAG);
    } else {
        ezh_ctrl |= (1 << EZHB_EXTERNAL_FLAG);
    }
    MCX_EZH_ARCH_B0->EZHB_CTRL = (0xC0DE0000 | ezh_ctrl);
}
