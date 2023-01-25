# Hardware Abstraction Layer Generation - CMSIS

## IRQ Vector Entry

Hardware interrupt configuration is obtained after **diplomatic** resolution
during harware composition.

```c++

// Type Definition for IRQ Handlers
typedef void (* const IRQHandler )( void );

// Forward Declaration of IRQ Handler
void __attribute__((weak,alias("Default_Handler")))
AHADMA_IRQHandler(void);

// IRQ Vector Table
__attribute__((section(".isr_vectors"),used))
IRQHandler __isr_vectors[] = {
    // Cortex-M Core Handlers
    (IRQHandler) &__stack,              // initial stack pointer
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    NULL,                               // RESERVED
    NULL,                               // RESERVED
    NULL,                               // RESERVED
    NULL,                               // RESERVED
    SVC_Handler,
    DebugMon_Handler,
    NULL,                               // RESERVED
    PendSV_Handler,
    SysTick_Handler,

    // External Interrupts
    AHADMA_IRQHandler
}

// Default Handler
void __attribute__((section(".after_vectors")))
Default_Handler(void)
{
    #if defined(DEBUG)
    __DEBUG_BKPT();
    #endif

    while (1) {
        ;
    }
}

```

## AHADMA Interrupt Service Routine

```c++

// .. some includes (ignored)

// Helper Generated from RegSeq
static DMA_Status Helper_AHADMA_IRQHandler(AHADMAInst *InstancePtr)
{
    DMA_Status stat = AHADMA_GetStatCode(AHADMA_HAL_Regs.STAT.STATUS_CODE);
    // Clear Interrupt
    DMA_HAL_Regs.INTR.CLR = 1;
    return stat;
}

// AHADMA CMSIS IRQ Handler
void AHADMA_IRQHandler(void)
{
    NVIC_DisableIRQ(AHADMA_IRQn);   // AHADMA_IRQn auto-generated from diplomatic
                                    // resolution
    AHADMAInst *InstancePtr = SocTools_GetIrqDev(AHADMA_IRQn);
    if (InstancePtr) {
        DMA_Status stat = Helper_AHADMA_IRQHandler(InstancePtr);
        InstancePtr.Status = stat;
    }
    NVIC_EnableIRQ(AHADMA_IRQn);
    return;
}

```

## AHADMA Non-IRQ HAL

```c++

//
// DMA Transfer
// @param Src   The transfer source address
// @param Dest  The transfer destination address
// @param Size  The transfer size    
//   
//******************************************************************************
// Constraints:
// - Src    must be 8-byte aligned
// - Dest   must be 8-byte aligned
// - Size   must be < (1 << 30)
//******************************************************************************
DMA_Status SimpleTransfer(u32 Src, u32 Dest, u32 Size)
{
    // Requirements
    if (Src & 0x3)  return DMA_ALIGNMENT_ERROR;
    if (Src & 0x3)  return DMA_ALIGNMENT_ERROR;
    if (!(Size < (1 << 30))) return DMA_VALUE_RROR;

    // Transfer Setup
    AHADMA_HAL_Regs.SA.ADDR     = Src;  // alignment taken care of
    AHADMA_HAL_Regs.DA.ADDR     = Dest; // alignment taken care of
    AHADMA_HAL_Regs.CTRL.IE     = 1;
    AHADMA_HAL_Regs.LEN.SIZE    = Size;
    AHADMA_HAL_Regs.CTRL.GO     = 1;

    u32 token =  AHADMA_WaitForEvent();
    return AHADMA_GetStatCode(token);
}


```
