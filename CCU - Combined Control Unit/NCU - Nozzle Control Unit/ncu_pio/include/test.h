#pragma once

#include "controller.h"
#include "angleSensor.h"
#include "heartbeat.h"
#include "canHandler.h"

namespace Test{
    //testing of polling based angle MA filter
    void anglePollFilter(AngleSensor* encoder);

    //outputs csv of angle sensor filtered values:
    void angleFilter(AngleSensor* encoder);

    //reads stepper motor velocity (open loop from driver)
    void driverVelRead(Controller* controller);

    //void reference tracking with serial output
    void refTrack(Controller* controller);

    //reference tracking without the use of interuppt to see ADC. See README for timing plan
    void refTrack2(Controller* controller, AngleSensor* encoder, Heartbeat* heartbeat);

    //testing functions of mbed circular buffer
    void circBuff();

    void printingFloat();

    void printCanRx(CanHandler* rxHandler);
}