#pragma once
#include "mbed.h"
#include "heartbeat.h"

class ThrottleController{
    public:
    //constructor
    ThrottleController(Heartbeat* heartbeat, AnalogIn* senseAuto);

    //public enums

    enum RefSource{
        NONE, RCU, ACU, TESTING
    };

    enum State{
        STANDBY, STARTING, RUNNING, ERROR
    };

    void pollSpeedUpdate();
    void pollState();
    bool setThrottle(uint8_t* msgData, RefSource source);   //msgData length is assumed to be 4
    bool writeThrottle();

    //get functions

    float getApsOut(){return _aps1Var;}
    float getApsFCOut(){return _fcOutVar;}
    float getRpsOut(){return _rpsVar;}
    float getApsIn(){return _aps1In.read();}
    float getApsFCIn(){return _apsOcIn.read();}
    float getRpsIn(){return _rpsIn.read();}
    float getSenseAutoFilt(){return _senseAutoFiltered;}
    //returns true if 12V auto is activated, otherwise false
    bool getAuto12V(){return _state == State::RUNNING;}

    private:
    //*******INPUT*********
    AnalogIn _aps1In;
    AnalogIn _apsOcIn;
    AnalogIn _rpsIn;
    //******OUTPUT**********
    AnalogOut _apsOut;
    AnalogOut _ocOut;
    AnalogOut _rpsOut;

    Heartbeat* _heartbeat;
    AnalogIn* _senseAuto;

    Timer _refSourceTimeout;
    State _state;
    RefSource _refSource;

    void _setState(State state);

    //Throttle Control Variables:

    float _aps1Var;
    float _fcOutVar;
    float _rpsVar;
    bool _speedUpdated;

    float _aps1InV;
    float _apsOcInV;
    float _rpsInV;

    uint8_t _canCommand[4];                     //stores latest CAN command
    
    void _setSpeed();
    void _ocCtrl();
    void _setRps();

    //12V Auto reading variables
    Timer _filterTimer;
    float _senseAutoFiltered;
    const float _filterTimeConstant = 2;
    const float _senseAutoThreshold = 10;       //measured in volts
    const float _senseAutoHyst = 1;

};