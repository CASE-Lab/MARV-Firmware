#include "test.h"

#include "mbed.h"
#include "helper.h"
#include "config.h"
#include <math.h>
#include "RingBufCPP.h"

namespace Test{

    void anglePollFilter(AngleSensor* encoder){
        Timer loopTimer;
        int angle, angleMA, angleVel;
        encoder->initSensor();
        while(1){
            loopTimer.start();
            encoder->pollSensor(EncoderConfig::ANGLE_AVGS);
            angle = (int)10000*encoder->getAngle();
            angleMA = (int)10000*encoder->getAngleMA();
            angleVel = (int)encoder->getHoloboro5();

            printf("%d,", angle);
            printf("%d,", angleMA);
            printf("%d\n\r", angleVel);

            //printf("%d.%d,", (int)angle, (int)abs(1000000*(angle - (int)angle)) );
            //printf("%d.%d\n\r", (int)angleMA, (int)abs(1000000*(angleMA - (int)angleMA)) );

            //printing with float
            //printFloat(encoder->getAngle(),",");
            //printFloat(encoder->getAngleMA(),",");
            //printFloat(encoder->getHoloboro5(),"\n\r");

            while(loopTimer.elapsed_time() < 3.7ms){
                wait_us(1);
            }
            loopTimer.stop();
            loopTimer.reset();
        }
    }


    void refTrack(Controller* controller){
        //set up timer for accurate loop delay (20 ms -> 50 Hz)
        Timer loopTime;
        float refAngles[] = {10, -10};
        int drvVel = 0;
        float encVel = 0, encPos = 0, encRaw = 0, encVolt = 0;
        const int nrOfRefAngles = sizeof(refAngles)/sizeof(*refAngles);
        const int pRes = 100000; //float to int resolution conversion for printf

        for(int ref = 0; ref < nrOfRefAngles; ref++){

            controller->setTargetAngle(refAngles[ref], Controller::RefSource::TESTING);

            for(int i = 0; i < 200; i++){
                loopTime.start();

                controller->runController();
                controller->readDriverVelocity();
                controller->updateAngle();
                drvVel = controller->getDriverVelocity();
                encVel = controller->getCurrentEncVel();
                encPos = controller->getCurrentAngle();
                encRaw = controller->getCurrentAngleRaw();
                encVolt = controller->readEncVolt();

                //printout info comma sepperated:
                printf("%d,", drvVel);
                printf("%d.%d,", (int)encVel, (int)abs(pRes*(encVel - (int)encVel)));
                printf("%d.%d,", (int)encPos, (int)abs(pRes*(encPos - (int)encPos)));
                printf("%d.%d,", (int)encRaw, (int)abs(pRes*(encRaw - (int)encRaw)));
                printf("%d.%d,", (int)refAngles[ref], (int)abs(pRes*(refAngles[ref] - (int)refAngles[ref])));
                printf("%d.%d\n\r", (int)encVolt, (int)abs(pRes*(encVolt - (int)encVolt)));

                while(loopTime.elapsed_time() < 20ms){
                    wait_us(10);
                }

                loopTime.stop();
                loopTime.reset();
            }
        }
    }

    void refTrack2(Controller* controller, AngleSensor* encoder, Heartbeat* heartbeat){
        Timer loopTime;
        float refAngles[] = {15, -15};
        constexpr uint8_t nrOfRefAngles = sizeof(refAngles)/sizeof(*refAngles);
        constexpr int ps = 10000;

        for(int ref = 0; ref < nrOfRefAngles; ref++){

            controller->setTargetAngle(refAngles[ref], Controller::RefSource::TESTING);

            for(int i = 0; i < 200; i++){
                loopTime.start();
                controller->pollAngle();
                controller->runController();    //poll state is included in run controller
                //idle time
                waitUntill(&loopTime, 3400us, 1);
                controller->pollAngle();
                //idle time (printing of debug info)
                //angle, encVel, drvVel
                controller->updateAngle();  //for getting updated angle when not running controller
                printf("%d,", (int)(ps*controller->getCurrentAngle()));
                printf("%d,", (int)controller->getCurrentEncVel());
                printf("%d,",(int)ps*controller->getDriverVelocity());
                
                //printFloat(controller->getCurrentAngle(),",");
                //printFloat(controller->getCurrentEncVel(),",");
                //printf("%d\n\r", controller->getDriverVelocity());
                
                waitUntill(&loopTime, 6800us, 2);
                controller->pollAngle();
                waitUntill(&loopTime, 7400us, 3);
                controller->readDriverVelocity();
                waitUntill(&loopTime, 10200us, 4);
                controller->pollAngle();
                //idle time (maybe do CAN logging or simular - switch with printf to lower latency)
                waitUntill(&loopTime, 13600us, 5);
                controller->pollAngle();
                //idle time
                heartbeat->update();
                //testing 12V auto filter
                printf("%d\n\r", (int)(ps*controller->getSenseAutoFiltered()));

                waitUntill(&loopTime, 17000us, 6);
                loopTime.stop();
                loopTime.reset();
            }
        }

    }

    

    void circBuff(){
        int dest;
        RingBufCPP<int, 10> buffer;
        debug("Pushing 1,2,3,4,5,6,7,8,9,10 to buffer\n\r");
        buffer.add(1);
        buffer.add(2);
        buffer.add(3);
        buffer.add(4);
        buffer.add(5);
        buffer.add(6);
        buffer.add(7);
        buffer.add(8);
        buffer.add(9);
        buffer.add(10);
        debug("Peek 0: %d\n\r", *buffer.peek(0));
        debug("Peek 1: %d\n\r", *buffer.peek(1));
        debug("Popping 2 values\n\r");
        buffer.pull(&dest);
        buffer.pull(&dest);
        debug("Peek 0: %d\n\r", *buffer.peek(0));
        debug("Peek 1: %d\n\r", *buffer.peek(1));
        debug("Adding 2 new values\n\r");
        buffer.add(11);
        buffer.add(12);
        debug("Peek 0: %d\n\r", *buffer.peek(0));
        debug("Peek 1: %d\n\r", *buffer.peek(1));
        debug("Adding 2 new values with overwrite true\n\r");
        buffer.add(13, true);
        buffer.add(14, true);
        debug("Peek 0: %d\n\r", *buffer.peek(0));
        debug("Peek 1: %d\n\r", *buffer.peek(1));
        ThisThread::sleep_for(10s);
    }

    void printingFloat(){
        float test = -1;
        for(int i = 0; i < 2000; i++){
            printf("%d,", (int)(1000*test));
            test += 0.001;
        }
        ThisThread::sleep_for(10s);
    }

    void printCanRx(CanHandler* rxHandler){
        while(1){
            while(rxHandler->isEmpty() == false){
                //read and print message
                CANMessage msg;
                if(rxHandler->popMsg(&msg)){
                    printf("%lu ", rxHandler->getRxMissed());
                    canPrintMsg(&msg);
                }
            }
            ThisThread::sleep_for(1s);
        }
    }
}

/*
    //stuipd angle sensor testing (Paste in MAIN)
    debug("Stupid angle sensor testing\n\r");
    ThisThread::sleep_for(10s);
    controller.configMotor(initCmdList, initCmdListLen);
    Timer loopTimer;
    int drvVel = 0;
    float rawAngle = 0;
    int dir;
    while(1){
        //rotate right
        motor.sendRawCommandRetry(TmcmInstruction::ROR, TmcmType::NO_TYPE, 10000);
        dir = 1000;
        for(int i = 0; i < 50; i++){
            loopTimer.start();
            controller.readDriverVelocity();
            drvVel = controller.getDriverVelocity();
            rawAngle = encoder.readAngle();

            printf("%d,%d,",dir, drvVel);
            printf("%d.%d\n\r", (int)rawAngle, (int)abs(10000*(rawAngle - (int)rawAngle)));

            while(loopTimer.elapsed_time() < 10ms){
                    wait_us(10);
            }
            loopTimer.stop();
            loopTimer.reset();
        }

        //rotate left
        motor.sendRawCommandRetry(TmcmInstruction::ROL, TmcmType::NO_TYPE, 10000);
        dir = -1000;
        for(int i = 0; i < 70; i++){
            loopTimer.start();
            controller.readDriverVelocity();
            drvVel = controller.getDriverVelocity();
            rawAngle = encoder.readAngle();

            printf("%d,%d,",dir, drvVel);
            printf("%d.%d\n\r", (int)rawAngle, (int)abs(10000*(rawAngle - (int)rawAngle)));

            while(loopTimer.elapsed_time() < 10ms){
                    wait_us(10);
            }
            loopTimer.stop();
            loopTimer.reset();
        }
    }
*/