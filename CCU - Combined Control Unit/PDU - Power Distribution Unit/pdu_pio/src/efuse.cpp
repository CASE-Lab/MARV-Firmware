#include "efuse.h"

EFuse::EFuse(PinName pinPowerGood, PinName pinControl, PinName pinCurrentSense, float currentScaling, chrono::milliseconds resetTime) :
    _powerGood(pinPowerGood, PullUp),
    _control(pinControl),
    _currentSense(pinCurrentSense),
    _currentScaling (currentScaling),
    _state (State::OFF),
    _resetTime (resetTime){}

float EFuse::readCurrent(){
    _current =  _currentScaling * _currentSense.read();
    return _current;
}

void EFuse::enable(){
    _control.write(1);
    _state = State::ON;
}

void EFuse::disable(){
    _control.write(0);
    _state = State::OFF;
}

void EFuse::reset(){
    disable();
    _resetTimer.start();
}

void EFuse::poll(){
    //check reset timer
    if(_resetTimer.elapsed_time() > _resetTime){
        enable();
        _resetTimer.stop();
        _resetTimer.reset();
    }
}