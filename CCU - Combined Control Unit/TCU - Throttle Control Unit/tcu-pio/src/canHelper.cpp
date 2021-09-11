#include "canHelper.h"
#include "config.h"
#include "ct.h"


bool canDecoder(CANMessage* msg, ThrottleController* controller){
    uint16_t id = msg->id;
    switch (id)
    {
        case CanID::RCU_STEERING :{
            controller->setThrottle(msg->data, ThrottleController::RefSource::RCU);
            }break;

        case CanID::ACU_STEERING :{
            controller->setThrottle(msg->data, ThrottleController::RefSource::ACU);
            }break;

        default:
            break;
    }
    return false;
}

namespace Cansend{
    void log1(CAN* bus, float apsRaw, float rpsRaw, float fcRaw, float aps, float rps, uint8_t fc, bool valid){
        CANMessage canMsg;
        canMsg.id = CT_LOG1_TCU_FRAME_ID;
        canMsg.len = CT_LOG1_TCU_LENGTH;
        ct_log1_tcu_t msg = {
            .log1_tcu_aps_raw_out = ct_log1_tcu_log1_tcu_aps_raw_out_encode(apsRaw),
            .log1_tcu_rps_raw_out = ct_log1_tcu_log1_tcu_rps_raw_out_encode(rpsRaw),
            .log1_tcu_fc_raw_out = ct_log1_tcu_log1_tcu_fc_raw_out_encode(fcRaw),
            .log1_tcu_aps_out = ct_log1_tcu_log1_tcu_aps_out_encode(aps),
            .log1_tcu_rps_out = ct_log1_tcu_log1_tcu_rps_out_encode(rps),
            .log1_tcu_fc_out = fc,
            .log1_tcu_status = (uint8_t)valid
        };
        ct_log1_tcu_pack(canMsg.data, &msg, canMsg.len);

        bus->write(canMsg);
    }

    void log2(CAN* bus, float apsRaw, float rpsRaw, float fcRaw, float aps, float rps, uint8_t fc, bool valid){
        CANMessage canMsg;
        canMsg.id = CT_LOG2_TCU_FRAME_ID;
        canMsg.len = CT_LOG2_TCU_LENGTH;
        ct_log2_tcu_t msg = {
            .log2_tcu_aps_raw_in = ct_log2_tcu_log2_tcu_aps_raw_in_encode(apsRaw),
            .log2_tcu_rps_raw_in = ct_log2_tcu_log2_tcu_rps_raw_in_encode(rpsRaw),
            .log2_tcu_fc_raw_in = ct_log2_tcu_log2_tcu_fc_raw_in_encode(fcRaw),
            .log2_tcu_aps_in = ct_log2_tcu_log2_tcu_aps_in_encode(aps),
            .log2_tcu_rps_in = ct_log2_tcu_log2_tcu_rps_in_encode(rps),
            .log2_tcu_fc_in = fc,
            .log2_tcu_status = (uint8_t)valid
        };
        ct_log2_tcu_pack(canMsg.data, &msg, canMsg.len);

        bus->write(canMsg);
    }
} 
