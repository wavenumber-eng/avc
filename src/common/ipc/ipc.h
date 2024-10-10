#include "stdint.h"
#include "eGFX_Driver_ER-TFT020-3.h"
#include "eGFX.h"

#ifndef __IPC_H
#define __IPC_H

#define CORE1_EXE_ADDRESS	0x4001000


typedef struct {

	volatile uint32_t core1_counter;
	volatile uint32_t core1_magic_boot_value;
	volatile eGFX_ImagePlane * image_plane_ptr;
	volatile uint32_t core1_cmd;
	volatile bool display_request;
}ipc_t;


typedef enum {
	CORE1_IDLE,
	CORE1_DUMP,
	CORE1_DOUBLE_AND_DUMP,
	CORE1_NUM_OF_CMDS
} core1_cmd_e;


//this value will be set in avc_ipc.core1_magic_boot_value once it is ready
#define CORE1__MAGIC_BOOT_VALUE 	0x12345689

extern volatile ipc_t avc_ipc;


#define CORE0__MAILBOX___CORE1_READY	(1<<0)


#endif
