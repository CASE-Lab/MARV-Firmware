#pragma once

#include "mbed.h"
#include "Menu.h"

namespace fixstackConf{
    constexpr uint8_t STACK_SIZE = 10;
}

class fixstack
{
private:
    /* data */
    Menu* _stackArray[fixstackConf::STACK_SIZE];
    Menu** _currentPos;

public:
    fixstack();
    ~fixstack();

    //stack functions
    void pop();
    void push(Menu* dataIn);
    Menu* top();
    bool empty();
};
