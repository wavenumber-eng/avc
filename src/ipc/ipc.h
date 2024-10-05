#include "stdint.h"

#ifndef __IPC_H
#define __IPC_H

typedef struct {

	volatile uint32_t core1_counter;

}ipc_t;


extern volatile ipc_t avc_ipc;

#endif
