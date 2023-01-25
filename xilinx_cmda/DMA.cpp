#include <stdint.h>

//==============================================================================
// Type Declarations
//------------------------------------------------------------------------------

typedef uint32_t        u32;
typedef uint64_t        u64;

//
// Transfer Control
//
typedef union {
    u32 Cfg;
    struct {
        u32 SGMode  : 1;
        u32 Cyclic  : 1;
        u32 KHR     : 1;
        u32 KHW     : 1;
    } CfgItems;
} CDMA_XferCtrl_t;

//
// Transfer Configuration
//
typedef struct {
    CDMA_XferCtrl_t XferCtrl;
    u32 Length;
    u64 SrcAddr;
    u64 DestAddr;
} CDMA_XferConfig_t;

//
// Device Configuration
//
typedef union {
    u32 Cfg;
    struct {
        u32 IrqEn       : 1;
        u32 ErrIrqEn    : 1;
        u32 DlyIrqEn    : 1;
        u32 IOCIrqEn    : 1;
        u32 IrqThresh   : 8;
        u32 IrqDelay    : 8;
    } CfgItems;
} CDMA_Config_t;

//
// Device Handle
//
typedef struct CDMA_s {
    u32 *BaseAddr;
    CDMA_Config_t Config;
    CDMACallBackFn CallBack;
} CDMA_t;

//
// CDMA CallBack Function Type
//
typedef void (* CDMACallBackFn)(CDMA_t *DevPtr, uint32_t EventMask);

//
// CDMA Status Codes
//
enum CDMA_Status_e {
    OK      = 0,
    ERR
};



//==============================================================================
// Macro Definitions
//------------------------------------------------------------------------------

#define CDMA_EVENT_IOC          (1 << 0)
#define CDMA_EVENT_DLY          (1 << 1)
#define CDMA_EVENT_ERR          (1 << 2)

//==============================================================================
// Function Declarations
//------------------------------------------------------------------------------

CDMA_Status_e CDMA_Init(CDMA_t *InstancePtr, CDMA_Config_t Config, u32 BaseAddr);

// IRQ/Events
CDMA_Status_e CDMA_SetCallBack(CDMA_t *InstancePtr, CDMACallBackFn CallBack);
CDMA_Status_e CDMA_EnableEvents(CDMA_t *InstancePtr, u32 EventMask);
CDMA_Status_e CDMA_DisableEvents(CDMA_t *InstancePtr, u32 EventMask);
CDMA_Status_e CDMA_SetIRQDelay(CDMA_t *InstancePtr, u32 IRQDelay);
CDMA_Status_e CDMA_SetIRQThreshold(CDMA_t *InstancePtr, u32 IRQThreshold);

// Transfer
CDMA_Status_e CDMA_Start(CDMA_t *InstancePtr, CDMA_XferConfig_t *XferConfig);
CDMA_Status_e CDMA_WaitCompletion(CDMA_t *InstancePtr);
CDMA_Status_e CDMA_CheckXferConfig(CDMA_t *InstancePtr, CDMA_XferConfig_t *XferConfig);
