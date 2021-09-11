#include "indicator.h"

Indicator::Indicator(const PinName red, const PinName green, const PinName blue, chrono::milliseconds blinktime) :
_ledRed(red),
_ledGreen(green),
_ledBlue(blue),
_onColor (&IndicatorColor::OFF),
_offColor (&IndicatorColor::OFF),
_on (false),
_blinkTime (blinktime){
    _blinker.start();
}

void Indicator::setColor(const Color* color){
    _onColor = color;
}

void Indicator::poll(){
    //check if timer is done
    if(_blinker.elapsed_time() > _blinkTime/2){
        _blinker.reset();
        _on = !_on;
        if(_on){
            _writeColor(_onColor);
        }
        else{
            _writeColor(_offColor);
        }
    }
}

void Indicator::_writeColor(const Color* color){
    _ledRed = color->red/255.0;
    _ledGreen = color->green/255.0;
    _ledBlue = color->blue/255.0;
}

void Indicator::setBlinkrate(chrono::milliseconds blinktime){
    _blinkTime = blinktime;
}