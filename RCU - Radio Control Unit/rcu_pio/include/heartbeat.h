#pragma once

#include "mbed.h"

namespace heartbeatConfig{
    constexpr std::chrono::microseconds UPDATE_TIME = 500ms;
}

namespace heartbeatStatus{
    constexpr uint8_t ERROR = 0;
    constexpr uint8_t OK = 1;
}

class Heartbeat{
    private:
        Timer _heartbeatTimer; //Timer for unit heartbeat status messages
        CAN* _databus_P;
        uint16_t _canStatusReq_ID;
        uint8_t _currentStatus;
        void _send(uint8_t status);
    public:
        Heartbeat(CAN *databus_P, uint16_t canStatusReq_ID);
        void update();
        bool setStatus(uint8_t status);
        uint8_t getStatus(){return _currentStatus;}
};
