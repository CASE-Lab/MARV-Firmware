#include <mbed.h>

#include "config.h"     
#include "tempsen.h"
#include "efuse.h"
#include "indicator.h"
#include "controller.h"
#include "canHandler.h"
#include "canHelper.h"
#include "helper.h"

const uint32_t bitrate = 1000000;
CAN databus(PB_8, PB_9,bitrate);                             //Rd, Td physical pin 31 32
BufferedSerial pc(USBTX, USBRX, 115200);

//temp sensors 10 mV/C 500 mV at 0 C -> temp = voltage * 100 + -50
TempSen tempSenInternal(Pin::INT_TEMP, 100.0, -50.0);
AnalogIn vrefint(ADC_VREF);

//Voltage sense
AnalogIn sense12V(Pin::SENSE_12V, Constant::VGAIN_12V);
AnalogIn sense12VAuto(Pin::SENSE_12V_AUTO, Constant::VGAIN_12V);

//Data Bus External eFuse (7 A limit):
EFuse dbExtPwr(Pin::DB_EXT_PG, Pin::DB_EXT_EN, Pin::DB_EXT_IMON, Constant::IGAIN_TPS25982, 500ms);

//Data Bus Internal eFuse (5 A limit):
EFuse dbIntPwr(Pin::DB_INT_PG, Pin::DB_INT_EN, Pin::DB_INT_IMON, Constant::IGAIN_TPS25940, 500ms);

//ACU efuse (15 A limit):
EFuse acuPwr(Pin::ACU_PG, Pin::ACU_EN, Pin::ACU_IMON, Constant::IGAIN_TPS25982, 500ms);

//Data Bus Internal Auto eFuse (15 A limit);
EFuse dbIntAutoPwr(Pin::DB_INT_AUTO_PG, Pin::DB_INT_AUTO_EN, Pin::DB_INT_AUTO_IMON, Constant::IGAIN_TPS25982, 500ms);

//Data Bus External Auto eFuse (5 A limit):
EFuse dbExtAutoPwr(Pin::DB_EXT_AUTO_PG, Pin::DB_EXT_AUTO_EN, Pin::DB_EXT_AUTO_IMON, Constant::IGAIN_TPS25940, 500ms);

//struct storing pointer to all efuses
EFuses fuses{
  .dbExtPwr = &dbExtPwr,
  .dbIntPwr = &dbIntPwr,
  .acuPwr   = &acuPwr,
  .dbIntAutoPwr = &dbIntAutoPwr,
  .dbExtAutoPwr = &dbExtAutoPwr
};

void pollFuses(){
  fuses.dbExtPwr->poll();
  fuses.dbIntPwr->poll();
  fuses.acuPwr->poll();
  fuses.dbIntAutoPwr->poll();
  fuses.dbExtAutoPwr->poll();
}

// various system functinos
DigitalOut systemKill(Pin::SYS_KILL);


int main() {
  printf("PDU Starting\n\r");
  // put your setup code here, to run once:
  Indicator led(Pin::LED_R, Pin::LED_G, Pin::LED_B, 500ms);
  Heartbeat canHeartbeat(&databus, CanID::PDU_HEARTBEAT);
  Controller controller(&canHeartbeat, &sense12VAuto, &led);
  CanHandler canRx(&databus, true, true);

  //can bus filter
  databus.filter(CanID::FILTER_HEARTBEAT, CanID::MASK_MSG, CANFormat::CANStandard, 0);
  databus.filter(CanID::FILTER_STEERING, CanID::MASK_MSG, CANFormat::CANStandard, 1);
  databus.filter(CanID::FILTER_OCU, CanID::MASK_NODE, CANFormat::CANStandard, 2);

  // enable all eFuses after initial delay:
  ThisThread::sleep_for(500ms);
  dbExtPwr.enable();
  acuPwr.enable();
  dbIntPwr.enable();
  dbIntAutoPwr.enable();
  dbExtAutoPwr.enable();

  systemKill = 0;

  //Stops TX from PDU
  databus.monitor(true);

  Timer loopTimer;
  loopTimer.start();

  //can message timer
  Timer can10Hz;
  can10Hz.start();

  // while(1){
  //   CANMessage temp;
  //   databus.read(temp);
  // }

  while (1){
    led.poll();
    canHeartbeat.update();
    controller.pollState();
    pollFuses();
    
    //handle CAN messages
        while ( canRx.isEmpty() == false){
        CANMessage canRxMsg;
        if(canRx.popMsg(&canRxMsg)){
            canDecoder(&canRxMsg, &controller, &fuses);
        }
    }

    //send can messages
    if(can10Hz.elapsed_time() > CanConfig::TIMEOUT_10HZ){
      can10Hz.reset();
      printf("PDU 12V Auto: %d\n\r",  controller.getAuto12V());
      Cansend::status12V_Auto(&databus, controller.getAuto12V());
      Cansend::log1(&databus, &fuses, tempSenInternal.readCelsius());

      //check for can bus error
      //canBusError(&databus, false);
    }

    waitUntill(&loopTimer, 10ms, 1);    //do not change (effects filtering and message rate)
    loopTimer.reset();
  }
}