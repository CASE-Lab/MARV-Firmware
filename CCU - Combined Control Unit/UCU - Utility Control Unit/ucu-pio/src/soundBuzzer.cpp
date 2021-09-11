#include "soundBuzzer.h"
#include "mbed.h"
#include "globals.h"
#include "config.h"

using namespace std;
using namespace std::chrono;

//Signal variables
const int *SIGNAL_TIME;
const uint8_t *SIGNAL_LEVEL;
uint8_t SIGNAL_LENGTH;
bool playingSignal = false;
Timer playingSignalTimer;
uint8_t playingIndex = 0;
bool signalSet = false;

bool temp = false;

soundBuzzer::soundBuzzer(bool temp){
    this->temp = temp;
}

bool soundBuzzer::playSignal(const int *SIGNAL_TIME_in, const uint8_t *SIGNAL_LEVEL_in, const uint8_t SIGNAL_LENGTH_in, bool forcePlay){
    if(!playingSignal || forcePlay){ //Signal playing or forcing new signal
        SIGNAL_TIME = SIGNAL_TIME_in;  SIGNAL_LEVEL =  SIGNAL_LEVEL_in; SIGNAL_LENGTH = SIGNAL_LENGTH_in;
        playingSignal = true;
        playingIndex = 0;
        playingSignalTimer.start();
        BUZZER_ON.period(0.00075f);
        return true;
    }
    else{ //Signal already playing
        return false;
    }
}

void soundBuzzer::updateSignal(){
    if(playingSignal && duration_cast<milliseconds>(playingSignalTimer.elapsed_time()).count() > 0){
        if(!signalSet){ //Set signal to on or off
            signalSet = true;
            if(SIGNAL_LEVEL[playingIndex] == 1){
                BUZZER_ON.write(0.4f);
            }
            else{
                BUZZER_ON.write(0);
            }
        }
        
        if(duration_cast<milliseconds>(playingSignalTimer.elapsed_time()).count() > SIGNAL_TIME[playingIndex]){
            if(playingIndex < SIGNAL_LENGTH-1){ //Move to next index
                playingIndex++;
                playingSignalTimer.reset();
                signalSet = false;
            }
            else{ //Done playing
                playingIndex = 0;
                playingSignalTimer.stop();
                playingSignalTimer.reset();
                BUZZER_ON.write(0);
                signalSet = false;
                playingSignal = false;
                SIGNAL_TIME = 0;  SIGNAL_LEVEL = 0; SIGNAL_LENGTH = 0;
            }
        }
    }
}