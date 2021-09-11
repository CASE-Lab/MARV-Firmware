#pragma once

#include "mbed.h"
#include "RingBufCPP.h"
#include "config.h"

/*
Handels RX of can messages
Can be either interupt based if rxInt is set true or based on user polling
*/
class CanHandler{
    public:
    CanHandler(CAN* bus, bool rxInt, bool overwrite, int handle = 0);
    uint32_t getRxMissed(){return _rxMissed;}
    
    //copies CANMessage from buffer into destination and frees space in buffer
    bool popMsg(CANMessage* destination){return _canBuffer.pull(destination);}
    bool isEmpty(){return _canBuffer.isEmpty();}

    private:
    CAN* _bus;
    uint32_t _rxMissed;
    bool _overwrite;
    bool _rxInt;
    int _handle;
    void _canRxCallback();
    RingBufCPP<CANMessage, CanConfig::BUF_SIZE> _canBuffer;
};

