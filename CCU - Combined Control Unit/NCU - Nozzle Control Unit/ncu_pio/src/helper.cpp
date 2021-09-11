#include "helper.h"

bool waitUntill(Timer* timer, std::chrono::microseconds micros, uint32_t id){
    bool missedDeadline = true;
    while(timer->elapsed_time() < micros){
        missedDeadline = false;
        wait_us(1);
        //debug("wait\n\r");
    }

    if(missedDeadline){
        printf("ERROR: missed deadline");
        //print id if defined (not zero)
        if(id){
            printf(" with id: %lu\n\r", id);
        }
        else{
            printf("\n\r");
        }

    }

    return missedDeadline;
}

void printFloat(const float number, const char* postString, const char* preString, const uint32_t scaling){
    int integer, decimal;
    char sign = 0;  //defualt is no sign
    if(number < 0.0F){
        sign = '-';
    }
    integer = (int)abs(number);
    decimal = (int)(scaling*(number - integer));
    decimal = abs(decimal);
    printf("%s%c%d.%d%s", preString, sign, integer, decimal, postString);
}

void canPrintMsg(CANMessage* msg){
    printf("ID %d : ",msg->id);
    for(int i=0; i<msg->len ; i++)
    {
       printf("%02x ",msg->data[i]);
    }
    printf("\n\r");
}