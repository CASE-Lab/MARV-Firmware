#pragma once

#include "mbed.h"

bool waitUntill(Timer* timer, std::chrono::microseconds micros, uint32_t id = 0);

bool canBusError(CAN* bus, bool debugEn);