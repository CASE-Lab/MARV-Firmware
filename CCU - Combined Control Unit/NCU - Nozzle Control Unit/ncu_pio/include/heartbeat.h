#pragma once

#include "mbed.h"

namespace heartbeatConfig{
    constexpr std::chrono::microseconds UPDATE_TIME = 500ms;
}



class Heartbeat{
    public:
        Heartbeat(CAN* databus, uint16_t canStatusReqID);

        //public enums
        enum Status{
            ERROR = 0, OK = 1
        };

        enum Source{
            NONE = 0, RCU = 1, ACU = 2, TESTING = 3
        };

        enum ErrorType{
            NO_TYPE = 0
        };

        void update();
        bool setStatus(Status status);
        bool setErrorType(ErrorType errorType);
        void setSource(Source source);

        uint8_t getStatus(){return _currentStatus;}
    private:
        CAN* _databus;
        uint16_t _canStatusReqID;
        Status _currentStatus;
        Source _source;
        ErrorType _errorType;
        void _send();
        Timer _heartbeatTimer; //Timer for unit heartbeat status messages
};