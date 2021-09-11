#include "tempsen.h"

//constructor using memeber init list
TempSen::TempSen(PinName pin, float tempCoeff, float tempOffset) :
    tempCoeff (tempCoeff),
    tempOffset (tempOffset),
    analogTemp(pin){}

float TempSen::readCelsius(){
    tempCelsius = tempCoeff * 3.3 * analogTemp.read() + tempOffset;
    return tempCelsius;
}