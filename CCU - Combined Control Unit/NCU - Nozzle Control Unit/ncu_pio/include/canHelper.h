#pragma once

#include "mbed.h"
#include "controller.h"


bool canDecoder(CANMessage* msg, Controller* controller);

namespace Cansend{
    void log1(CAN* bus, float angle, float angleRef, float filtVel, float drvVel);
}