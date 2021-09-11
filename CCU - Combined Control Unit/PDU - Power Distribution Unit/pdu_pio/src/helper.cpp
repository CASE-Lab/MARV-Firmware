#include "helper.h"

bool waitUntill(Timer* timer, std::chrono::microseconds micros, uint32_t id){
    bool missedDeadline = true;
    while(timer->elapsed_time() < micros){
        missedDeadline = false;
        wait_us(1);
        //debug("wait\n\r");
    }

    if(missedDeadline){
        printf("ERROR: missed deadline");
        //print id if defined (not zero)
        if(id){
            printf(" with id: %lu\n\r", id);
        }
        else{
            printf("\n\r");
        }

    }

    return missedDeadline;
}

bool canBusError(CAN* bus, bool debugEn){
    static uint8_t rxErrorMax = 0;
    static uint8_t txErrorMax = 0;
    uint8_t rxError = bus->rderror();
    uint8_t txError = bus->tderror();

    if(rxError > rxErrorMax){
        rxErrorMax = rxError;
    }

    if(txError > txErrorMax){
        txErrorMax = txError;
    }

    //reading of last error code
    uint32_t* CAN1_CAN_ESR_REG = (uint32_t*)(0x40006400 + 0x18);
    uint32_t lastErrorCode = ((*CAN1_CAN_ESR_REG)>>4)&0b111;

    //debug for current transmit error counter
    debug_if(debugEn,"Current [TEC REC]: [%u %u]  ", txError, rxError);
    debug_if(debugEn,"Max [TEC REC]: [%u %u]\n\r", txErrorMax, rxErrorMax);
    debug_if(debugEn,"Last error code [%lu]\n\r", lastErrorCode);
    return rxError || txError;
}