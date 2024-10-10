#ifndef AVC__DATA_MOVEMENT_H_
#define AVC__DATA_MOVEMENT_H_

#include "stdint.h"
#include "stdbool.h"

extern volatile bool data_movement_transfer_done;

extern void avc__data_movement_init();
extern void avc__data_movement_copy(void * src_buff, void * dest_buff, uint8_t data_width, uint32_t transfer_size);


#endif