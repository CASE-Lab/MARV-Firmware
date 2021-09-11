#include "heartbeat.h"

using namespace std::chrono;

Timer heartbeatTimer; //Timer for unit heartbeat status messages
CAN *databus_P;
uint16_t canStatusReq_ID;
uint8_t current_status;

//Constructors
heartbeat::heartbeat(CAN *databus_P, uint16_t canStatusReq_ID){
    this->databus_P = databus_P;
    this->canStatusReq_ID = canStatusReq_ID;
    this->current_status = 1;
    heartbeatTimer.start();
}

//Send status if enough time has passed
void heartbeat::update(){
    //Send unit heartbeat
        if(duration_cast<milliseconds>(heartbeatTimer.elapsed_time()).count() > heartbeatUpdateTime){
            char status[2] = {1,0};
            databus_P->write(CANMessage(canStatusReq_ID,status,2));
            heartbeatTimer.reset();
        }
}

//Update status to send, return true if valid status, false if status is out of the scope of valid options
bool heartbeat::setStatus(uint8_t status){
    if(status == 0){ //Error
        current_status = 0;
        return true;
    }
    else if(status == 1){ //Okay
        current_status = 0;
        return true;
    }
    else{
        return false;
    }
}




