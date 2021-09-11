#include "heartbeat.h"

using namespace std::chrono;

//Constructors
Heartbeat::Heartbeat(CAN* databus, uint16_t canStatusReqID) :
_databus (databus),
_canStatusReqID (canStatusReqID),
_currentStatus (Status::OK),
_source (Source::NONE),
_errorType (ErrorType::NO_TYPE){
    _heartbeatTimer.start();
}

//Send status if enough time has passed
void Heartbeat::update(){
    //Send unit heartbeat
        if(_heartbeatTimer.elapsed_time() > heartbeatConfig::UPDATE_TIME){
            _send();
            _heartbeatTimer.reset();
        }
}

//Update status to send, return true if valid status, false if status is out of the scope of valid options
bool Heartbeat::setStatus(Status status){
    switch (status)
    {
    case Status::ERROR:
        _currentStatus = status;
        _send();
        return true;
        break;

    case Status::OK:
        _currentStatus = status;
        _errorType = ErrorType::NO_TYPE;
        _send();
        return true;
        break;

    default:
        return false;
        break;
    }
}

bool Heartbeat::setErrorType(ErrorType errorType){
    _currentStatus = Status::ERROR;
    switch (errorType)
    {
    case ErrorType::NO_TYPE:
        _errorType = errorType;
        _send();
        return true;
        break;
    
    default:
        debug("Incorrect ErrorType\n\r");
        return false;
        break;
    }
}

void Heartbeat::setSource(Source source){
    _source = source;
    _send();
}

void Heartbeat::_send(){
    CANMessage canMsg;
    canMsg.id = _canStatusReqID;
    canMsg.len = 3;
    canMsg.data[0] = (uint8_t)_currentStatus;
    canMsg.data[1] = (uint8_t)_errorType;
    canMsg.data[2] = (uint8_t)_source;
    _databus->write(canMsg);
}