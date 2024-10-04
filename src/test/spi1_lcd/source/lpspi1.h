#ifndef LPSPI1_H_
#define LPSPI1_H_

#include "stdint.h"
#include "fsl_lpspi.h"
#include "fsl_lpspi_edma.h"

extern void lpspi1_init(uint8_t transaction_bits);
extern void lpspi1_transfer_block(void *block, uint32_t block_size);
extern void lpspi1_transfer_byte(uint8_t next_byte);

extern void lpspi1_configure_16_bit_transfer();
extern void lpspi1_configure_8_bit_transfer();

void LPSPI_MasterUserCallback(LPSPI_Type *base, lpspi_master_edma_handle_t *handle, status_t status, void *userData);


#endif
