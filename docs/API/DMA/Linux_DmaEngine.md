# Linux DMA Engine Framework

The Linux DMA engine framework provides an abstraction/interface that decouples vendor-specific DMA engines from the rest of the Linux environment.
With the Linux DMA engine framework, we have the following 4 layers:
- User-land code
- Kernel-land client driver
- DMA engine framework
- vendor-specific DMA driver

At a minimum, a DMA transaction is specified by the following:
- Source Address
- Destination Address
- Size
- Configuration

## DMA Hardware Model

A DMA controller hardware is modeled/characterized as follows:
- at least one master interface for data transfers
- at least one slave interface for control operations
- optionally, dedicated master interface(s) for descriptor/code access
- optionally, dedicated peripheral request lines
- optionally, dedicated interrupt lines 
