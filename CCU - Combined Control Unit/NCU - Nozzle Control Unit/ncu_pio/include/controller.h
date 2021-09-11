#pragma once
#include "tmcm.h"
#include "config.h"
#include "angleSensor.h"
#include "heartbeat.h"

class Controller{
    public:
    //constructor takes pointer to tmcl driver
    Controller(Heartbeat* heartbeat, AngleSensor* encoder, Tmcm* driver, AnalogIn* senseAuto, TmcmCommand* initCmdList, uint8_t nrOfCmds);
    
    //public enums:
    enum State{                                                     //tracks controller state in relation to 12V auto
        STANDBY, STARTING, RUNNING, ERROR
    };
    enum Dir{
        RIGHT, LEFT
    };

    //reference source indicates source of controller reference signal
    enum RefSource{
        NONE, RCU, ACU, TESTING
    };

    //control loop reltaed functions
    bool setTargetAngle(float targetAngle, RefSource source);
    void pollState();                                               //checks 12V auto and updates state
    void runController();
    
    //motor driver related
    void setTurnTest(bool turnTest){_turnTest = turnTest;}
    int configMotor(TmcmCommand* cmdArray, uint8_t nrOfCmds);       //motor configuration - needs to run every restart
    int readDriverVelocity();                                       //return read status
    void cmdVelocity(uint32_t velocity, Dir direction);

    //encoder realated functions:
    float readEncVolt();
    void updateAngle();
    void pollAngle();                                               //poll to update angle filter (strict timing needed)

    //get funtions:
    float getCurrentEncVel(){return _currentEncoderVelocity;}
    float getCurrentAngleRaw(){return _currentAngleRaw;}
    float getCurrentAngle(){return _currentAngle;}
    int getDriverVelocity(){return _driverVelocity;}                //gets motor driver velocity (can be diffrent from actual velocity)
    float getSenseAutoFiltered(){return _senseAutoFiltered;}
    float getTargetAngle(){return _targetAngle;}

    private:
    AngleSensor* _encoder;
    Tmcm* _driver;
    State _state;
    AnalogIn* _senseAuto;
    TmcmCommand* _initCmdList;
    uint8_t _nrOfCmds;
    float _senseAutoFiltered;
    RefSource _refSource;
    Heartbeat* _heartbeat;
    int _driverVelocity;

    Timer _filterTimer;
    Timer _refSourceTimeout;                //tracks timeout time for current angle reference source
    bool _turnTest;
    const uint32_t _testVel = 2000;

    //controller variables
    float _angleError;
    float _targetAngle;
    float _currentAngle;
    float _currentAngleRaw;
    float _currentEncoderVelocity;

    //constants:
    const float _filterTimeConstant = 2;
    const float _senseAutoThreshold = 10;                            //measured in volts
    const float _senseAutoHyst = 1;

    const float _gainP = MotorConfig::MICROSTEPS * 43;

    //private functions
    void setState(State state);
};
