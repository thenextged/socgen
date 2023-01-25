# AHA Onyx DMA Hardware Abstraction Layer (HAL) API

## Supported Features
- Interrupt
- AXI-4 (INCR mode only)
- 1-D Transfers (src addr, dst addr, length)
- APB and AXI-Lite protocol for register file access

## Data Structures

```c++

struct AHASOC_DMA_TypeDef {
    uint32_t    CTRL;           // Control Register (RW)
    uint32_t    STAT;           // Status Register (RO)
    uint32_t    INT_CLR;        // Interrupt Clear Register (WO)
    uint32_t    SRC;            // Source Address Register
    uint32_t    DST;            // Destination Address Register    
    uint32_t    LEN;            // Transfer Length Register
    uint32_t    ID;             // Peripheral ID Register
}
```

## Low-Level APIs

```c++

//
// Enable DMA Interrupts
//
// @param DmaHandle     : the DMA object
// @param ie_value      : 1 -> Enable, 0 -> Disable
//
void AHASOC_dma_SetIE(  AHASOC_DMA_TypeDef *    DmaHandle,
                        uint8_t                 ie_value
                    )

//
// Get DMA Interrupt Enable Status
//
// @param DmaHandle     : the DMA object
// @return DMA enable status
//
uint8_t AHAOSC_dma_GetIE(   AHASOC_DMA_TypeDef*     DmaHandle)

//
// Start DMA Transfer
//
// @param DmaHandle     : the DMA object
//
void AHASOC_dma_Start(  AHASOC_DMA_TypeDef *        DmaHandle)

//
// Set Source Address
//
// @param DmaHandle     : the DMA object
// @param src_addr      : the transaction source address
//
void AHASOC_dma_SetSrcAddr( AHASOC_DMA_TypeDef *    DmaHandle,
                            uint32_t                src_addr
                        )

//
// Get Source Address
//
// @param DmaHandle     : the DMA object
//
void AHASOC_dma_GetSrcAddr( AHASOC_DMA_TypeDef *    DmaHandle)

//
// Set Destination Address
//
// @param DmaHandle     : the DMA object
// @param dst_addr      : the transaction destination address
//
void AHASOC_dma_SetDstAddr( AHASOC_DMA_TypeDef *    DmaHandle,
                            uint32_t                dst_addr
                        )

//
// Get Destination Address
//
// @param DmaHandle     : the DMA object
//
void AHASOC_dma_GetDstAddr( AHASOC_DMA_TypeDef *    DmaHandle)

//
// Set Transfer Length
//
// @param DmaHandle     : the DMA object
// @param len           : the length of the transaction in bytes
//
void AHASOC_dma_SetLength( AHASOC_DMA_TypeDef *    DmaHandle,
                            uint32_t                len
                        )

//
// Get Transfer Length
//
// @param DmaHandle     : the DMA object
//
void AHASOC_dma_GetLength( AHASOC_DMA_TypeDef *    DmaHandle)

//
// Get DMA Device ID
//
// @param DmaHandle     : the DMA object
//
void AHASOC_dma_GetID( AHASOC_DMA_TypeDef *    DmaHandle)

//
// Clear Interrupt
//
// @param DmaHandle     : the DMA object
//
void AHASOC_dma_ClearIrq( AHASOC_DMA_TypeDef *    DmaHandle)

//
// Get Busy Status
//
// @param DmaHandle     : the DMA object
// @return              : 1 -> Busy, 0 -> Idle
//
uint8_t AHASOC_dma_GetBusy( AHASOC_DMA_TypeDef *    DmaHandle)

//
// Get Interrupt Status
//
// @param DmaHandle     : the DMA object
// @return              : 1 -> IRQ occurred, 0 -> no IRQ occurred
//
uint8_t AHASOC_dma_GetIrqStatus( AHASOC_DMA_TypeDef *    DmaHandle)

//
// Get Value of Status Register
//
// @param DmaHandle     : the DMA object
//
uint8_t AHASOC_dma_GetStatus( AHASOC_DMA_TypeDef *    DmaHandle)
```

## High-Level APIs

```c++

//
// Transfer Data using DMA
//
void StartDMA(  AHASOC_DMA_TypeDef *        DmaHandle,
                uint64_t                    DstAddr,
                uint64_t                    SrcAddr,
                uint32_t                    Length
            )
{
    // Set Dst Addr
    AHASOC_dma_SetDstAddr( DmaHandle, (uint32_t)(DstAddr));

    // Set Source Address
    AHASOC_dma_SetSrcAddr( DmaHandle, (uint32_t)(SrcAddr));

    // Set Transfer Length - in bytes
    AHASOC_dma_SetLength( DmaHandle, Length);

    // Enable DMA Interrupts at System-Level
    NVIC_ClearPendingIRQ( GetIrqNum(DmaHandle) );
    NVIC_EnableIRQ( GetIrqNum(DmaHandle) );

    // Enable DMA Interrupts at Device-Level
    AHASOC_dma_SetIE( DmaHandle, 1 );

    // Start DMA Transfer
    AHASOC_dma_Start( DmaHandle );

}

//
// Wait for DMA Transfer Completion
//
//
// Note:
// - device-level IRQ servicing is performed in an ISR
//
void WaitDMA(  AHASOC_DMA_TypeDef *        DmaHandle )
{
    // Wait for DMA Interrupt
    while (dma_irq_counters[GetDMAInstanceId(DmaHandle)] < 1);
    // Clear DMA Interrupt Counter
    dma_irq_counters[GetDMAInstanceId(DmaHandle)] = 0;
    // Clear DMA Interrupt at System-Level
    NVIC_DisableIRQ( GetIrqNum(DmaHandle) );
    NVIC_EnableIRQ( GetIrqNum(DmaHandle) );
}

//
// DMA IRQ ISR
//
void DMA_Handler(void)
{
    // Clear Device-Level Interrupt
    AHASOC_dma_ClearIrq( GetCurrentDMAInstance() );

    // Clear Interrupt Counter
    dma_irq_counters[GetDMAInstanceId(GetCurrentDMAInstance())] = 1;
}


```
