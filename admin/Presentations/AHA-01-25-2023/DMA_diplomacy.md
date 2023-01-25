# Diplomacy Layer for Example SoC

```c++

#include "soc_tools.h"
#include "hw_spec.h"

int main(int argc, char *argv[])
{

    //
    // Diplomatic Node Declarations
    //

    System SoC("SoC");

    Node DMA("AHADMA");
    Node ClockResetCtrl("ClockResetCtrl");
    Node CPU("CPU");
    Node IrqCtrl("IRQ_CTRL");
    Node SRAM("SRAM");
    Node Interc("Interconnect");

    //
    // Diplomatic Node Configurations
    //

    // DMA
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
    Interc.Connect(DMA);
    Interc.Connect(SRAM);
    Interc.Connect(IrqCtrl);

    ClockResetCtrl.Connect(DMA);

    IrqCtrl.Connect(DMA);

    CPU.Connect(IrqCtrl.GetIF("CombinedIrq"));

    //
    // CodeGen
    //
    SoC.GenHAL("Onyx");

    return 0;
}


```
