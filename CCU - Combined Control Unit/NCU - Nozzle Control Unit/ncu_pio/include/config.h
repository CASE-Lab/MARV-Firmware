#pragma once
#include "mbed.h"

//All variable definitions need to be constant (avoids linking errors)

//Pins
namespace Pin{
    const PinName ANGLE_SENSOR = PA_0;
    const PinName CAN_RD = PA_11;
    const PinName CAN_TD = PA_12;
    const PinName CAN_RS = PF_1;
    const PinName SENSE_AUTO = PA_1;
    const PinName TMCL_TX = PA_9;
    const PinName TMCL_RX = PA_10;
    const PinName TMCL_DE = D3;
}

// helper functions (constexpr compile time calculated)
constexpr unsigned int CONV_MICRO_STEPS(unsigned int microStep){
    unsigned int log2 = 0;
    unsigned int microstep = microStep;
    while(microstep != 1){                                                  //Command for motor goes from 0-8
        microstep = microstep>>1;                                           //Calculating x in microstep = 2^x to get correct command
        log2++;
    }
    return log2;
}

// constants
namespace Constant {
    constexpr float AUTO_SENSE_SCALING = 1/(3900.0/(3900.0+15000.0))*3.3;   // 1/((R2/(R2+R1)) * VCC
    constexpr float VGAIN_ANGLE_SENSOR = 3.3 * (2.7+6.8)/(6.8);             
    constexpr float ANGLE_DEADBAND = 0.5;
    constexpr std::chrono::microseconds RECIVE_TIMEOUT = 300ms;
}

namespace EncoderConfig{
    //values for controll loop without interrupt
    constexpr uint32_t ANGLE_POLLRATE_US = 3400;                            //fixed value (do not change)
    constexpr uint8_t ANGLE_BUFF_LENGTH = 10;                               //for filter buffer  
    constexpr uint8_t MA_LENGTH = 5;                                        //number of samples to use for moving average 
    constexpr uint8_t ANGLE_AVGS = 4;                                       //how many readings to perform directly after each other                                
}

//New configuration values need to be added to initCmdList to be used
namespace MotorConfig{
    constexpr uint32_t MICROSTEPS = CONV_MICRO_STEPS(32);
    constexpr uint32_t FULLSTEP_RESOLUTION = 200;
    constexpr uint32_t MAX_CURRENT = 255;                                       //valid values are
    constexpr uint32_t STANDBY_CURRENT = 255;
    constexpr uint32_t MAX_ACCELERATION = 5000*MICROSTEPS;                      //pps^2
    constexpr uint32_t MAX_DECELERATION = MAX_ACCELERATION;                     //pps^2
    constexpr uint32_t REVERSE_DIR = 1;                                         //if 1 switches right/left rotation
    constexpr float PULLEY_RATIO = 50.0/26.0;                                   //Ratio between number of teeth on pullies
}
namespace CanConfig{
    constexpr uint32_t BUF_SIZE = 10;
}

namespace CanID{
    constexpr uint16_t ID_CALC(uint16_t nodeID, uint16_t messageID){
        return 32*messageID + nodeID;
    }
    //Nodes:

    constexpr uint16_t NODE_NCU = 5;
    constexpr uint16_t NODE_ACU = 1;
    constexpr uint16_t NODE_RCU = 2;

    //Message ID (for common messages):

    constexpr uint16_t MSG_HEARTBEAT = 40;
    constexpr uint16_t MSG_STEERING =  1;
    constexpr uint16_t MSG_LOG1 =  30;

    //Arbitration IDs:

    //Reciving

    constexpr uint16_t NCU_HEARTBEAT = ID_CALC(NODE_NCU, MSG_HEARTBEAT);
    constexpr uint16_t ACU_STEERING = ID_CALC(NODE_ACU, MSG_STEERING);
    constexpr uint16_t RCU_STEERING = ID_CALC(NODE_RCU, MSG_STEERING);

    //Sending:

    constexpr uint16_t OCU_LOG1 = ID_CALC(NODE_NCU, MSG_LOG1);

    //Filters

    constexpr uint32_t MASK_MSG = 0b11111100000;
    constexpr uint32_t MASK_NODE = 0b00000011111;
    constexpr uint32_t MASK_ARB = 0xFFFFFFFF;

    constexpr uint32_t FILTER_NODE(uint16_t nodeID){
        return nodeID;
    }

    constexpr uint32_t FILTER_MSG(uint16_t msgID){
        return msgID*32;
    }

    constexpr uint32_t FILTER_STEERING = FILTER_MSG(1);

}