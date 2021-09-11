#include "CANCom.h"
#include "mbed.h"
#include "Types.h"
#include <string.h>
#include <globals.h>
#include "config.h"
#include "CANConfig.h"

void canRxIsr();

//### CAN #########################################
uint32_t canBufReadIndex;

//used to merge/split chars in candata and integer value.
union command {                 
    int16_t value;
    char data[2];
} canCommand;

bool canStatusReq=false;
//##################################################

//Constructor
CANCom::CANCom(){
    this->canBufReadIndex=0;
}

void CANCom::setup(){
    databus.attach(&canRxIsr,CAN::RxIrq);
    databus.filter(CanID::FILTER_SOUND, CanID::MASK_MSG, CANFormat::CANStandard, 0);                                          
}

void canRxIsr(){
    while(databus.read(canBuf[canBufWriteIndex])) {
        canBufWriteIndex++;
        if (canBufWriteIndex==canBufSize)
            canBufWriteIndex=0;
        }
}

void CANCom::updateVariables(){
    //CAN
    //Decode incoming message
    if (canBufReadIndex != canBufWriteIndex) {  //Check if buffer indices are different which indicates that the position at the read index has been filled.
        int signal = 0;
        switch (canBuf[canBufReadIndex].id) {
            case canSoundBuzzer_ID_OCU:
                canCommand.data[0]=canBuf[canBufReadIndex].data[0];
                signal = (uint8_t)canCommand.data[0];
                
                if(signal == 0x1){
                    WR_buzzer.playSignal(statusMode_SIGNAL_TIME, statusMode_SIGNAL_LEVEL, statusMode_SIGNAL_LENGTH, false);
                    printf("sound\n");
                }
                if(signal == 0x2){
                    WR_buzzer.playSignal(faultState_SIGNAL_TIME, faultState_SIGNAL_LEVEL, faultState_SIGNAL_LENGTH, true);
                    printf("sound2\n");
                }
                if(signal == 0x3){
                    WR_buzzer.playSignal(finishedState_SIGNAL_TIME, finishedState_SIGNAL_LEVEL, finishedState_SIGNAL_LENGTH, false);
                    printf("sound3\n");
                }
                if(signal == 0x4){
                    WR_buzzer.playSignal(start_SIGNAL_TIME, start_SIGNAL_LEVEL, start_SIGNAL_LENGTH, false);
                    printf("sound4\n");
                }

                break;
            case canSoundBuzzer_ID_ACU:
                canCommand.data[0]=canBuf[canBufReadIndex].data[0];
                signal = (uint8_t)canCommand.data[0];
                
                if(signal == 0x1){
                    WR_buzzer.playSignal(statusMode_SIGNAL_TIME, statusMode_SIGNAL_LEVEL, statusMode_SIGNAL_LENGTH, false);
                    printf("sound\n");
                }
                if(signal == 0x2){
                    WR_buzzer.playSignal(faultState_SIGNAL_TIME, faultState_SIGNAL_LEVEL, faultState_SIGNAL_LENGTH, false);
                    printf("sound2\n");
                }
                if(signal == 0x3){
                    WR_buzzer.playSignal(finishedState_SIGNAL_TIME, finishedState_SIGNAL_LEVEL, finishedState_SIGNAL_LENGTH, false);
                    printf("sound3\n");
                }
                if(signal == 0x4){
                    WR_buzzer.playSignal(start_SIGNAL_TIME, start_SIGNAL_LEVEL, start_SIGNAL_LENGTH, false);
                    printf("sound4\n");
                }
                
                break;
            default: //Not a message to OCU
                break;
        }
        if(++canBufReadIndex == canBufSize) //Increment buffer index and check for 'wrap around'.
            canBufReadIndex = 0;
    }
}

/*
void CANCom::statusUpdate(uint8_t* UNIT_status,uint8_t data){
    if(data >= 0 && data <=2){
         *UNIT_status = data;
    }
}*/