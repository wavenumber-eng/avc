#include "stdint.h"

#ifndef __IPC_H
#define __IPC_H

#define CORE1_EXE_ADDRESS	0x4001000


typedef struct {

	volatile uint32_t core1_counter;
	volatile uint32_t core1_magic_boot_value;
	volatile uint8_t * image_ptr;
	volatile uint32_t core1_cmd;

}ipc_t;


//this value will be set in avc_ipc.core1_magic_boot_value once it is ready
#define CORE1__MAGIC_BOOT_VALUE 	0x12345689

extern volatile ipc_t avc_ipc;


#define CORE0__MAILBOX___CORE1_READY	(1<<0)


#endif
