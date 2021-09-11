#ifndef SOUNDBUZZER_H
#define SOUNDBUZZER_H

#include "mbed.h"

class soundBuzzer{
    private:
        //Signal variables
        const int *SIGNAL_TIME;
        const uint8_t *SIGNAL_LEVEL;
        uint8_t SIGNAL_LENGTH;
        bool playingSignal;
        Timer playingSignalTimer;
        uint8_t playingIndex;
        bool signalSet;
        bool temp;
    public:
        soundBuzzer(bool temp);
        bool playSignal(const int *SIGNAL_TIME_in, const uint8_t *SIGNAL_LEVEL_in, const uint8_t SIGNAL_LENGTH_in, bool forcePlay);
        void updateSignal();

};

#endif //SOUNDBUZZER_H