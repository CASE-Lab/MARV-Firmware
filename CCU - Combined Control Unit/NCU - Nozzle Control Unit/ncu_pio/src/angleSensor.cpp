#include "angleSensor.h"

AngleSensor::AngleSensor(PinName sensorPin, const float voltageGain):
    _sensor(sensorPin, voltageGain),
    _angleOffset (-13.5){
    }

void AngleSensor::initSensor(){
    _angle = 0;
    _angleFiltered = 0;

    //init filter buffer to zero
    for(int i = 0; i  < EncoderConfig::ANGLE_BUFF_LENGTH; i++){
        _angleBuffer.add(0);
    }
}

void AngleSensor::pollSensor(uint8_t readings){
    _angle = 0;
    for(int i = 0; i < readings; i++){
        _angle += readAngle();
    }

    _angle = _angle/readings;   //divide by number of readings to get avrage

    //push value into bufffer with overwrite (deletes oldest value)
    _angleBuffer.add(_angle, true);
}

float AngleSensor::getAngleMA(){
    float sum = 0;
    float *data;
    for(int i = 0; i < EncoderConfig::MA_LENGTH; i++){
        //verify that pointer returned by peek isn't null
        if( (data = _angleBuffer.peek(i)) ){
            sum += *data;
        }
        else{
            error("ERROR: angleBuffer out of bounds\n\r");
        }
    }
    return sum/EncoderConfig::MA_LENGTH;
}

float AngleSensor::getHoloboro5(){
    //const float c_k5[] = {1, 3, 2, -2, -3, -1};
    const float c_k7[] = {1,5,9,5,-5,-9,-5,-1};
    constexpr float  dtS = EncoderConfig::ANGLE_POLLRATE_US * 1e-6;
    constexpr uint32_t filterLen = sizeof(c_k7)/sizeof(*c_k7);
    return _filterFIR(c_k7, filterLen, dtS);
}

float AngleSensor::_filterFIR(const float* filter, const uint32_t filterLen, const float dtSec){
    //check that filter length is not longer than buffer
    if( filterLen > _angleBuffer.numElements() ){
        error("ERROR: (in AngleSensor::_filterFIR) filter length > buffer length\n\r");
        return NAN;
    }

    //filter[n] gives X_{0-1}, ie increasing index goes backwards in time
    float filterResult = 0;
   for(uint32_t i = 0; i < filterLen; i++){
       //peek buffer (and derefernce) from newest to older value while applying FIR filter coeeficient and sum values
       filterResult += (*_angleBuffer.peek(EncoderConfig::ANGLE_BUFF_LENGTH - 1 - i)) * filter[i] * 1/dtSec;
   }
   return filterResult;
}

