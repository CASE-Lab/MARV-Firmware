#pragma once
#include "config.h"
#include "heartbeat.h"
#include "unitMonitor.h"
#include "indicator.h"

class Controller{
    public:
    //constructor takes pointer to tmcl driver
    Controller(Heartbeat* heartbeat, AnalogIn* senseAuto, Indicator* led);
    
    //public enums:
    enum State{                                                     //tracks controller state in relation to 12V auto
        STANDBY, ENABLED, STARTING, RUNNING, ERROR
    };
    enum Dir{
        RIGHT, LEFT
    };

    //reference source indicates source of controller reference signal
    enum RefSource{
        NONE = 0, RCU = 1, ACU = 2, TESTING = 3
    };

    bool setRefSource(RefSource source);
    void pollState();                                               //checks 12V auto and updates state

    void sysKill();

    //get funtions:
    float getSenseAutoFiltered(){return _senseAutoFiltered;}
    bool getAuto12V();                                              //returns true if 12V Auto is on

    //track state of other units
    UnitMonitor unitNCU, unitTCU, unitACU, unitRCU;
  

    private:
    State _state;
    AnalogIn* _senseAuto;
    float _senseAutoFiltered;
    RefSource _refSource;
    Heartbeat* _heartbeat;
    DigitalOut _autoEn;
    DigitalOut _systemKill;
    Indicator* _led;


    Timer _filterTimer;
    Timer _refSourceTimeout;                //tracks timeout time for current angle reference source


    //constants:
    const float _filterTimeConstant = 4;
    const float _senseAutoThreshold = 10;                            //measured in volts
    const float _senseAutoHyst = 1;

    //private functions
    void _setState(State state);
    bool _enableAuto(RefSource source);
};
