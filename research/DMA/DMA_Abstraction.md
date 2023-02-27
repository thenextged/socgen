# DMA Abstraction/Model

In this article, we develop a complete DMA abstraction. The DMA abstraction is made of 2 sub-abstractions:
- DMA Hardware Abstraction
- DMA Software Abstraction

Our overall strategy in developing the DMA abstraction is quite straightforward: examine representative DMA engines and formulate a common abstraction.

## DMA Hardware Abstraction/Model

From a hardware perspective, a DMA device has the following physical interfaces

- clocks
- resets
- interrupts/events
- data
- register
- peripheral request

The DMA Hardware Abstraction is most useful in the **hardware construction** stage of an SoC. In this study, however, we are most concerned with the **software construction** stage. Hence, we will assumme that a proper DMA hardware abstraction exists and we will focus on/use only those features that are relevant to **software construction**.

## DMA Software Abstraction/Model

From a software perspective, a DMA device consists of two types of channels that entirely control its functionality:
- request channels
- data channels

### Request Channels

**Request channels** are used to manage the lifecycle of a DMA transfer. These channels can be physical, software, or both.
Traditional lifecycle operations supported on a request channel are:
- START
- PAUSE
- RESUME
- TERMINATE

### Data Channels

**Data channels** are used to perform the actual DMA transfer. Hence, data channels are configured with the specific characteristics of a transfer. Each data channel is associated with a request channel that dictates which lifecycle operations are permitted on the channel

#### How are Channels Used

The goal is the following:
1. First, a DMA device will specify the characteristics of all of its channels
2. Second, each channel is specified using the same parameter space
3. Third, the `transfer` function is written, taking a channel configuration as parameter
4. Finally, the codegen will generate all the target-dependent code (along with operations for request channels)

#### Channel Specification

1. supported source address widths
2. supported destination address widths
3. supported transfer directions
4. supported burst size range (min, max)
5. supported completion/IRQ granularity (word, burst, descriptor, transfer)
6. supported access patterns (1-D contiguous block, 2-D blocks with strides)
7. supported in-flight computations (XOR, PQ, etc.)
8. support for descriptor coalescing
9. support for scatter-gather

##### Source Address Widths

Supported bus widths for the READ interface

##### Destination Address Widths

Supported bus widths for the WRITE interface

##### Transfer Directions

Supported DMA transfer types (MEM2MEM, MEM2DEV, DEV2MEM, DEV2DEV)

##### Burst Size

Supported range of burst sizes (Min, Max)

##### Completion/IRQ Granularity

Supported levels of completion events (word, burst, descriptor, transfer)

##### Access Patterns

Supported memory access patterns:
- 1-D : for contiguous blocks
- 2-D : for strided blocks

##### In-Flight/Side Computations

Supported in-flight/side computations carred by the engine

##### Scatter-Gather

