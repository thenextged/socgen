# Xilinx CDMA - Programming Guide

## Register Space

| Offset | Register |
|--------|----------|
| 0x00      | DMACR |
| 0x04      | DMASR |
| 0x08      | CURDESC (LSB) |
| 0x0C      | CURDESC (MSB) |
| 0x10      | TAILDESC (LSB) |
| 0x14      | TAILDESC (MSB) |
| 0x18      | SA (LSB)      |
| 0x1C      | SA (MSB)      |
| 0x20      | DA (LSB)      |
| 0x24      | DA (MSB)      |
| 0x28      | BTT           |

```c++

// CDMA Struct

#ifdef CDMA_64_BIT

typedef struct CDMA_t {
    u32 DMACR;
    u32 DMASR;
    u64 CURDESC;
    u64 TAILDESC;
    u64 SA;
    u64 DA;
    u32 BTT;
} CDMA_t;

#else

typedef struct CDMA_t {
    u32 DMACR;
    u32 DMASR;
    u32 CURDESC;
    u32 TAILDESC;
    u32 SA;
    u32 DA;
    u32 BTT;
} CDMA_t;

// CDMA Transfer (Simple, No IRQ) 
int CDMA_Xfer_Simple_Poll(CDMA_t *InstancePtr, u64 SrcAddr, u64 DestAddr, u32 Len)
{
    if (!InstancePtr) return XST_FAILURE;

    InstancePtr->DMACR = 0;
    while (! (InstancePtr->DMASR & (1 << 1))) {}

    #ifdef CDMA_64_BIT

    InstancePtr->SA     = SrcAddr;
    InstancePtr->DA     = DestAddr;
 
    #else 

    InstancePtr->SA     = (u32) SrcAddr;
    InstancePtr->DA     = (u32) DestAddr;

    #endif /* CDMA_64_BIT */

    InstancePtr->BTT    = Len;
    while (! (InstancePtr->DMASR & (1 << 1))) {}
}

#endif /* CMA_64_BIT */

```