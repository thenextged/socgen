```c++

class AHADMA : public RegSeq, public HasIRQ {

    Resource<AHADMAReg> iDMAReg;
    Resource<AHADMAIrq> iDMAIrq;
    Resource<AHADMAInstance> iDMAInst;

public:
    //
    // Resource Binding with Constructor
    //
    AHADMA( const std::string &name,
            const Resource<AHADMAReg> &reg,
            const Resource<AHADMAIrq> &irq,
            const Resource<AHADMAInstance> &inst)
            :   RegSeq(name),
                iDMAReg(reg),
                iDMAIrq(irq),
                iDMAInst(inst)
    {}

    //
    // DMA Transfer
    // @param Src   The transfer source address
    // @param Dest  The transfer destination address
    // @param Size  The transfer size    
    //        
    __SEQ__ DMA_Status SimpleTransfer(MemObj Src, MemObj Dest, u32 Size)
    {
        // Requirements
        Req(Src.Alignment == 3);
        Req(Dest.Alignment == 3);
        Req(Size < (1 << 30));

        // Transfer Setup
        iDMAReg.SA.ADDR     = Src.GetAddress();     // alignment taken care of
        iDMAReg.DA          = Dest.GetAddress();    // alignment taken care of
        iDMAReg.CTRL.IE     = 1;
        iDMAReg.LEN         = Size;
        iDMAReg.CTRL.GO     = 1;

        Token<AHADMAIrq> token = iDMAIrq.WaitForEvent();
        return token.GetStatusCode();
    }     

    //
    // Target-Independent ISR.
    // IRQHandlers must:
    // - detect which event has occured
    // - return a status code
    __SEQ__ DMA_Status IRQHandler() override {
        DMA_Status stat = Status<AHADMA>(iDMAReg.STAT.STAT_CODE);
        // Clear Interrupt
        iDMAReg.INTR.CLR    =  1;
        return stat;
    }                             

};

```
