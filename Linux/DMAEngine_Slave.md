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

The 3 different modes of transfers supported by the DMAEngine are:
- scatter-gather (slave_sg) : DMA a list of scatter-gather buffers from/to a peripheral
- cyclic (dma_cyclic) : DMA a cyclic operation from/to a peripheral until the operation is explicitly stopped
- interleaved (interleaved_dma) :

Descriptors can have metadata attached to them. DMAEngine provides two ways to attach metadata to descriptors:
- Client Metadata
- Engine Metadata

1. Client Metadata

The metadata buffer is allocated/provided by the client driver and it is attached to the descriptor using:
```c++
int dmaengine_desc_attach_metadata(struct dma_async_tx_descriptor *desc, void *data, size_t len);
```

for DMA_MEM_TO_DEV and DMA_MEM_TO_MEM transfers:
- prepare the descriptor (dmaengine_prep_*)
- construct the metadata in the client's buffer
- use `dmaengine_desc_attach_metadata()` to attach the metadata buffer to the descriptor
- submit the transfer

for DMA_DEV_TO_MEM transfers:
- prepare the descriptor (dmaengine_prep_*)
- construct the metadata in the client's buffer
- use `dmaengine_desc_attach_metadata()` to attach the metadata buffer to the descriptor
- submit the transfer
- when the transfer has completed, the metadata should be available in the attached buffer

2. Engine Metadata

The metadata buffer is allocated/provided by the DMA driver.
The client driver can request the pointer, maximum size, and the currently used size of the metadata and can directly update or read it.

However, because the DMA driver manages the memory area containing the metadata, client drivers must not access the metadata buffer after the transfer completion callback has run for the descriptor.
If no completion callback has been defined for the transfer, then the metadata must not be accessed after `issue_pending` call.
In order words, if the aim is to read back the metadata after the transfer is completed, then the client must use the completion callback.

The API to use for engine metadata:
```c++
void *dmaengine_desc_get_metadata_ptr(struct dma_async_tx_descriptor *desc, size_t *payload_len, size_t *max_len);

int dmaengine_desc_set_metadata_len(struct dma_async_tx_descriptor *desc, size_t payload_len);
```

for DMA_MEM_TO_DEV and DMA_MEM_TO_MEM
- prepare the descriptor (dmaengine_prep_*)
- use `dmaengine_desc_get_metadata_ptr()` to get the pointer to the engine's metadata buffer
- update the metadata using the returned pointer
- use `dmaengine_desc_set_metadata_len()` to tell the DMA engine the amount of data the client has placed into the metadata buffer
- submit the transfer

for DMA_DEV_TO_MEM:
- prepare the descriptor (dmaengine_prep_*)
- submit the transfer
- on transfer completion, use `dmaengine_desc_get_metadata_ptr()` to get the pointer to the engine's metadata buffer
- read out the metadata from the pointer

**Note**: client drivers can query if a given mode is supported with:
```c++
bool dmaengine_is_metadata_mode_supported(struct dma_chan *chan, enum dmaengine_desc_get_metadata_mode mode);
```
Mixed use of DESC_METADATA_CLIENT and DESC_METDATA_ENGINE is not allowed. Client must use either of the modes per descriptor.

### Submit the Transaction

Once the descriptor has been prepared and the callback information added, it must be placed on the DMA engine drivers pending queue.

API:
```c++
dma_cookie_t dmaengine_submit(struct dma_async_tx_descriptor *desc);
```

The returned cookie can be used to check progress of the DMA engine activity via other DMA engine calls.
**Note:
- Once `dmaengine_submit()` is called, the submitted transfer descriptor (`struct dma_async_tx_descriptor`) belongs to the DMA engine. Consequently, the client must consider invalid the pointer to that descriptor.

### Issue Pending DMA Requests and Waiting for Callback Notification

The transactions in the pending queue can be activated by calling the `dma_async_issue_pending()` API. If the channel is idle, then the first transaction in the queue is started and subsequent ones queued up.

On completion of each DMA operation, the next in queue is started and a tasklet triggered. The tasklet will then call the client driver completion callback routine for notification, if set.

API:
```c++
void dma_async_issue_pending(struct dma_chan *chan);
```

### Further APIs

#### Terminate API

#### Pause API

#### Resume API
