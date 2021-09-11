#pragma once

#include "mbed.h"
#include "throttleController.h"


bool canDecoder(CANMessage* msg, ThrottleController* controller);

namespace Cansend{
    //APS & RPS written to WaveRunner system
    void log1(CAN* bus, float apsRaw, float rpsRaw, float fcRaw, float aps, float rps, uint8_t fc, bool valid);
    //APS & RPS read from WaveRunner handles
    void log2(CAN* bus, float apsRaw, float rpsRaw, float fcRaw, float aps, float rps, uint8_t fc, bool valid);
}