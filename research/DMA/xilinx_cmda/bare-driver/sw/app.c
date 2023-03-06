//==============================================================================
// Includes
//------------------------------------------------------------------------------

#include "xil_printf.h"
#include "xstatus.h"
#include "xparameters.h"
#include "cdma.h"

//==============================================================================
// Constants
//------------------------------------------------------------------------------

#define LEN         64


//==============================================================================
// Local Data Structures
//------------------------------------------------------------------------------

static CDMA_t Instance;

static u8 SrcBuffer[LEN] __attribute__((aligned (64)));
static u8 DstBuffer[LEN] __attribute__((aligned (64)));

//==============================================================================
// Main
//------------------------------------------------------------------------------

int main()
{
	xil_printf("CDMA App\r\n");
    xil_printf("****************\r\n");

    CDMA_t *InstancePtr = &Instance;

    // Initialize Driver

    int Status = CDMA_Init(XPAR_AXI_CDMA_0_DEVICE_ID, InstancePtr);
    if (Status != XST_SUCCESS) {
        xil_printf("CDMA_Init Failed!\r\n");
        return Status;
    }

    // Initialize Source and Destination Buffers

    for (u8 i = 0; i < LEN; i++) {
        SrcBuffer[i] = i;
        DstBuffer[i] = 0;
    }

    // Perform DMA Transfer

    Status = CDMA_Xfer_Simple_Poll( InstancePtr,
                                    PNTR_TO_UINT(SrcBuffer),
                                    PNTR_TO_UINT(DstBuffer),
                                    LEN
                                  );
    if (Status != XST_SUCCESS)  {
        xil_printf("CDMA_Xfer_Simple_Poll Failed!\r\n");
        return Status;
    }

    // Check Dst Buffer
    u32 Errors = 0;
    for (u8 i = 0; i < LEN; i++) {
        if (DstBuffer[i] != i) Errors++;
    }

    if (Errors) {
        xil_printf("Invalid data found at Dst Buffer\r\n");
        return XST_FAILURE;
    } else {
        xil_printf("Transfer Successful!\r\n");
    }

    return 0;
}
