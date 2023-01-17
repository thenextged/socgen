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
