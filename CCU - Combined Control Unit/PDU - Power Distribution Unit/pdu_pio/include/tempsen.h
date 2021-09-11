#pragma once

#include <mbed.h>

/*  TODO:
    Look at adding memmber initlaizer list when adding AnalogIn as a memeber variable
    https://stackoverflow.com/questions/12927169/how-can-i-initialize-c-object-member-variables-in-the-constructor
    https://www.youtube.com/watch?v=1nfuYMXjZsA&t=446s
*/
class TempSen{
    public:
    //constructor:
    TempSen(PinName pin, float tempCoeff, float tempOffset);

    float readCelsius();
    float getCelsius(){return tempCelsius;}

    private:
    const float tempCoeff, tempOffset;
    float tempCelsius;
    AnalogIn analogTemp;
};

