#ifndef CAN_CONFIG_H
#define CAN_CONFIG_H

#include "mbed.h"

namespace CanID{
    constexpr uint16_t ID_CALC(uint16_t nodeID, uint16_t messageID){
        return 32*messageID + nodeID;
    }
    //Nodes:

    constexpr uint16_t NODE_NCU = 5;
    constexpr uint16_t NODE_ACU = 1;
    constexpr uint16_t NODE_RCU = 2;
    constexpr uint16_t NODE_PDU = 3;
    constexpr uint16_t NODE_TCU = 4;

    //Message ID (for common messages):

    constexpr uint16_t MSG_HEARTBEAT = 40;

    //Filters

    constexpr uint32_t MASK_MSG = 0b11111100000;
    constexpr uint32_t MASK_NODE = 0b00000011111;

    constexpr uint32_t FILTER_NODE(uint16_t nodeID){
        return nodeID;
    }

    constexpr uint32_t FILTER_MSG(uint16_t msgID){
        return msgID*32;
    }

    constexpr uint32_t FILTER_HEARTBEAT = FILTER_MSG(MSG_HEARTBEAT);
    constexpr uint32_t FILTER_ACU = FILTER_NODE(NODE_ACU);

    //Arbitration IDs:


    //Reciving



    //Sending:



}

#endif //CAN_CONFIG_H