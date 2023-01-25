# Infineon/Cypress DMA Hardware Abstraction Layer (HAL) API

Source : https://infineon.github.io/psoc6hal/html/group__group__hal__dma.html

- The DMA driver allows for initializing and configuring a DMA channel in order to trigger data transfers to and from memory and peripherals
- The transfers occur independently of the CPU and are triggered in software
- Multiple channels are available with user-selectable priority and transfer characteristics

## Supported Features

- CPU independent memory access
- Access to memory and peripherals
- Multiple independent channels
- Configurable transfer sizes and bursts
- Configurable priorities
- Event completion notification

## Data Structures

### DMA Results

| Macro | Comment |
|-------|---------|
| CYHAL_DMA_RSLT_ERR_INVALID_TRANSFER_WIDTH | Invalid transfer width parameter error |
| CYHAL_DMA_RSLT_ERR_INVALID_PARAMETER | Invalid parameter error |
| CYHAL_DMA_RSLT_ERR_INVALID_PRIORITY | Invalid priority parameter error |
| CYHAL_DMA_RSLT_ERR_INVALID_ALIGNMENT | Invalid src or dst alignment error |
| CYHAL_DMA_RSLT_ERR_INVALID_BURST_SIZE | Invvalid burst_size parameter error |
| CYHAL_DMA_RSLT_ERR_CHANNEL_BUSY | Channel busy error |
| CYHAL_DMA_RSLT_WARN_TRANSFER_ALREADY_STARTED | Transfer has already been started warning |
| CYHAL_DMA_RSLT_FATAL_UNSUPPORTED_HARDWARE | Unsupported hardware error |

### DMA Transfer Configuration

```c++

struct cyhal_dma_cfg_t {
    uint32_t    src_addr;           // source address
    uint16_t    src_increment;      // source address auto increment amount in multiples of transfer_width
    uint32_t    dst_addr;           // destination address
    uint16_t    dst_increment;      // destination address increment amount in multiples of transfer_width
    uint8_t     transfer_width;     // transfer width in bits. Valid values are 8, 16, or 32
    uint32_t    length;             // number of elements to be transferred in total
    uint32_t    burst_size;         // number of elements to be transferred per trigger. If set to 0 every element is transferred, otherwise burst_size must evenly divide length
    cyhal_dma_transfer_action_t action; // sets the behavior of the channel when triggered (sung start_transfer). Ignored if burst_size is not configured
}
```

```c++
// Direction for DMA transfers
enum cyhal_dma_direction_t {
    CYHAL_DMA_DIRECTION_MEM2MEM,        // memory to memory
    CYHAL_DMA_DIRECTION_MEM2PERIPH,     // memory to peripheral
    CYHAL_DMA_DIRECTION_PERIPH2MEM,     // peripheral to memory
    CYHAL_DMA_DIRECTION_PERIPH2PERIPH,  // peripheral to peripheral
}
```

```c++
// Flags for DMA events
// Multiple events can be enabled
enum cyhal_dma_event_t {
    CYHAL_DMA_NO_INTR,                  // No interrupt
    CYHAL_DMA_TRANSFER_COMPLETE,        // indicates that a burst or full transfer has completed
    CYHAL_DMA_SRC_BURS_ERROR,           // indicates that there is a source bus error
    CYHAL_DMA_DST_BUS_ERROR,            // indicates that there is a destination bus error
    CYHAL_DMA_SRC_MISAL,                // indicates that the source address is not aligned
    CYHAL_DMA_DST_MISAL,                // indicates that the destination address is not aligned
    CYHAL_DMA_CURR_PTR_NULL,            // indicates tat the current descriptor pointer is null
    CYHAL_DMA_ACTIVE_CH_DISABLED,       // indicates that the active channel is disabled
    CYHAL_DMA_DESCR_BUS_ERROR,          // indicates that there has been a descriptor bus error
}
```

```c++
// If burst_size is used, selects whether a single trigger of the channel transfers a single burst of burst_size or a full transfer of size length (that is, every burst is triggered).
// This will also select when a trigger complete event will occur: after each burst or after full transfer

enum cyhal_dma_transfer_action_t {
    CYHAL_DMA_TRANSFER_BURST,           // a single burst is triggered and a transfer completion event will occur after the burst
    CYHAL_DMA_TRANSFER_FULL,            // all bursts are triggered and a single transfer completion event will occur at the end of all of them
}
```

## APIs

```c++
// Initialize the DMA peripheral
//
// @param obj       : pointer to DMA object. The caller must allocate the memory for this object but the init function will initialize its contents
// @param priority  : priority of this DMA operation relative to others.
//                    The number of priority levels which are supported is hardware dependent.
//                    All implementations define a CYHAL_DMA_PRIORITY_DEFAULT constant which is always valid.
//                    If supported, implementations will also define:
//                      - CYHAL_DMA_PRIORITY_HIGH
//                      - CYHAL_DMA_PRIORITY_MDEIUM
//                      - CYHAL_DMA_PRIORITY_LOW
// @param direction : the direction memory is copied
// @return the status of the init request
//
cy_rslt_t cyhal_dma_init (  cyhal_dma_t *           obj,
                            uint8_t                 priority,
                            cyhal_dma_direction_t   direction
                        )

//
// Free the DMA object
//
// @param obj       : the DMA object
//
cy_rslt_t cyhal_dma_free(   cyhal_dma_t *           obj )

//
// Set up a DMA descriptor for specified resource
//
// @param obj       : the DMA object
// @param cfg       : configuration parameters for the transfer
// @return the status of the configure request
//
cy_rslt_dma_configure(  cyhal_dma_t *               obj,
                        const cyhal_dma_cfg_t *     cfg
                    )

//
// Initiates DMA channel transfer for specified DMA object
//
// @param obj       : the DMA object
// @return the status if the start transfer request
//
cy_rslt_t cyhal_dma_start_transfer( cyhal_dma_t *   obj )

//
// Checks whether a transfer is pending or running on the DMA channel
//
// @param obj       : the DMA object
// @return True if DMA channel is busy, False otherwise
cy_rslt_t cyhal_dma_is_busy(    cyhal_dma_t *       obj )

//
// Register a DMA callback handler
//
// @param obj       : the DMA object
// @param callback  : the callback handler which will be invoked when an event triggers
// @param callback_arg : generic argument that will be provided to the callback when called
//
void cyhal_dma_register_callback(   cyhal_dma_t *   obj,
                                    cyhal_dma_event_callback_t  callback,
                                    void *          callback_arg
                                )

//
// Configure DMA event enablement
//
// @param obj       : the DMA object
// @param event     : the DMA event type
// @param intr_priority : the priority for NVIC interrupt events. The priority from the most recent call will take precedence, i.e all events will have the same priority
// @param enable    : True to turn on interrupts, False to turn off
//
void cyhal_dma_enable_event(    cyhal_dma_t *       obj,
                                cyhal_dma_event_t   event,
                                uint8_t             intr_priority,
                                bool                enable
                            )
```
