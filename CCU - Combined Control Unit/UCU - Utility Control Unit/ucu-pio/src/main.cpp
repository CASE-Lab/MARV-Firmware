#include "mbed.h"
#include "config.h"
#include "globals.h"
#include "CANCom.h"
#include "heartbeat.h"
#include "soundBuzzer.h"

using namespace std;
using namespace std::chrono;

#define signalPeriod 350
#define modeChangeCounterThreshold 10
#define voltageThreshold 5

//Variables
bool toggleMode = false;
int modeChangeCounter=0;
volatile int speedPulses=0;

//Functions
void switchModeBuzzer();
void countSpeedPulses();

int main() {
    printf("####### UCU INIT #######\n\r");
    BUZZER_EN = 1;
    //SPEED_SENSOR_STM_IN.rise(&countSpeedPulses);
    
    CANCom canRX;
    canRX.setup();

    //disable tx
    databus.monitor(true);

    heartbeat unit_status(&databus, canStatusReq_ID_UCU);

    BUZZER_ON.period(0.00075f);
    BUZZER_ON.write(0.2f);    
    thread_sleep_for((double)signalPeriod);
    BUZZER_ON.write(0);
    
    while(1) {
        canRX.updateVariables();

        switchModeBuzzer();
        float temp = SPEED_SENSOR_STM_IN;
        
        //printf("Pin level: %f\n",temp);

        unit_status.update();
        WR_buzzer.updateSignal();

        thread_sleep_for(1);
    }
}

void countSpeedPulses(){
    speedPulses++;
}

void switchModeBuzzer(){
    float systemVoltage = STM_12_INDIC*16;
    
    if(toggleMode){ //In automatic mode, 12V on, this part give the signal function hysterisis
        if(systemVoltage < voltageThreshold && modeChangeCounter < modeChangeCounterThreshold){ //If the system voltage is less than the threshold, count up
            modeChangeCounter++; 
        }
        else if(modeChangeCounter >= modeChangeCounterThreshold){ //If the counter is equal to the threshold, signal and switch mode
            /*BUZZER_ON.period(0.00075f);
            BUZZER_ON.write(0.2f);    
            thread_sleep_for((double)signalPeriod);
            BUZZER_ON.write(0);*/
            WR_buzzer.playSignal(extMode_SIGNAL_TIME, extMode_SIGNAL_LEVEL, extMode_SIGNAL_LENGTH, true);
            
            modeChangeCounter=0;
            toggleMode=false;
        }
        else{ //If the voltage suddenly is higher again, reset the counter
            modeChangeCounter=0;    
        }
    }
    else{ //In manual mode, 12V off
        if(systemVoltage > voltageThreshold && modeChangeCounter < modeChangeCounterThreshold){ //If the system voltage is less than the threshold, count up
            modeChangeCounter++; 
        }
        else if(modeChangeCounter >= modeChangeCounterThreshold){ //If the counter is equal to the threshold, signal and switch mode            
            /*BUZZER_ON.period(0.00075f);
            BUZZER_ON.write(0.2f);    
            thread_sleep_for((double)(signalPeriod/3));
            BUZZER_ON.write(0);
            thread_sleep_for((double)(signalPeriod/3));
            BUZZER_ON.period(0.00075f);
            BUZZER_ON.write(0.2f);    
            thread_sleep_for((double)(signalPeriod/3));
            BUZZER_ON.write(0);*/
            WR_buzzer.playSignal(manMode_SIGNAL_TIME, manMode_SIGNAL_LEVEL, manMode_SIGNAL_LENGTH, true);
            
            modeChangeCounter=0;
            toggleMode=true;
        }
        else{ //If the voltage suddenly is higher again, reset the counter
            modeChangeCounter=0;    
        }
    }
}

