#ifndef CANCOM_H
#define CANCOM_H

#include "mbed.h"
#include "Types.h"
#include <string.h>

class CANCom{
    private:
        //Variables
        uint32_t canBufReadIndex;
        void statusUpdate(uint8_t* UNIT_status,uint8_t data);
    public:
        //Constructor
        CANCom();
        //Functions
        void setup();
        void updateVariables();
                

};

#endif //CANCOM_H