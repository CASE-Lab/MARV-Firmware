#pragma once

#include <mbed.h>

// eFuse external data bus pins (TPS25982)
namespace Pin{
    //Data Bus External eFuse (7 A limit):
    constexpr PinName DB_EXT_PG = PC_3;         // Power Good
    constexpr PinName DB_EXT_EN = PA_9;         // Enable/disable control
    constexpr PinName DB_EXT_IMON = PA_1;       // Current monitoring

    //Data Bus Internal eFuse (5 A limit):
    constexpr PinName DB_INT_PG = PA_11;        // Power Good
    constexpr PinName DB_INT_EN = PA_10;        // Enable/disable control
    constexpr PinName DB_INT_IMON = PA_5;       // Current monitoring

    //ACU efuse (15 A limit):
    constexpr PinName ACU_PG = PC_5;           // Power Good
    constexpr PinName ACU_EN = PA_8;           // Enable/disable control
    constexpr PinName ACU_IMON = PA_4;         // Current monitoring

    //Data Bus Internal Auto eFuse (15 A limit):
    constexpr PinName DB_INT_AUTO_PG = PC_4;     // Power Good
    constexpr PinName DB_INT_AUTO_EN = PA_7;     // Enable/disable control
    constexpr PinName DB_INT_AUTO_IMON = PA_6;   // Current monitoring

    //Data Bus External Auto eFuse (5 A limit):
    constexpr PinName DB_EXT_AUTO_PG = PC_2;     // Power Good
    constexpr PinName DB_EXT_AUTO_EN = PC_6;     // Enable/disable control
    constexpr PinName DB_EXT_AUTO_IMON = PA_0;   // Current monitoring

    //Led indicator
    constexpr PinName LED_R = PB_4;
    constexpr PinName LED_G = PB_5;
    constexpr PinName LED_B = PB_2;

    //System functions
    constexpr PinName SYS_KILL = PB_0;
    constexpr PinName RLY_AUTO_EN = PB_10;
    constexpr PinName INT_TEMP = PB_1;

    //12V sense
    constexpr PinName SENSE_12V = PC_0;
    constexpr PinName SENSE_12V_AUTO = PC_1;
}

namespace Constant{
    // ****** eFuse defenitions ******
    constexpr float IGAIN_TPS25982 = 3.3 * 1/(0.000246 * 820);   // Vin * 1/(currentGain * R_IMON)
    constexpr float IGAIN_TPS25940 = 3.3 * 1/(0.000056 * 39000); // Vin * 1/(currentGain * R_IMON)
    constexpr std::chrono::microseconds RECIVE_TIMEOUT = 300ms;
    constexpr std::chrono::microseconds HEARTBEAT_TIMEOUT = 3000ms;
    constexpr float VGAIN_12V = 3.3 * 120.0/20.0;                 //Vref * (R1+R2/R2)
}

namespace CanConfig{
    constexpr uint32_t BUF_SIZE = 10;
    constexpr std::chrono::milliseconds TIMEOUT_10HZ = 1000ms/10;
}

namespace CanID{
    constexpr uint16_t ID_CALC(uint16_t nodeID, uint16_t messageID){
        return 32*messageID + nodeID;
    }
    //Nodes:

    constexpr uint16_t NODE_ACU = 1;
    constexpr uint16_t NODE_RCU = 2;
    constexpr uint16_t NODE_PDU = 3;
    constexpr uint16_t NODE_TCU = 4;
    constexpr uint16_t NODE_NCU = 5;
    constexpr uint16_t NODE_OCU = 7;

    //Message ID (for common messages):

    constexpr uint16_t MSG_HEARTBEAT = 40;
    constexpr uint16_t MSG_STEERING =  1;
    constexpr uint16_t MSG_LOG1 =  30;

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

    constexpr uint32_t FILTER_HEARTBEAT = FILTER_MSG(MSG_HEARTBEAT);
    constexpr uint32_t FILTER_STEERING = FILTER_MSG(MSG_STEERING);

    constexpr uint32_t FILTER_OCU = FILTER_NODE(NODE_OCU);
    constexpr uint32_t FILTER_ACU = FILTER_NODE(NODE_ACU);
    constexpr uint32_t FILTER_RCU = FILTER_NODE(NODE_RCU);

    //Arbitration IDs:


    //Reciving

    constexpr uint16_t NCU_HEARTBEAT = ID_CALC(NODE_NCU, MSG_HEARTBEAT);
    constexpr uint16_t TCU_HEARTBEAT = ID_CALC(NODE_TCU, MSG_HEARTBEAT);
    constexpr uint16_t ACU_HEARTBEAT = ID_CALC(NODE_ACU, MSG_HEARTBEAT);
    constexpr uint16_t RCU_HEARTBEAT = ID_CALC(NODE_RCU, MSG_HEARTBEAT);
    constexpr uint16_t ACU_STEERING = ID_CALC(NODE_ACU, MSG_STEERING);
    constexpr uint16_t RCU_STEERING = ID_CALC(NODE_RCU, MSG_STEERING);
    constexpr uint16_t POWER_PDU = 615;

    //Sending:

    constexpr uint16_t PDU_HEARTBEAT = ID_CALC(NODE_PDU, MSG_HEARTBEAT);
    constexpr uint16_t PDU_LOG1 = ID_CALC(NODE_PDU, MSG_LOG1);
    constexpr uint16_t STATUS_12V_AUTO = 35;
}

namespace CanValue{
    constexpr uint8_t POWER_PDU_SYS_REBOOT = 1;
    constexpr uint8_t POWER_PDU_SHUTDOWN = 2;
    constexpr uint8_t POWER_PDU_DBW_REBOOT = 0;
}
