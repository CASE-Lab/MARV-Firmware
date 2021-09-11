#pragma once
#include "mbed.h"
#include "config.h"
#include "RingBufCPP.h"

namespace Rty{
    constexpr float CENTRE_VOLTAGE = 2.5;   // voltage in range 0.5 to 4.5 V (absolute - )
    constexpr float DEG_PER_V = 22.5;       // 4 V with 90 deg total range of motion
}

class AngleSensor{
    public:
    AngleSensor(PinName sensorPin, const float voltageGain);

    //poll based sensor reading (reads adc and shifts angle value into buffer)
    void initSensor();
    //reads from sensors readings number of times and avrages, also shifts value into angle buffer for filtering
    void pollSensor(uint8_t readings = EncoderConfig::ANGLE_AVGS);      

    //Read functions:
    float readVoltage(){return _sensor.read_voltage();}
    float readAngle(){return (Rty::CENTRE_VOLTAGE - _sensor.read_voltage()) * Rty::DEG_PER_V + _angleOffset;}
    uint16_t readADC(){return _sensor.read_u16();}

    //Angular velocity estimation testing:
    float getHoloboro5();

    //Filtered angle
    float getAngleMA();

    //get functions:
    float getAngle(){return _angle;}
    
    //set functions:
    void setAngleOffset(float offset){_angleOffset = offset;}           //sets angle calibrations offest

    private:
    AnalogIn _sensor;
    float _angleOffset;             //calibration offset for angle sensor

    float _angle;
    float _angleFiltered;
    RingBufCPP<float, EncoderConfig::ANGLE_BUFF_LENGTH> _angleBuffer;

    //calculate filter applied to ring buffer (only single sided backward time)
    float _filterFIR(const float* filter, const uint32_t filterLen, const float dtSec);
    float _dtMicros;                    //sensor poll period (for filter calculation)
};