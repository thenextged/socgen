# Using RegSeq to Automate Software Development

## Introduction

SoC design is at least a time-consuming effort both in terms of hardware design and software development. The goal of our work is to alleviate the SoC design task by:
1. exploiting the synergistic relationship between hardware construction and software design
2. allowing faster software development cycles

The success of our goals is primarily based on the development of powerful abstractions

## Example Setup

To demonstrate our core concept, we will use an example system consisting of:
- a DMA IP
- an interrupt controller
- a clock and reset controller
- a CORTEX-M processor
- SRAM blocks
- an interconnect

The goal of the experiment will be to show how we can:
- capture abstractions for various IP types/classes
- provide concrete implementations of the abstractions
- generate software collateral for a target environment

## Hardware Composition

```c++

//==============================================================================
// Example SoC Hardware Composition
//
// The syntax and hardware abstractions used in this hardware composition is
// completely made up for the simple purpose of providing a means to express our
// intent at the hardware composition stage
//------------------------------------------------------------------------------

#include "hw_spec.h"

int main(int argc, char *argv[])
{

    //
    // Diplomatic Node Declarations
    //

    System SoC("SoC");

    Node DMA("DMA");
    Node ClockResetCtrl("ClockResetCtrl");
    Node CPU("CPU");
    Node IrqCtrl("IRQ_CTRL");
    Node SRAM("SRAM");
    Node Interc("Interconnect");

    //
    // Diplomatic Node Configurations
    //

    // DMA
    DMA.AddIF<AXI4>("SG", Role::Master);
    DMA.AddIF<AXI4>("Data", Role::Master);
    DMA.AddIF<AXILITE>("Reg", Role::Slave);
    DMA.AddIF<IRQ>("Irq", Role::Master);
    DMA.AddIF<Clock>("Clock", Role::Slave, 500);
    DMA.AddIF<Reset>("Reset", Role::Slave, Polarity::Negative);

    DMA.GetIF<Reset>("Reset").AssociatedClock("Clock");


    // SRAM
    SRAM.AddIF<AXI4>("Data", Role::Slave);
    SRAM.AddIF<Clock>("Clock", Role::Slave);
    SRAM.AddIF<Reset>("Reset", Role::Slave, Polarity::Negative);
    SRAM.AddMemBlock(64);   // 64K

    // IRQ Controller
    IrqCtrl.AddIF<AXILITE>("Reg", Role::Slave);
    IrqCtrl.AddIF<IRQ>("SrcIrq", Role::Slave);
    IrqCtrl.AddIF<IRQ>("CombinedIrq", Role::Master);


    //
    // Diplomatic Node Connections
    //

    Interc.Connect(CPU);
    Interc.Connect(DMA.GetIF("SG"));
    Interc.Connect(DMA.GetIF("Data"));
    Interc.Connect(DMA.GetIF("Reg"));
    Interc.Connect(SRAM.GetIF("Data"));
    Interc.Connect(IrqCtrl.GetIF("Reg"));

    ClockResetCtrl.Connect(DMA.GetIF("Clock"), DMA.GetIF("Reset"));

    IrqCtrl.Connect(DMA.GetIF("Irq"));

    CPU.Connect(IrqCtrl.GetIF("CombinedIrq"));

    SoC.GenHAL("Onyx");

    return 0;
}

```

## IP Abstract Sequences

In order to define abstract sequences, we need to first create abstractions for the individual IP classes. In this example, we have the following classes of IP:
- Data mover
- Interrupt controller
- CPU

### DMA Abstraction

In this example, we will use the following abstraction for a DMA device:
A DMA:
- has one or multiple channels
- a channel can perform either a READ or WRITE transaction at a time
- a channel can generate interrupts to signal events (STARTED, STOPPED, COMPLETED, ERROR)
- a transaction can be described as
    - simple: start address, destination address, length
    - scatter-gather: multiple simple transaction descriptions in a list

## IP Concrete Sequences

#### DMA

```c++

class DMA : public RegSeq, public HasIRQ {

    Resource<DMAReg> iDMAReg;
    Resource<DMAIrq> iDMAIrq;
    Resource<DMAInstance> iDMAInst;

public:
    //
    // Resource-Binding with Constructor
    //
    DMA(const std::string &name,
        const Resource<DMAReg> &reg,
        const Resource<DMAIrq> &irq,
        const Resource<DMAInst> &inst) :    RegSeq(name),
                                            iDMAReg(reg),
                                            iDMAIrq(irq),
                                            iDMAInst(inst)
    {}

    //
    // DMA Transfer in Simple Mode.
    // In simple mode, scatter-gather lists are not used
    //
    __SEQ__ DMA_Status SimpleTransfer(MemObj Src, MemObj Dest, u32 Size)
    {
        // Requirements
        Req(Src.Alignment == 2);
        Req(Dst.Alignment == 2);
        Req(Size < (1 << 30));

        // Transfer Setup
        iDMAReg.SA          = Src.GetAddress();
        iDMAReg.DA          = Dest.GetAddress();
        iDMAReg.CTRL.IE     = 1;
        iDMAReg.BTT         = Size;
        auto token          = iDMAIrq.WaitForEvent();
        return token.GetStatusCode();
    }

    //
    // Target-Independent Interrupt Service Routine.
    // IRQHandlers must:
    // - detect which event has occured
    // - return a status code
    //
    __SEQ__ DMA_Status IRQHandler() override {
        DMA_Status stat = DMA_OK;
        if (iDMAReg.STATUS.ERR) stat = DMA_ERROR;
        else if(!iDMAReg.STATUS.DONE) stat = DMA_ABORT;
        iDMAReg.STATUS = 1; // clear interrupt
        return stat;
    }
}

```

## Software Generation for Target Environment

For this example, we will use a `CMSIS-capable` target environment to illustrate key concepts
in software generation.

### Interrupt Handling
 In CMSIS, interrupt handling requires the following:
 - an IRQ vector entry for the handler
 - a handler subroutine

```c++

//
// DMA IRQ Vector Entry.
//
// This information is obtained after diplomatic resolution during hardware
// composition

// Forward Declaration of IRQ Handlers
void __attribute__((weak, alias ("Default_Handler")))
DMA_IRQHandler(void);

// Type Definition for IRQ Handlers
typedef void (* const IRQHandler)(void);

// Table of IRQ Handlers
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
    DMA_IRQHandler
};

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

```c++
// DMA HAL contains the generated register file
// from SystemRDL description
#include "DMA_HAL.h"
#include "SoC_Wrappers.h"

//
// Helper Handler Generated from RegSeq
//
static DMA_Status Helper_DMA_IRQHandler(DMAInst *InstancePtr)
{
    DMA_Status stat = DMA_OK;
    if (DMA_HAL_Regs.STATUS.ERR == 1) stat = DMA_ERROR;
    else if (DMA_HAL_Regs.STATUS.DONE == 0) stat = DMA_ABORT;
    DMA_HAL_Regs.STATUS |= 1;

    return stat;
}

// DMA IRQ Handler
void DMA_IRQHandler(void)
{
    NVIC_DisableIRQ(DMA_IRQn);
    DMAInst *InstancePtr = Wrappers_GetIrqDev(DMA_IRQn);
    if (InstancePtr) {
        DMA_Status stat = Helper_DMA_IRQHandler(InstancePtr);
        InstancePtr.Status = stat;
    }
    NVIC_EnableIRQ(DMA_IRQn);
    return;
}

### Non-Interrupt API

```c++
#include <DMA_HAL.h>

//
// Simple DMA Transfer
// @param Src   : source address
// @param Dest  : desctination address
// @param Size  : size of transfer in bytes
//
// Constraints:
// - Src must be 4-byte aligned
// - Dest must be 4-byte aligned
// - Size must be < (1 << 30)
DMA_Status SimpleTransfer(u64 *Src, u64 *Dest, u32 Size)
{
    // Requirements

    u64 iSrc    = (u64 Src);
    u64 iDest   = (u64 Dest);

    if (iSrc & 0x3)
        return DMA_ERROR;
    if (iDest & 0x3)
        return DMA_ERROR;
    if (Size >= (1 << 30))
        return DMA_ERROR;

    // Transfer Setup
    DMA_HAL_Regs.SA     = (u32) iSrc;
    DMA_HAL_Regs.DA     = (u32) iDest;
    DMA_HAL_Regs.CTRL.IE = 1;
    DMA_HAL_Regs.BTT    = Size;

    return Wrappers_GetStatusCode(Wrappers_WaitForEvent(DMA_IRQn));
}
```

```
