#include "canHandler.h"

CanHandler::CanHandler(CAN* bus, bool rxInt, bool overwrite, int handle) :
    _bus (bus),
    _rxMissed (0),
    _overwrite (overwrite),
    _rxInt (rxInt),
    _handle (handle){
    
    //setup RX interuppt if enabled
    if(_rxInt){
        _bus->attach(callback(this, &CanHandler::_canRxCallback));
    }
}

void CanHandler::_canRxCallback(){
    //read bus
    CANMessage rxMsg;
    while(_bus->read(rxMsg)){
        //add to buffer and check if buffer is overwirtten/not added
        if(!_canBuffer.add(rxMsg, _overwrite)){
            _rxMissed++;
        }
    }
}