# Linux DMA Engine - Slave API

## Usage

To use a DMA using the slave DMAEngine API:
1. Allocate a DMA channel
2. Set slave and controller specific parameters
3. Get a descriptor for the transaction
4. Submit the transaction
5. Issue pending requests and wait for callback notification

### Allocate a DMA Slave Channel

```c++
struct dma_chan *dma_request_chan(struct device *dev, const char *name);

```

### Set Slave and Controller Specific Parameters

The DMA driver needs to be passed some information. Most of the generic information that can be passed from a slave to the DMA can be found in the `struct dma_slave_config`.

If some DMA controller requires more parameters/info to be passed, then the slave can embed the `struct dma_slave_config` in the controller-specific structure. This is basically some sort of inheritance from the `struct dma_slave_config`.

```c++
int dmaengine_slave_config(struct dma_chan *chan, struct dma_slave_config *config);
```

The configuration information passed on to the DMA pertains to the previously requested channel.
The `struct dma_slave_config` details can be obtained from `dmaengine.h`

### Get a Descriptor for the Transaction
