#include "canHelper.h"
#include "config.h"
#include "controller.h"
#include "ct.h"


bool canDecoder(CANMessage* msg, Controller* controller, EFuses* fuses){
    uint16_t id = msg->id;
    switch (id)
    {
        //Steering command signal from RCU
        case CanID::RCU_STEERING :{
            controller->setRefSource(Controller::RefSource::RCU);
            }break;

        //Steering command signal from ACU
        case CanID::ACU_STEERING :{
            controller->setRefSource(Controller::RefSource::ACU);
            }break;

        //Heartbeat from ACU
        case CanID::ACU_HEARTBEAT :{
            controller->unitACU.setState((UnitMonitor::State)msg->data[0]);
            controller->unitACU.setSource((UnitMonitor::Source)msg->data[2]);
            }break;

        //Heartbeat from RCU
        case CanID::RCU_HEARTBEAT :{
            controller->unitRCU.setState((UnitMonitor::State)msg->data[0]);
            controller->unitRCU.setSource((UnitMonitor::Source)msg->data[2]);
            }break;

        //Heartbeat from NCU
        case CanID::NCU_HEARTBEAT :{
            controller->unitNCU.setState((UnitMonitor::State)msg->data[0]);
            controller->unitNCU.setSource((UnitMonitor::Source)msg->data[2]);
            }break;

        //Heartbeat from ACU
        case CanID::TCU_HEARTBEAT :{
            controller->unitTCU.setState((UnitMonitor::State)msg->data[0]);
            controller->unitTCU.setSource((UnitMonitor::Source)msg->data[2]);
            }break;

        //Power control signal from OCU:
        case CanID::POWER_PDU :{
            switch (msg->data[0]){
                case CanValue::POWER_PDU_DBW_REBOOT:
                    if(controller->getAuto12V()){
                        printf("12V Auto on: reboot blocked\n\r");
                    }
                    else{
                        printf("DBW system rebooting...\n\r");
                        fuses->dbExtPwr->reset();
                        fuses->dbIntPwr->reset();
                    }
                    break;

                case CanValue::POWER_PDU_SYS_REBOOT:
                    if(controller->getAuto12V()){
                        printf("12V Auto on: reboot blocked\n\r");
                    }
                    else{
                        printf("System rebooting...\n\r");
                        //fuses->dbExtPwr->reset();
                        //fuses->dbIntPwr->reset();
                        //fuses->acuPwr->reset();
                        NVIC_SystemReset();
                    }
                    break;

                case CanValue::POWER_PDU_SHUTDOWN:
                    printf("System shuting down...\n\r");
                    controller->sysKill();
                    break;
            }
            }break;

        default:
            //debug("Unhandeled can with id: %lu\n\r", msg->id);
            break;
    }
    return false;
}

namespace Cansend{
    void log1(CAN* bus, EFuses* fuses, float temp){
        CANMessage canMsg;
        canMsg.id = CT_LOG1_PDU_FRAME_ID;
        canMsg.len = CT_LOG1_PDU_LENGTH;
        ct_log1_pdu_t msg = {
            .log1_pdu_db_ext_current = ct_log1_pdu_log1_pdu_db_ext_current_encode(1000*fuses->dbExtPwr->readCurrent()),
            .log1_pdu_db_int_current = ct_log1_pdu_log1_pdu_db_int_current_encode(1000*fuses->dbIntPwr->readCurrent()),
            .log1_pdu_acu_current = ct_log1_pdu_log1_pdu_acu_current_encode(1000*fuses->acuPwr->readCurrent()),
            .log1_pdu_db_int_auto_current = ct_log1_pdu_log1_pdu_db_int_auto_current_encode(1000*fuses->dbIntAutoPwr->readCurrent()),
            .log1_pdu_db_ext_auto_current = ct_log1_pdu_log1_pdu_db_ext_auto_current_encode(1000*fuses->dbExtAutoPwr->readCurrent()),
            .log1_pdu_int_temp = ct_log1_pdu_log1_pdu_int_temp_encode(temp)
        };
        ct_log1_pdu_pack(canMsg.data, &msg, canMsg.len);

        bus->write(canMsg);
    }

    void status12V_Auto(CAN* bus, bool on){
        CANMessage canMsg;
        canMsg.id = CanID::STATUS_12V_AUTO;
        canMsg.len = CT_STATUS12_V_AUTO_LENGTH;
        canMsg.data[0] = on;
        bus->write(canMsg);
    }
}