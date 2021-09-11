#pragma once

#include "mbed.h"

bool waitUntill(Timer* timer, std::chrono::microseconds micros, uint32_t id = 0);

void printFloat(const float number, const char* postString = "", const char* preString = "", const uint32_t scaling = 10000);

void canPrintMsg(CANMessage* msg);