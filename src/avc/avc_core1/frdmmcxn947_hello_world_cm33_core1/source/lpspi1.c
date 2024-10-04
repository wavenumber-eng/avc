#include "lpspi1.h"
#include "fsl_debug_console.h"

#define MASTER_DMA_RX_CHANNEL   0U
#define MASTER_DMA_TX_CHANNEL   1U
#define DEMO_LPSPI_TRANSMIT_EDMA_CHANNEL      kDma0RequestMuxLpFlexcomm1Tx
#define DEMO_LPSPI_RECEIVE_EDMA_CHANNEL       kDma0RequestMuxLpFlexcomm1Rx

#define LPSPI_MASTER_CLK_FREQ CLOCK_GetLPFlexCommClkFreq(1u)
#define CONFIG__TRANSFER_BAUDRATE 50000000  //50000000 original
#define CONFIG__BITS_PER_FRAME  8


AT_NONCACHEABLE_SECTION_INIT(lpspi_master_edma_handle_t g_m_edma_handle) = {0};
edma_handle_t lpspiEdmaMasterRxRegToRxDataHandle;
edma_handle_t lpspiEdmaMasterTxDataToTxRegHandle;
edma_config_t dma_config = {0};
volatile bool isTransferCompleted  = true;
volatile bool mem_transfer_done = false;

edma_handle_t g_DMA_Handle;



void lpspi1_init(uint8_t transaction_bits)
{
    uint32_t srcClock_Hz;
    lpspi_master_config_t masterConfig;

    /*Master config*/
    LPSPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.bitsPerFrame = transaction_bits;
    masterConfig.baudRate = CONFIG__TRANSFER_BAUDRATE;
    masterConfig.whichPcs = kLPSPI_Pcs0;
    masterConfig.direction = kLPSPI_MsbFirst;
    masterConfig.pcsToSckDelayInNanoSec        = 0;
    masterConfig.lastSckToPcsDelayInNanoSec    = 0;
    masterConfig.betweenTransferDelayInNanoSec = 0;

    srcClock_Hz = LPSPI_MASTER_CLK_FREQ;
    LPSPI_MasterInit(LPSPI1, &masterConfig, srcClock_Hz);


    EDMA_GetDefaultConfig(&dma_config);
    EDMA_Init(DMA0, &dma_config);

    /*Set up lpspi master*/
    memset(&(lpspiEdmaMasterRxRegToRxDataHandle), 0, sizeof(lpspiEdmaMasterRxRegToRxDataHandle));
    memset(&(lpspiEdmaMasterTxDataToTxRegHandle), 0, sizeof(lpspiEdmaMasterTxDataToTxRegHandle));

    EDMA_CreateHandle(&(lpspiEdmaMasterRxRegToRxDataHandle), DMA0,
                      MASTER_DMA_RX_CHANNEL);
    EDMA_CreateHandle(&(lpspiEdmaMasterTxDataToTxRegHandle), DMA0,
                      MASTER_DMA_TX_CHANNEL);
    EDMA_CreateHandle(&g_DMA_Handle, DMA0, 2);

    EDMA_SetChannelMux(DMA0, MASTER_DMA_TX_CHANNEL,
                       DEMO_LPSPI_TRANSMIT_EDMA_CHANNEL);
    EDMA_SetChannelMux(DMA0, MASTER_DMA_RX_CHANNEL,
                       DEMO_LPSPI_RECEIVE_EDMA_CHANNEL);

    EDMA_SetCallback(&g_DMA_Handle, DMA_Callback, NULL);

    LPSPI_MasterTransferCreateHandleEDMA(LPSPI1, &g_m_edma_handle, LPSPI_MasterUserCallback,
                                         NULL, &lpspiEdmaMasterRxRegToRxDataHandle,
                                         &lpspiEdmaMasterTxDataToTxRegHandle);
    
    LPSPI_MasterTransferPrepareEDMALite(LPSPI1, &g_m_edma_handle, kLPSPI_Pcs0 | kLPSPI_MasterByteSwap | kLPSPI_MasterPcsContinuous);
}


void lpspi1_transfer_block(void *block, uint32_t block_size)
{
    lpspi_transfer_t masterXfer;

    while (!isTransferCompleted);

    masterXfer.txData   = block;
    masterXfer.rxData   = NULL;
    masterXfer.dataSize = block_size;
    masterXfer.configFlags = kLPSPI_MasterPcs0 | kLPSPI_MasterPcsContinuous | kLPSPI_MasterByteSwap;
    isTransferCompleted = false;

    //LPSPI_MasterTransferBlocking(LPSPI1, &masterXfer);
    LPSPI_MasterTransferEDMALite(LPSPI1, &g_m_edma_handle,&masterXfer);

}

void lpspi1_transfer_byte(uint8_t next_byte)
{
    lpspi1_transfer_block(&next_byte, 1);
}


void LPSPI_MasterUserCallback(LPSPI_Type *base, lpspi_master_edma_handle_t *handle, status_t status, void *userData)
{
    isTransferCompleted = true;
}





void dma_copy_buffer(void * src_buff, void * dest_buff, uint8_t data_width, uint32_t transfer_size)
{
    edma_transfer_config_t transferConfig;

    EDMA_PrepareTransfer(&transferConfig,   //GOOD
                            src_buff,       //GOOD
                            data_width,     //GOOD
                            dest_buff,      //GOOD
                            data_width,     //GOOD
                            data_width * transfer_size, 
                            data_width * transfer_size, 
                            kEDMA_MemoryToMemory);
    mem_transfer_done =false;

    EDMA_SubmitTransfer(&g_DMA_Handle, &transferConfig);
    EDMA_StartTransfer(&g_DMA_Handle);


//    while (mem_transfer_done != true);

}

/* User callback function for EDMA transfer. */
void DMA_Callback(edma_handle_t *handle, void *userData, bool transferDone, uint32_t tcds)
{
    if (transferDone)
    {
        mem_transfer_done = true;
    }
}