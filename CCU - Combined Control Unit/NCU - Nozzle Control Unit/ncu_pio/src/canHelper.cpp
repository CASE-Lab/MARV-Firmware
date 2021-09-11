#include "canHelper.h"
#include "ct.h"


bool canDecoder(CANMessage* msg, Controller* controller){
    uint16_t id = msg->id;
    switch (id)
    {
        case CanID::RCU_STEERING :{
            int16_t rawAngle = (msg->data[5] | msg->data[4] << 8);
            float angle = 0.01*rawAngle;
            controller->setTargetAngle(angle, Controller::RefSource::RCU);
            }break;

        case CanID::ACU_STEERING :{
            int16_t rawAngle = (msg->data[5] | msg->data[4] << 8);
            float angle = 0.01*rawAngle;
            controller->setTargetAngle(angle, Controller::RefSource::ACU);
            }break;

        default:
            break;
    }
    return false;
}

namespace Cansend{
    void log1(CAN* bus, float angle, float angleRef, float filtVel, float drvVel){
        CANMessage canMsg;
        canMsg.id = CT_LOG1_NCU_FRAME_ID;
        canMsg.len = CT_LOG1_NCU_LENGTH;
        ct_log1_ncu_t msg = {
            .log1_ncu_angle = ct_log1_ncu_log1_ncu_angle_encode(angle),
            .log1_ncu_angle_ref = ct_log1_ncu_log1_ncu_angle_ref_encode(angleRef),
            .log1_ncu_filt_vel = 0,  //not implemented
            .log1_ncu_drv_vel = 0  /*not implemented*/
        };
        ct_log1_ncu_pack(canMsg.data, &msg, canMsg.len);

        bus->write(canMsg);
    }
}