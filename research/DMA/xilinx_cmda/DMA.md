# Xilinx AXI CMDA

The goal here is to :
- develop a standalone software driver for the Xilinx CDMA
- capture standalone driver in RegSeq
- develop a DMAEngine-based Linux driver for the Xilinx CDMA
- capture a RegSeq description of the Xilinx CDMA (using RegSeq DMA abstraction)
- regenerate the different drivers using the RegSeq specification
- refine the RegSeq DMA abstraction as needed

## Features
- AXI4 master interface for data access
- AXI4-Lite slave interface for register access
- AXI4 master interface for Scatter/Gather function
- Data realignment engine
- Simple and SG modes
- 32-bit and 64-bit address modes
- Fixed and incrementing address burst support

## Register Space

| Address Offset | Name | Description |
|----------------|------|-------------|
| 00h | CDMACR | Control Register |
| 04h | CDMASR | Status Register |
| 08h | CURDESC_PNTR | Current Descriptor Pointer |
| 0Ch | CURDESC_PNTR_MSB | Current Descriptor Pointer MSB 32 bits |
| 10h | TAILDESC_PNTR | Tail Descriptor Pointer |
| 14h | TAILDESC_PNTR_MSB | Tail Descriptor Pointer MBS 32 bits |
| 18h | SA | SOurce Address |
| 1Ch | SA_MSB | Source Address MBS 32 bits |
| 20h | DA | Destination Address |
| 24h | DA_MSB | Destination Address MSB 32 bits |
| 28h | BTT | Bytes to transfer |

## SystemRDL Comments

The Xilinx CDMA has the following hardware parameterizations that affect software :
- address widths (32 and 64 bits)
- Scatter-gather support (simple or SG with buffer descriptors)
- Data realignment engine (DRE)
- Maximum transfer length

In addition to the IP-specific parameters listed above, each Xilinx CDMA engine
also has a unique address. However, we can assume that the `dedicated physical address` information
is implicitly available from the realization of the `SystemRDL` specification.

We need to convey the aforementioned hardware parameterization to the software framework. The question becomes where should we capture the parameterization info? In the SystemRDL description or the RegSeq specification?

In the case of `SystemRDL`, we would need a way to **parameterize** the top-level *SystemRDL addressmap*. We think that this would be necessary for specifying reconfigurable IP. Therefore, we would want to add the following functionality to SystemRDL:
- ability to add **parameters** address-map
- ability to manipulate address map structures and register behavior using expressions containing **parameters**
- allow tools to bind parameter values before elaborating the SystemRDL spec

In the case of 'RegSeq', we would need to provide the parameterization information in the form of **instance-specific data**.
The `RegSeq` specification can then utilize the provided **instance-specific data** to dynamically perform different actions.
Following our **resource** strategy in `RegSeq`, **instance-specific data** can be modeled as a **RegSeq resource**.
It is then the task of the **initializer** to fill-in the right information for each device instance.

## Types of transfer

- Simple transfer (Normal, Keyhole Read, Keyhole Write)
- Scatter-Gather transfer (Normal, Keyhole Read, Keyhole Write)

Each of these two transfer types can have the following properties :
- Normal
- Fixed Read (Keyhole Read - KHR)
- Fixed Write (Keyhole Write - KHW)
- Cyclic

## Services

The DMA sequences make use of the following resources:
- Interrupt
- Instance-Specific Data
- Memory
- Register Definition

## Resources

When an IP is connected in a system, it requires certain interfaces to be connected externally. Some of these interfaces are:
- power control
- clocks and resets
- interrupts
- data interfaces

From a software perspective, when specifying sequence specs for an IP, there might be dependencies on system-centric components. Most of the times, those components have a 1-to-1 mapping with physical interfaces.

The challenge in writing sequences is that they must be provided in an implementation-independent fashion. That means we should not rely on a target software environment (bare-metal, OS, etc.) to completely specify a sequence.

The solution we are proposing is to define abstractions for those components/services at the sequence level. One of the main questions we need to answer is:
- How do we deal with resource binding

A DMA requires the following resources:
- interrupt
- register definition
- memory

The interrupt resource is used to deal with interrupts generically within the sequences.
Questions:
- How to deal with multiple physical interrupt lines

The register definition resource abstracts the details about how register access is implemented in the target environment. It allows us to specify register actions (read/writes).

The memory resource is a generalization of an accessible memory-mapped region. Other more specific abstractions can be built on top of the memory resource/abstraction

## Software

### Simple Mode

```c++

// Wait for DMA to be ready
while (CDMASR.IDLE != 1);

// Enable completion and error IRQs
CDMACR.IOC_IrqEn    = 1;
CDMACR.Err_IrqEn    = 1;

// Set source address
CDMA.SA     = <ADDR_LSB>;
CDMA.SA_MSB = <ADDR_MSB>;

// set destination address
CDMA.DA     = <ADDR_LSB>;
CDMA.DA_MSB = <ADDR_MSB>;

// Set number of bytes to transfer
CDMA.BTT    = <Bytes up to 67,108,863>;

// In poll mode:
// while (CDMASR.IDLE != 1)

// In IRQ Mode:
// if (CDMASR.IOC_Irq) CDMASR.IOC_Irq = 1;
// else if (CDMASR.ERR_Irq) CDMASR.ERR_Irq = 1;

```

> Note:
>   in Simple Mode, both the SOURCE and DESTINATION addresses must be aligned to the width of the address bus

### SG Mode

```c++

// Toggle CDMACR.SGMode to Accept new value for CDMA.CURDESC_PNTR
CDMACR.SGMode   = 0;
CDMACR.SGMode   = 1;

// Set CURDESC_PNTR
CDMA.CURDESC_PNTR       = <ADDR_LSB>;
CDMA.CURDESC_PNTR_MSB   = <ADDR_MSB>;

// Enable IRQ and Keyhole Features If needed
CDMACR.IOC_IrqEn        = 1;
CDMACR.Err_IrqEn        = 1;
// CDMACR.KHR           = 1;
// CDMACR.KHW           = 1;

// Write Valid pointer value for TAILDESC_PNTR
CDMA.TAILDESC_PNTR      = <ADDR_LSB>;
CDMA.TAILDESC_PNTR_MSB  = <ADDR_MSB>;   

// In poll mode:
// while (CDMASR.IDLE != 1);

// In IRQ Mode:
// if (CDMASR.IOC_Irq) CDMASR.IOC_Irq = 1;
// else if (CDMASR.ERR_Irq) CDMASR.ERR_Irq = 1;

```
7KU-8OX
> Note:
>   in SG mode, if DRE is not enabled, both the SOURCE and DESTINATION addresses must be aligned to the width of the address bus

### Cyclic Mode

Same procedure as in **SG Mode** with the following differences:
- The last buffer descriptor in the scatter-gather list should point to the first BD
- set `CDMACR.CYCLIC = 1`
- **TAILDESC_PNTR** should be programmed with a value that is not part of the BD chain (e.g. 0x50)

### Buffer Descriptor (BD) Format

Transfer descriptors must be aligned 16 32-bit word alignment (64-byte alignment) (e.g. 0x00, 0x40, 0x80, etc.)

| Offset | Name |
|--------|------|
| 00h   | NEXTDESCR_PNTR |
| 04h   | NEXTDESCR_PNTR_MSB (used in 64-bit mode only) |
| 08h   | SA (Source Address) |
| 0Ch   | SA_MSB (used in 64-bit mode only) |
| 10h   | DA (Destination Address) |
| 14h   | DA_MSB (used in 64-bit mode only) |
| 18h   | CONTROL/LENGTH (bits [26:0] only) |
| 1Ch   | Status |
