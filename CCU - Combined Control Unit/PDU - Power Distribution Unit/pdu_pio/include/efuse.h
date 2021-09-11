#pragma once

#include <mbed.h>

class EFuse{
    public:
    EFuse(PinName pinPowerGood, PinName pinControl, PinName pinCurrentSense, float currentScaling, chrono::milliseconds resetTime);

    enum State{
        OFF, ON, FAULT
    };

    void enable();
    void disable();
    void reset();
    void poll();
    float readCurrent();
    float getCurrent(){return _current;}
    bool getPowerGood(){return _powerGood;}

    private:
    DigitalIn _powerGood;               // pin for reading power good status of eFuse
    DigitalOut _control;                // pin for enabling/disabling eFuse
    AnalogIn _currentSense;             // pin for reading current through eFuse
    const float _currentScaling;        // scales analog input into current in amps
    State _state;
    const chrono::milliseconds _resetTime;

    Timer _resetTimer;

    float _current;                     // stores last read current value
};

struct EFuses{
    EFuse* dbExtPwr;
    EFuse* dbIntPwr;
    EFuse* acuPwr;
    EFuse* dbIntAutoPwr;
    EFuse* dbExtAutoPwr;
};