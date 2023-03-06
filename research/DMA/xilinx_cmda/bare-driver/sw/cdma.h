//==============================================================================
// Includes
//------------------------------------------------------------------------------

#include "xil_types.h"

//==============================================================================
// Macro Definitions
//------------------------------------------------------------------------------

#ifndef __IO
#define __IO    volatile
#endif /* __IO */

#ifndef __I
#define __I     volatile const
#endif /* __I */

#define MASK(bit)           (1 << (bit))
#define MASK_RANGE(msb,lsb) \
    (((1<<((msb)+1))-1) ^ ((1<<(lsb))-1))

#define TEST_BIT(n,pos) \
    (!!((n) & MASK(pos)))

#define PNTR_TO_UINT(ptr)  \
    ((UINTPTR)(const void *)(ptr))

//==============================================================================
// CONSTANTS
//------------------------------------------------------------------------------

#define CDMA_MAX_BTT    ((1 << 26) - 1)

//==============================================================================
// CDMA Register Space
//------------------------------------------------------------------------------

typedef struct CDMA_r {
    __IO    u32 CDMACR;
    __IO    u32 CDMASR;
    __IO    u32 CURDESC_PNTR;
    __IO    u32 CURDESC_PNTR_MSB;
    __IO    u32 TAILDESC_PNTR;
    __IO    u32 TAILDESC_PNTR_MSB;
    __IO    u32 SA;
    __IO    u32 SA_MSB;
    __IO    u32 DA;
    __IO    u32 DA_MSB;
    __IO    u32 BTT;
} CDMA_r;


//
// CDMACR Fields
//

#define CDMACR_TAILPTREN_Pos        1
#define CDMACR_RESET_Pos            2
#define CDMACR_SGMODE_Pos           3
#define CDMACR_KHR_Pos              4
#define CDMACR_KHW_Pos              5
#define CDMACR_IOC_IRQEN_Pos        12
#define CDMACR_DLY_IRQEN_Pos        13
#define CDMACR_ERR_IRQEN_Pos        14
#define CDMACR_IRQTHRESHOLD_Pos     16
#define CDMACR_IRQDELAY_Pos         24

//
// CDMASR Fields
//

#define CDMASR_IDLE_Pos             1
#define CDMASR_SGINCLD_Pos          3
#define CDMASR_DMAINTERR_Pos        4
#define CDMASR_DMASLVERR_Pos        5
#define CDMASR_DMADECERR_Pos        6
#define CDMASR_SGINTERR_Pos         8
#define CDMASR_SGSLVERR_Pos         9
#define CDMASR_SGDECERR_Pos         10
#define CDMASR_IOC_IRQ_Pos          12
#define CDMASR_DLY_IRQ_Pos          13
#define CDMASR_ERR_IRQ_Pos          14
#define CDMASR_IRQTHRESHOLD_Pos     16
#define CDMASR_IRQDELAY_Pos         24

//==============================================================================
// CDMA EVENTS
//------------------------------------------------------------------------------

#define CDMA_EVENT_ERR              14
#define CDMA_EVENT_DLY              13
#define CDMA_EVENT_IOC              12
#define CDMA_EVENT_SGDECERR         10
#define CDMA_EVENT_SGSLVERR         9
#define CDMA_EVENT_SGINTERR         8
#define CDMA_EVENT_DMADECERR        6
#define CDMA_EVENT_DMASLVERR        5
#define CDMA_EVENT_DMAINTERR        4

struct CDMA_t;
typedef void (*CDMA_CallbackFn)(CDMA_t *InstancePtr, u32 Events);

//==============================================================================
// Instance Data
//------------------------------------------------------------------------------

typedef struct CDMA_t {
    CDMA_r *        RegSpace;
    int             AddrWidth;
    int             DataWidth;
    int             HasSG;
    int             HasDRE;
    int             IsLite;
    int             BurstLen;
    CDMA_CallbackFn Callback;
} CDMA_t;

//==============================================================================
// Functional API
//==============================================================================

int CDMA_Init(u32 DeviceId, CDMA_t *InstancePtr);

int CDMA_Xfer_Simple_Poll(CDMA_t *InstancePtr, UINTPTR SrcAddr, UINTPTR DstAddr, u32 Len);

int CDMA_SetCallback(CDMA_t *InstancePtr, CDMA_CallbackFn Callback);
int CDMA_Xfer_Simple_Irq(CDMA_t *InstancePtr, UINTPTR SrcAddr, UINTPTR DstAddr, u32 Len);
