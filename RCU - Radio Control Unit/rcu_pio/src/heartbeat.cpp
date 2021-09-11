#include "heartbeat.h"

using namespace std::chrono;

Timer heartbeatTimer; //Timer for unit heartbeat status messages
CAN *databus_P;
uint16_t canStatusReq_ID;
uint8_t current_status;

//Constructors
Heartbeat::Heartbeat(CAN *databus_P, uint16_t canStatusReq_ID){
    this->_databus_P = databus_P;
    this->_canStatusReq_ID = canStatusReq_ID;
    this->_currentStatus = heartbeatStatus::OK;
    _heartbeatTimer.start();
}

//Send status if enough time has passed
void Heartbeat::update(){
    //Send unit heartbeat
        if(_heartbeatTimer.elapsed_time() > heartbeatConfig::UPDATE_TIME){
            _send(_currentStatus);
            _heartbeatTimer.reset();
        }
}

//Update status to send, return true if valid status, false if status is out of the scope of valid options
bool Heartbeat::setStatus(uint8_t status){
    switch (status)
    {
    case heartbeatStatus::ERROR:
        _currentStatus = status;
        _send(_currentStatus);
        return true;
        break;

    case heartbeatStatus::OK:
        _currentStatus = status;
        _send(_currentStatus);
        return true;
        break;

    default:
        return false;
        break;
    }
}

void Heartbeat::_send(uint8_t status){
    _databus_P->write(CANMessage(_canStatusReq_ID,&status,1));
}