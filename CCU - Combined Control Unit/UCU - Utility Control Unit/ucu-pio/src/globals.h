#ifndef GLOBALS_H
#define GLOBALS_H

#include "mbed.h"
#include "config.h"
#include "soundBuzzer.h"

extern CAN databus;

extern CANMessage canBuf[canBufSize];
extern uint32_t canBufWriteIndex;

extern DigitalOut BUZZER_EN;
extern PwmOut BUZZER_ON;
extern DigitalOut led;
extern AnalogIn STM_12_INDIC;
//InterruptIn SPEED_SENSOR_STM_IN(A4);
extern AnalogIn SPEED_SENSOR_STM_IN;

extern soundBuzzer WR_buzzer;

extern const int extMode_SIGNAL_TIME[1];
extern const uint8_t extMode_SIGNAL_LEVEL[1]; 
extern const uint8_t extMode_SIGNAL_LENGTH;
extern const int manMode_SIGNAL_TIME[3];
extern const uint8_t manMode_SIGNAL_LEVEL[3];
extern const uint8_t manMode_SIGNAL_LENGTH;
extern const int statusMode_SIGNAL_TIME[3];
extern const uint8_t statusMode_SIGNAL_LEVEL[3];
extern const uint8_t statusMode_SIGNAL_LENGTH;
extern const int faultState_SIGNAL_TIME[1];
extern const uint8_t faultState_SIGNAL_LEVEL[1];
extern const uint8_t faultState_SIGNAL_LENGTH;
extern const int finishedState_SIGNAL_TIME[5];
extern const uint8_t finishedState_SIGNAL_LEVEL[5];
extern const uint8_t finishedState_SIGNAL_LENGTH;
extern const int start_SIGNAL_TIME[7];
extern const uint8_t start_SIGNAL_LEVEL[7];
extern const uint8_t start_SIGNAL_LENGTH;

#endif //GLOBALS_H