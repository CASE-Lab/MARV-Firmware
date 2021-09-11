#pragma once

#include "mbed.h"

class Indicator{
    public:
    Indicator(const PinName red, const PinName green, const PinName blue, chrono::milliseconds blinktime);

    struct Color
    {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

    void setColor(const Color* color);
    void setBlinkrate(chrono::milliseconds blinktime);
    void poll();
    

    private:
    PwmOut _ledRed;
    PwmOut _ledGreen;
    PwmOut _ledBlue;

    const Color* _onColor;
    const Color* _offColor;
    bool _on;
    chrono::milliseconds _blinkTime;

    Timer _blinker;

    void _writeColor(const Color* color);
};

//predefined colors:
namespace IndicatorColor{
    constexpr Indicator::Color OFF = {.red = 0, .green = 0, .blue = 0};
    constexpr Indicator::Color RED = {.red = 255, .green = 0, .blue = 0};
    constexpr Indicator::Color GREEN = {.red = 0, .green = 255, .blue = 0};
    constexpr Indicator::Color BLUE = {.red = 0, .green = 0, .blue = 255};
    constexpr Indicator::Color ORANGE = {.red = 255, .green = 10, .blue = 0};
    constexpr Indicator::Color WHITE = {.red = 255, .green = 150, .blue = 150};
}