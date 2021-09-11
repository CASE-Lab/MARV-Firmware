#pragma once

#include "mbed.h"
#include "controller.h"
#include "efuse.h"


bool canDecoder(CANMessage* msg, Controller* controller, EFuses* fuses);

namespace Cansend{
    //logs currents from EFuses and internal temperature
    void log1(CAN* bus, EFuses* fuses, float temp);

    void status12V_Auto(CAN* bus, bool on);
}