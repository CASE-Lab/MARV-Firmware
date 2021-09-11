#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "mbed.h"

#define heartbeatUpdateTime 500

class heartbeat{
    private:
        Timer heartbeatTimer; //Timer for unit heartbeat status messages
        CAN *databus_P;
        uint16_t canStatusReq_ID;
        uint8_t current_status;
    public:
        heartbeat(CAN *databus_P, uint16_t canStatusReq_ID);
        void update();
        bool setStatus(uint8_t status);
};

#endif //HEARTBEAT_H