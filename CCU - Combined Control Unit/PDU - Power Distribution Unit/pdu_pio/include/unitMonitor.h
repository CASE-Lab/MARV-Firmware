#pragma once

#include "mbed.h"

class UnitMonitor{
    public:
        UnitMonitor();

        enum State{
            ERROR = 0, OK = 1, NOT_CONNECTED = 2
        };

        enum Source{
        NONE = 0, RCU = 1, ACU = 2, TESTING = 3
        };


        State getState(){return _state;}
        Source getSource(){return _source;}
        void poll();
        void setState(State state);
        void setSource(Source source){_source = source;}

    private:
        State _state;
        Source _source;
        Timer _timeout;
};

