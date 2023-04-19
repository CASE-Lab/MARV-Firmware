//*
// Drive By Wire - Throttle Control Unit (TCU)
// Originally written by: Axel Gunnarson, supported by the DBW team 2019.
// Updated by: Viktor Lindström, 2021
//*

#include "mbed.h"
#include "config.h"
#include "heartbeat.h"
#include "canHandler.h"
#include "canHelper.h"
#include "throttleController.h"

#define VERSION "1.0"

//Function prototypes
bool waitUntill(Timer* timer, std::chrono::microseconds micros);

// Port/Pin declarations
BufferedSerial pc(USBTX,USBRX,115200);                  // Debug serial port over USB. Tx,Rx
DigitalOut RsPin(Pin::CAN_RS);                          // CAN Tranceiver slew rate limitation

//Sense auto
AnalogIn senseAuto(Pin::SENSE_AUTO, Constant::AUTO_SENSE_SCALING);      // Sense line for 12V_AUTO


void init(){
    printf("####### TCU INIT #######\n\r");
    RsPin = 0; // Disable slew rate limitation (CAN)
    }

int main(){
    Timer loopTimer, serialTimer, canlogTimer;
    init();
    loopTimer.start();
    serialTimer.start();
    canlogTimer.start();

    //read out configuration of CAN master control register
    uint32_t* CAN_MCR = (uint32_t*)0x40006400;
    uint32_t CAN_MCR_RST_VALUE = *CAN_MCR;
    CAN databus(PA_11, PA_12,CanConfig::BITRATE);
    //set retry  (ABOM bit in CAN_MCR) to let system reset TEC and REC after bus-off
    *CAN_MCR |= 1U << 6;

    // Can filter:
    databus.filter(CanID::FILTER_STEERING, CanID::MASK_MSG, CANFormat::CANStandard, 0);

    CanHandler canRxHandler(&databus, true, true);
    Heartbeat canStatus(&databus, CanID::TCU_HEARTBEAT);
    ThrottleController controller(&canStatus, &senseAuto);



    //Main loop
    while(1) {
        //handle incoming CAN
        while ( canRxHandler.isEmpty() == false)
        {
            CANMessage canRx;
            if(canRxHandler.popMsg(&canRx)){
                canDecoder(&canRx, &controller);
            }
        }

        //poll heartbeat
        canStatus.update();

        //poll controller state:
        controller.pollState();

        //check if speed has been updated and write to output variables
        controller.pollSpeedUpdate();

        //write throttle (only sets non zero throttle if state is running)
        controller.writeThrottle();

        //send can logging

        //read aps/rps values & convert to mV from 0-1 analog value
        float apsRawIn = 1.0/Throttle::ANALOGREADREF * 3.3 * 1000 * controller.getApsIn();
        float rpsRawIn = 1.0/Throttle::ANALOGREADREF * 3.3 * 1000 * controller.getRpsIn();
        float fcRawIn = 1.0/Throttle::ANALOGREADREF * 3.3 * 1000 * controller.getApsFCIn();
        float apsRawOut = 1.0/Throttle::ANALOGREF * 3.3 * 1000 * controller.getApsOut();
        float rpsRawOut = 1.0/Throttle::ANALOGREF * 3.3 * 1000 * controller.getRpsOut();
        float fcRawOut = 1.0/Throttle::ANALOGREF * 3.3 * 1000 * controller.getApsFCOut();

        //convert to percentage values
        float apsIn = Throttle::apsMilliVolt2percent(apsRawIn);
        float rpsIn = Throttle::rpsMilliVolt2percent(rpsRawIn);
        uint8_t fcIn = Throttle::fcMilliVolt2State(fcRawIn);
        float apsOut = Throttle::apsMilliVolt2percent(apsRawOut);
        float rpsOut = Throttle::rpsMilliVolt2percent(rpsRawOut);
        uint8_t fcOut = Throttle::fcMilliVolt2State(fcRawOut);
        
        bool autoOn = controller.getAuto12V();

        //log can at 25 Hz
        if(canlogTimer.elapsed_time() > Constant::CANLOG_INTERVAL){
            //Cansend::log1(&databus, apsRawOut, rpsRawOut, fcRawOut, apsOut, rpsOut, fcOut, autoOn);
            Cansend::log2(&databus, apsRawIn, rpsRawIn, fcRawIn, apsIn, rpsIn, fcIn, !autoOn);
            canlogTimer.reset();
        }

        //debug print at specified interval
        if(serialTimer.elapsed_time() > Constant::DEBUG_INTERVAL){
            printf("TCU_"); printf(VERSION); printf(" | \n\r");
            printf("APS Out: %d | ", (int)apsRawOut);
            //printf("APSFC Out: %d | ",(int)rpsRawOut);
            //printf("RPS Out: %d | \n\r", (int)fcRawOut);
            printf("APS In: %d | ", (int)apsRawIn);
            printf("APSFC In: %d | ",(int)fcRawIn);
            //printf("RPS In: %d | \n\r", (int)rpsRawIn);
            //printf("12V: %d mV\n\r", (int)(controller.getSenseAutoFilt()*1000));
            serialTimer.reset();


            printf("\nCAN_MCR: 0x%08x   CAN_MCR_RST: 0x%08x \n", *CAN_MCR, CAN_MCR_RST_VALUE);
        }


        waitUntill(&loopTimer, 10ms);   //run loop at 100 Hz
        loopTimer.reset();
    }
}


//helper functions:
bool waitUntill(Timer* timer, std::chrono::microseconds micros){
    bool missedDeadline = true;
    while(timer->elapsed_time() < micros){
        missedDeadline = false;
        wait_us(1);
        //debug("wait\n\r");
    }

    if(missedDeadline){
        printf("ERROR: missed deadline");
    }

    return missedDeadline;
}
