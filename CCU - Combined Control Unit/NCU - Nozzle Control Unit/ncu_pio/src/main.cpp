// Drive By Wire - Nozzle Control Unit (NCU)

#include "mbed.h"
#include <math.h>

#include "config.h"
#include "tmcm.h"
#include "controller.h"
#include "angleSensor.h"
#include "test.h"
#include "helper.h"
#include "canHandler.h"
#include "canHelper.h"
#include "heartbeat.h"

static BufferedSerial pc(USBTX,USBRX,460800);                           // Debug serial port over USB. Tx,Rx
CAN databus(Pin::CAN_RD, Pin::CAN_TD,1000000);                          // CAN Interface for databus 2 (communication between actuators/ICU).
DigitalOut RsPin(Pin::CAN_RS);                                          // CAN Tranceiver slew rate limitation
AnalogIn angleSensor(Pin::ANGLE_SENSOR);                                // Angle from angle sensor (0.5-4.5)
AnalogIn senseAuto(Pin::SENSE_AUTO, Constant::AUTO_SENSE_SCALING);      // Sense line for 12V_AUTO

//Can heartbeat
Heartbeat canHeartbeat(&databus, CanID::NCU_HEARTBEAT);

//init config for motor
TmcmCommand initCmdList[] { 
    {TmcmInstruction::MST, TmcmType::NO_TYPE, TmcmConfig::NO_VALUE},
    {TmcmInstruction::SAP, TmcmType::AP_MAXIMUM_CURRENT, MotorConfig::MAX_CURRENT},
    {TmcmInstruction::SAP, TmcmType::AP_STANDBY_CURRENT, MotorConfig::STANDBY_CURRENT},
    {TmcmInstruction::SAP, TmcmType::AP_FULLSTEP_RESOLUTION, MotorConfig::FULLSTEP_RESOLUTION},
    {TmcmInstruction::SAP, TmcmType::AP_MAXIMUN_ACCELERATION, MotorConfig::MAX_ACCELERATION},
    {TmcmInstruction::SAP, TmcmType::AP_MAXIMUM_DECELERATION, MotorConfig::MAX_DECELERATION},
    {TmcmInstruction::SAP, TmcmType::AP_REVERSE_SHAFT, MotorConfig::REVERSE_DIR},
    {TmcmInstruction::SAP, TmcmType::AP_MICROSTEP_RESOLUTION, MotorConfig::MICROSTEPS}
};

constexpr unsigned int initCmdListLen = sizeof initCmdList / sizeof initCmdList[0];

int main()
{
    printf("####### NCU INIT #######\n\r");
    ThisThread::sleep_for(3s);
    Tmcm motor(PA_9, PA_10, D3, TmcmConfig::BAUD, 1, 2);
    AngleSensor encoder(Pin::ANGLE_SENSOR, Constant::VGAIN_ANGLE_SENSOR);
    encoder.initSensor();

    // setup CAN
    databus.filter(CanID::FILTER_STEERING, CanID::MASK_MSG, CANFormat::CANStandard, 0);
    CanHandler canRxHandler(&databus, true, true);
    //CAN master control register
    uint32_t* CAN_MCR = (uint32_t*)0x40006400;
    //set retry  (ABOM bit in CAN_MCR) to let system reset TEC and REC after bus-off
    *CAN_MCR |= 1U << 6;
    
    // test sending 200 messages and count how many are correct
    unsigned int correctMessageCount = 0;
    for(int i = 0 ; i < 200; i++){
       if(motor.sendRawCommand(TmcmInstruction::SAP, TmcmType::AP_MICROSTEP_RESOLUTION, MotorConfig::MICROSTEPS) == 100){
           correctMessageCount++;
       }
    }

    Controller controller(&canHeartbeat, &encoder, &motor, &senseAuto, initCmdList, initCmdListLen);
    controller.setTurnTest(false);

    debug("Correct message count: %d %%\n\r", correctMessageCount/2);
    ThisThread::sleep_for(1s);

    // while(1){
    //     Test::refTrack2(&controller, &encoder, &canHeartbeat);
    // }

    Timer loopTime;
    constexpr int ps = 10000;
    bool every_other = 0;
    while(1){
        loopTime.start();
        controller.pollAngle();
        controller.runController();    //poll state is included in run controller
        //idle time
        waitUntill(&loopTime, 3400us, 1);
        controller.pollAngle();
        //idle time (printing of debug info)
        //angle, encVel, drvVel
        controller.updateAngle();  //for getting updated angle when not running controller
        //printf("%d,", (int)(ps*controller.getCurrentAngle()));
        //printf("%d,", (int)controller.getCurrentEncVel());
        //printf("%d,",(int)ps*controller.getDriverVelocity());

        //can logging
        if(every_other){
            Cansend::log1(&databus, controller.getCurrentAngle(), controller.getTargetAngle(), 0, 0);
        }
        every_other = !every_other;
        
        waitUntill(&loopTime, 6800us, 2);
        controller.pollAngle();
        waitUntill(&loopTime, 7400us, 3);
        controller.readDriverVelocity();
        waitUntill(&loopTime, 10200us, 4);
        controller.pollAngle();
        //idle time (maybe do CAN logging or simular - switch with printf to lower latency)
        waitUntill(&loopTime, 13600us, 5);
        controller.pollAngle();
        //idle time
        canHeartbeat.update();
        //read in data from can bus
        while ( canRxHandler.isEmpty() == false)
        {
            CANMessage canRx;
            if(canRxHandler.popMsg(&canRx)){
                canDecoder(&canRx, &controller);
            }
        }
        
        //testing 12V auto filter
        //printf("%d\n\r", (int)(ps*controller.getSenseAutoFiltered()));
        waitUntill(&loopTime, 17000us, 6);
        loopTime.stop();
        loopTime.reset();
    }
}

