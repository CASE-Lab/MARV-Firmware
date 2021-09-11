#ifndef DISPLAY_H
#define DISPLAY_H

#include "Types.h"
#include "mbed.h"
#include "Menu.h"
#include "spk_oled_ssd1305.h"


class Display{
    private:
    public:
        Display();
        void updateDisplay();
        SPKDisplay screen;
};

#endif //DISPLAY_H
