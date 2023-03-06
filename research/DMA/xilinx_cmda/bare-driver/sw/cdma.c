//==============================================================================
// Includes
//------------------------------------------------------------------------------

#include "xil_types.h"
#include "xil_cache.h"
#include "xstatus.h"
#include "xaxicdma.h"
#include "cdma.h"

//==============================================================================
// Helpers
//==============================================================================

#define GCDMA_IS_ALIGNED(POINTER, BITS) \
    (!((UINTPTR)(const void *)(POINTER) & ((UINTPTR)((BITS)/8 - 1))))

//==============================================================================
// Functional API Implementation
//------------------------------------------------------------------------------

int CDMA_Init(u32 DeviceId, CDMA_t *InstancePtr)
{
    if (InstancePtr == NULL) return XST_FAILURE;

    XAxiCdma_Config *CfgPtr = XAxiCdma_LookupConfig(DeviceId);
    if (CfgPtr == NULL) return XST_FAILURE;

    InstancePtr->RegSpace       = (CDMA_r *) (CfgPtr->BaseAddress);
    InstancePtr->AddrWidth      = CfgPtr->AddrWidth;
    InstancePtr->DataWidth      = CfgPtr->DataWidth;
    InstancePtr->IsLite         = CfgPtr->IsLite;
    InstancePtr->HasSG          = TEST_BIT(InstancePtr->RegSpace->CDMASR, CDMASR_SGINCLD_Pos);
    InstancePtr->HasDRE         = CfgPtr->HasDRE;
    InstancePtr->BurstLen       = CfgPtr->BurstLen;
    InstancePtr->Callback       = NULL;

    return XST_SUCCESS;
}

int CDMA_Xfer_Simple_Poll(CDMA_t *InstancePtr, UINTPTR SrcAddr, UINTPTR DstAddr, u32 Len)
{
    // Check Initialization of Instance Data
    if (!InstancePtr || !InstancePtr->RegSpace) return XST_FAILURE;

    // Check Transfer Length
    if (Len > CDMA_MAX_BTT) return XST_FAILURE;

    // Check Source Address
    if (!InstancePtr->HasDRE && !GCDMA_IS_ALIGNED(SrcAddr, InstancePtr->DataWidth))
        return XST_FAILURE;

    // Check Destination Address
    if (!InstancePtr->HasDRE && !GCDMA_IS_ALIGNED(DstAddr, InstancePtr->DataWidth))
        return XST_FAILURE;

    // Flush Src and Dst Buffer
    Xil_DCacheFlushRange(PNTR_TO_UINT(SrcAddr), Len);
    Xil_DCacheFlushRange(PNTR_TO_UINT(DstAddr), Len);

    // Wait for IDLE
    while (!TEST_BIT(InstancePtr->RegSpace->CDMASR, CDMASR_IDLE_Pos)) {}

    // Set Source Address
    InstancePtr->RegSpace->SA = LOWER_32_BITS(SrcAddr);
    if (InstancePtr->AddrWidth > 32)
        InstancePtr->RegSpace->SA_MSB = UPPER_32_BITS(SrcAddr);

    // Set Destination Address
    InstancePtr->RegSpace->DA = LOWER_32_BITS(DstAddr);
    if (InstancePtr->AddrWidth > 32)
        InstancePtr->RegSpace->DA_MSB = UPPER_32_BITS(DstAddr);

    InstancePtr->RegSpace->BTT = Len;

    // Wait for Completion
    while (!TEST_BIT(InstancePtr->RegSpace->CDMASR, CDMASR_IDLE_Pos)) {}

    // Invalidate Cached Dst Buffer
    Xil_DCacheInvalidateRange(PNTR_TO_UINT(DstAddr), Len);

    // DONE
    return XST_SUCCESS;
}

int CDMA_SetCallback(CDMA_t *InstancePtr, CDMA_CallbackFn Callback)
{
    if (!InstancePtr) return XST_FAILURE;
    InstancePtr->Callback = Callback;
}

int CDMA_Xfer_Simple_Irq(CDMA_t *InstancePtr, UINTPTR SrcAddr, UINTPTR DstAddr, u32 Len);
{
    // Check Initialization of Instance Data
    if (!InstancePtr || !InstancePtr->RegSpace) return XST_FAILURE;

    // Check Transfer Length
    if (Len > CDMA_MAX_BTT) return XST_FAILURE;

    // Check Source Address
    if (!InstancePtr->HasDRE && !GCDMA_IS_ALIGNED(SrcAddr, InstancePtr->DataWidth))
        return XST_FAILURE;

    // Check Destination Address
    if (!InstancePtr->HasDRE && !GCDMA_IS_ALIGNED(DstAddr, InstancePtr->DataWidth))
        return XST_FAILURE;

    // Flush Src and Dst Buffer
    Xil_DCacheFlushRange(PNTR_TO_UINT(SrcAddr), Len);
    Xil_DCacheFlushRange(PNTR_TO_UINT(DstAddr), Len);

    // Invalidate Cached Dst Buffer
    Xil_DCacheInvalidateRange(PNTR_TO_UINT(DstAddr), Len);

    // Set Up Control Register
    InstancePtr->RegSpace->CDMACR = (1 << CDMACR_IRQTHRESHOLD_Pos) |
                                    (1 << CDMACR_ERR_IRQEN_Pos) |
                                    (1 << CDMACR_IOC_IRQEN_Pos);

    // Wait for IDLE
    while (!TEST_BIT(InstancePtr->RegSpace->CDMASR, CDMASR_IDLE_Pos)) {}

    // Set Source Address
    InstancePtr->RegSpace->SA = LOWER_32_BITS(SrcAddr);
    if (InstancePtr->AddrWidth > 32)
        InstancePtr->RegSpace->SA_MSB = UPPER_32_BITS(SrcAddr);

    // Set Destination Address
    InstancePtr->RegSpace->DA = LOWER_32_BITS(DstAddr);
    if (InstancePtr->AddrWidth > 32)
        InstancePtr->RegSpace->DA_MSB = UPPER_32_BITS(DstAddr);

    InstancePtr->RegSpace->BTT = Len;

    // DONE
    return XST_SUCCESS;
}
