#pragma once
namespace CanConfig{
    constexpr uint8_t BUF_SIZE = 10;
    constexpr uint32_t BITRATE = 1000000;
}

namespace CanID{
    constexpr uint16_t ID_CALC(uint16_t nodeID, uint16_t messageID){
        return 32*messageID + nodeID;
    }
    //Nodes:

    constexpr uint16_t NODE_TCU = 4;
    constexpr uint16_t NODE_ACU = 1;
    constexpr uint16_t NODE_RCU = 2;

    //Message ID (for common messages):

    constexpr uint16_t MSG_HEARTBEAT = 40;
    constexpr uint16_t MSG_STEERING =  1;
    constexpr uint16_t MSG_LOG1 =  30;

    //Arbitration IDs:

    //Reciving

    constexpr uint16_t ACU_STEERING = ID_CALC(NODE_ACU, MSG_STEERING);
    constexpr uint16_t RCU_STEERING = ID_CALC(NODE_RCU, MSG_STEERING);

    //Sending:
    constexpr uint16_t TCU_HEARTBEAT = ID_CALC(NODE_TCU, MSG_HEARTBEAT);
    constexpr uint16_t TCU_LOG1 = ID_CALC(NODE_TCU, MSG_LOG1);

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
namespace Pin{
    constexpr PinName APS1_IN = PA_3;
    constexpr PinName APS_OC_IN = PA_1;
    constexpr PinName RPS_IN = PA_0;

    constexpr PinName APS_OUT = PA_6;
    constexpr PinName OC_OUT = PA_5;
    constexpr PinName RPS_OUT = PA_4;

    constexpr PinName CAN_RS = PF_1;
    constexpr PinName SENSE_AUTO = PB_0;
}

namespace Constant {
    constexpr float AUTO_SENSE_SCALING = 1/(3900.0/(3900.0+15000.0))*3.3*0.85475;   // 1/((R2/(R2+R1)) * VCC * MAGIC NUMBER (not correct scale otherwise)        
    constexpr std::chrono::microseconds RECIVE_TIMEOUT = 300ms;
    constexpr std::chrono::microseconds DEBUG_INTERVAL = 500ms;
    constexpr std::chrono::microseconds CANLOG_INTERVAL = 50ms;
}

namespace Throttle{
    constexpr float REFSCALE = 1.0/3300;    
    constexpr float ANALOGREFOLD = 3300.0/5000;
    constexpr float OUT_CALIB = 1.03;          //derived from measuring system output
    constexpr float ANALOGREF = 4700.0/6800;
    constexpr float ANALOGREADREF = 3900.0/6800;

    constexpr uint32_t APSCLOSED = 595;         //mV
    //constexpr uint32_t APSOPEN = 4040;          //mV (should be but new measurements shows otherwise)
    constexpr uint32_t APSOPEN = 3100;          //mV
    constexpr uint32_t APSTHRESHOLD = 1340;     //mV
    constexpr uint32_t APSMIN = 658;            //mV
    constexpr uint32_t APSMAX = 4165;           //mV 
    constexpr uint32_t RPSMIN = 689;            //mV 
    constexpr uint32_t RPSMAX = 4173;           //mV

    constexpr uint8_t FC_UNDEFINED = 0;
    constexpr uint8_t FC_OPEN = 1;
    constexpr uint8_t FC_CLOSED = 2;
    //Scale to value 0-1
    constexpr float decScale (float in){
        return in*REFSCALE;
    }

    // scale committed by analog, hardware electronics.
    constexpr float analogScale(float in){
        return in*ANALOGREF;
    }

    constexpr float apsMilliVolt2percent(float aps){
        return ((aps - APSMIN) * 100) / (APSMAX - APSMIN);
    }

    constexpr float rpsMilliVolt2percent(float rps){
        return ((rps - RPSMIN) * 100) / (RPSMAX - RPSMIN);
    }
    
    constexpr uint8_t fcMilliVolt2State(float fc){
        if (fc > APSTHRESHOLD){
            return FC_CLOSED;
        }
        else{
            return FC_OPEN;
        }
    }


    //For input values only
    //implement for output if needed (too visualize output) 
    // yet to be smart
    constexpr float smartScale(float in){
        float out = analogScale(in);
        return decScale(out);
    }

    //constexpr calculated constants
    constexpr float apsClosed = smartScale(APSCLOSED); 
    constexpr float apsOpen = smartScale(APSOPEN);
    constexpr float apsThreshold = smartScale(APSTHRESHOLD);
    constexpr float apsMax = smartScale(APSMAX);
    constexpr float apsMin = smartScale(APSMIN);
    constexpr float rpsMax = smartScale(RPSMAX);
    constexpr float rpsMin = smartScale(RPSMIN);

    //additional constants
    constexpr float hyster = 0.01;
}