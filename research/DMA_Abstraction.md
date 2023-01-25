# The DMA Abstraction

In this study, we want to develop a unified abstraction for DMA devices.
Our strategy is as follows:
- collect information about various DMA devices that the most used in industry
- survey DMA APIs from most used software environments
- construct an abstraction that captures the essence of a DMA

### DMA Hardware Survey

From a hardware interface perspective, DMAs have the following interfaces

- clocks
- resets
- interrupt
- data
- register access
- peripheral request

#### Clock Interfaces

DMA engines will vary in the number of clock interfaces they possess. For DMA engines with multiple clock interfaces, each clock source will be clocking a different part of the engine.
The most common clock structures are:
- single-clock DMA: control and data paths are clocked by the same clock
- dual-clock DMA: control and data paths have different clocks where the control path's clock is usually expected to use a slower clock

#### Reset Interfaces

These follow the same structure as the `Clock Interfaces`. Each reset source is usually associated with a `Clock Interface`.

#### Interrupt Interfaces

DMA engines vary in the number of physical interrupt line they support. Some DMAs will collapse all events/interrupts onto a single interrupt interface whereas other DMA engines will dedicate certain events to different interrupt lines.

Below are the most seen configurations:
- single-IRQ DMA: all events/interrupts collapse onto a single physical interface
- multiple-IRQ DMA: 
