#include "globals.h"
#include "mbed.h"
#include "config.h"
#include "soundBuzzer.h"

//CAN
CAN databus(PA_11, PA_12, 1000000);

//CAN RX
CANMessage canBuf[canBufSize];
uint32_t canBufWriteIndex=0;

//UCU Pin definitions
DigitalOut BUZZER_EN(PA_4);
PwmOut BUZZER_ON(PA_8);
DigitalOut led(LED1);
AnalogIn STM_12_INDIC(PB_0);
//InterruptIn SPEED_SENSOR_STM_IN(A4);
AnalogIn SPEED_SENSOR_STM_IN(PB_1);

soundBuzzer WR_buzzer(true);

//Sound signals
//EXT mode on signal
const int extMode_SIGNAL_TIME[1] = {350};
const uint8_t extMode_SIGNAL_LEVEL[1] = {1}; 
const uint8_t extMode_SIGNAL_LENGTH = 1;
//MAN mode on signal
const int manMode_SIGNAL_TIME[3] = {115,115,115};
const uint8_t manMode_SIGNAL_LEVEL[3] = {1,0,1};
const uint8_t manMode_SIGNAL_LENGTH = 3;
//Unit status change signal
const int statusMode_SIGNAL_TIME[3] = {50,50,50};
const uint8_t statusMode_SIGNAL_LEVEL[3] = {1,0,1};
const uint8_t statusMode_SIGNAL_LENGTH = 3;
//Scenario FAULT signal
const int faultState_SIGNAL_TIME[1] = {1000};
const uint8_t faultState_SIGNAL_LEVEL[1] = {1};
const uint8_t faultState_SIGNAL_LENGTH = 1;
//Scenario FINISHED signal
const int finishedState_SIGNAL_TIME[5] = {75,75,75,75,150};
const uint8_t finishedState_SIGNAL_LEVEL[5] = {1,0,1,0,1};
const uint8_t finishedState_SIGNAL_LENGTH = 5;
//Scneario START signal
const int start_SIGNAL_TIME[7] = {500,500,500,500,500,500,1000};
const uint8_t start_SIGNAL_LEVEL[7] = {1,0,1,0,1,0,1};
const uint8_t start_SIGNAL_LENGTH = 7;

