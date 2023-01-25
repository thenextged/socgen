# Xilinx AXI CMDA

## Features
- AXI4 master interface for data interface
- AXI4-Lite slave interface for register access
- AXI4 master interface for Scatter/Gather function
- Data realignment engine
- Register direct mode
- Fixed address and incrementing-address burst support

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

In trying to define the SystemRDL spec for the Xilinx CMDA, we found a need for
**parameterizing** the top-level *SystemRDL addressmap*. We think that this is necessary for capturing RDL specs for configurable IP. Alternatively, one can provide distinct instances of specs for each IP hardware configuration mode but we think that this is not a good solution. Therefore, we want the following added functionality to SystemRDL:
- ability to parameterize the address-map
- ability to condition structure and behavior based on the expressions containing the parameters
- allow tools to bind parameter values before elaborating the SystemRDL spec (like IPXACT, we'll have an IP RDL spec and IP instance RDL spec)

## Initialization

During initialization, we want to set the following properties:
- interrupt enable or disable
- enable/disable ERR_Irq
- enable/disable IOC_Irq
- enable/disable Dly_Irq
- set IRQDelay
- set IRQThreshold

## Types of transfer

- Simple transfer (Normal, Keyhole Read, Keyhole Write)
- Scatter-Gather transfer (Normal, Keyhole Read, Keyhole Write)\
- Cyclic transfer (Normal, Keyhole Read, Keyhole Write)

## Services

The DMA sequences make use of the following resources:
- Interrupt
- Instance Handle
- Memory
- Register Space

## Resources

When an IP is connected in a system, it requires certain interfaces to be connected externally. Some of these interfaces are:
- power control
- clocks and resets
- interrupts
- data interfaces

From a software perspective, when specifying sequence specs for an IP, there might be dependencies on system-centric components. Most of the times, those components have a 1-to-1 mapping with physical interfaces.

The challenge in writing seqences is that they must be provided in an implementation-indepdent fashion. That means we should not rely on a target software environment (bare-metal, OS, etc.) to completely specify a sequence.

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