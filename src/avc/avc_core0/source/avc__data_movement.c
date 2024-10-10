#include "avc__data_movement.h"
#include "fsl_lpspi.h"
#include "fsl_lpspi_edma.h"

#define EXAMPLE_DMA_BASEADDR DMA1
#define DEMO_DMA_CHANNEL_0   0U

edma_config_t userConfig;
edma_handle_t datamovement_DMA_handle;
volatile bool data_movement_transfer_done = false;


void data_movement_callback(edma_handle_t *handle, void *userData, bool transferDone, uint32_t tcds)
{
    if (transferDone)
    {
        data_movement_transfer_done = true;
    }
}

void avc__data_movement_init()
{
    /* Configure EDMA channel for one shot transfer */
    EDMA_GetDefaultConfig(&userConfig);
    EDMA_Init(EXAMPLE_DMA_BASEADDR, &userConfig);

    EDMA_CreateHandle(&datamovement_DMA_handle, EXAMPLE_DMA_BASEADDR, DEMO_DMA_CHANNEL_0);
    EDMA_SetCallback(&datamovement_DMA_handle, data_movement_callback, NULL);
}


void avc__data_movement_copy(void * src_buff, void * dest_buff, uint8_t data_width, uint32_t transfer_size)
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
    data_movement_transfer_done =false;

    EDMA_SubmitTransfer(&datamovement_DMA_handle, &transferConfig);
    EDMA_StartTransfer(&datamovement_DMA_handle);


}