#include "e.h"

#ifndef AVC__IO_H_
#define AVC__IO_H_

void avc_io__uart_init();

extern byte_queue_t UART4_TX_Q;
extern byte_queue_t UART4_RX_Q;


#endif /* AVC__IO_H_ */
