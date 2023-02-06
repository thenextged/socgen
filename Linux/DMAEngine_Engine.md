# Linux DMA Engine - DMA Driver

## Simplistic Hardware Abstraction

The simplistic DMA hardware abstraction used in the Linux DMAEngine framework is as follows:
- a DMA has a N channels to use for the DMA transfers and M peripheral request lines
- channels are the entities that are used to perform the DMA transactions
- request lines are entities that dictate which endpoints are involved in the transfer
- request lines correspond to physical lines connecting DMA-eligible devices to the DMA controller
- scatter-gather entry format is specific to the DMA hardware used but in general will contain at least the following information:
    - source address
    - destination address
    - address increment mode
    - burst size : how many transfers to bundle up
    - transfer width : how many bits in each single transfer beat
    - transfer size : the total byte size of the transfer

## Major DMA Features
- Multiple register interfaces (e.g. secure and non-secure accesses)
- DRQ with peripheral request lines
- Interrupt granularity
- Stream vs. memory-mapped interfaces
- In-flight data manipulations (XOR, PQ, INC/DEC, etc.)
- Scatter-Gather support
- Transfer lifecycle control

## DMA Abstraction

- A DMA device moves data from an origin point to a destination point
- 3 steps are involved in any DMA transfer
    - DMA transfer specification/configuration
    - DMA channel configuration
    - DMA request
- request interfaces are used to control the life cycle of a DMA transaction:
    - START
    - PAUSE
    - RESUME
    - TERMINATE
- request interfaces are physically implemented as:
    - at least 1 software accessible register interface
    - optionally 1 or more dedicated peripheral request interfaces (DRQ lines)
    - DRQ lines often only implement the START operation
- channels are the entities that carry the actual DMA transfer. Each channel has a list of supported features can be used to manipulate a transfer
- DMA transfer specifications/configurations are, for the most part, a binding of legal values for the channel being used.

Creating a DMA abstraction essentially comes down to creating a unified abstraction for the channel configuration

### Channel Configuration

A DMA channel is configured with the following:
- supported source address widths (bitmask)
- supported destination address widths (bitmask)
- supported transfer directions (bitmask)
- supported minimum burst size
- supported maximum burst size
- supported completion granularity (bitmask: word, burst, descriptor, transfer)
- supported IRQ granularity is (bitmask: word, burst, descriptor, transfer)
- supported in-flight computations
- supported access patterns:
    - 1-D: contiguous block
    - 2-D: rectangular block with strides
- support for descriptor coalescing
- support for PAUSE/RESUME
- support for TERMINATE
- support for DRQ
- support for scatter-gather lists

### Transfer Specification

A DMA transfer specification is a binding of legal values to a channel configuration.

### API

Life Cycle API:
- START : triggered by software or hardware (DRQ)
- TERMINATE
- PAUSE
- RESUME
- GET_STATUS

Channel/Transfer Configuration API:
- GET_CHANNEL_INFO
- PREPARE_TRANSFER

### System-Level Resources Needed

DMA sequences utilize system-level resources to accomplish their tasks. The following is a list of system-level resources frequently used:
- IRQ
    - waiting for interrupts
- Memory
    - allocating/accessing buffers
    - synchronization
    - cache management
- Execution Contexts
    - waiting in a thread/tasklet
- Notifications
    - async calls
