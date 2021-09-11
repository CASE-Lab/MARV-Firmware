#include "channel.h"

// define and init programming channels
Channel pdu(Pin::PDU_EN, Pin::PDU_NRST, 1);
Channel tcu(Pin::TCU_EN, Pin::TCU_NRST, 0);
Channel ucu(Pin::UCU_EN, Pin::UCU_NRST, 0);
Channel ncu(Pin::NCU_EN, Pin::NCU_NRST, 0);

const char pduChannel = '4';
const char tcuChannel = '3';
const char ucuChannel = '2';
const char ncuChannel = '1';
const char noChannel = '0';

//variables for async led blink:
const unsigned long BLINK_DELAY_MS = 500;
unsigned long previousTime = 0;
bool ledState = 0;

Channel* activeChannel = NULL;  //NULL pointer indicates no active channel
uint8_t channelNumber = 0;
int serialInput;

//function prototypes
void switchChannel(Channel* channel); //sending NULL sets no channel active
void printInfo();


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output and ST Link
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Pin::STLINK_EN, OUTPUT);
  digitalWrite(Pin::STLINK_EN, LOW);
  //enable serial
  SerialUSB.begin(115200);
  
  while (!SerialUSB) {
    ; // wait for serial port to connect
  }
  printInfo();
}

// the loop function runs over and over again forever
void loop() {

  if(SerialUSB.available() > 0){
    serialInput = SerialUSB.read();
    switch(serialInput) {
      case pduChannel:
        SerialUSB.println("Activating PDU");
        switchChannel(&pdu);
        break;

      case tcuChannel:
        SerialUSB.println("Activating TCU");
        switchChannel(&tcu);
        break;

      case ucuChannel:
        SerialUSB.println("Activating UCU");
        switchChannel(&ucu);
        break;

      case ncuChannel:
        SerialUSB.println("Activating NCU");
        switchChannel(&ncu);
        break;

      case noChannel:
        SerialUSB.println("Deactivating all channels");
        switchChannel(NULL);
        break;

      //do nothing for newline or carriage return
      case '\n':
        break;

      case '\r':
        break;

      default:
        SerialUSB.println("\n\rINVALID INPUT\n\r");
        printInfo();
    }
  }

  //blink stuff
  if( (millis() - previousTime) >= BLINK_DELAY_MS){
    previousTime = millis();
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
  }
}

void switchChannel(Channel* newChannel){
  //disable old channel if one is currently active
  if(activeChannel){
    activeChannel->disable();
  }
  activeChannel = newChannel;
  //turn off ST link and delay to allow for full turn-off
  digitalWrite(Pin::STLINK_EN, LOW);
  delay(Config::ST_LINK_RESTART_DELAY_MS);
  //check if new active channel is not null and activate
  if(activeChannel){
    activeChannel->enable();
  }
}

void printInfo(){
  SerialUSB.println("CCU ST Link Programming Interface");
  SerialUSB.println("Write a number to select which unit to program");
  SerialUSB.println("0: No selection");
  SerialUSB.println("1: NCU");
  SerialUSB.println("2: UCU");
  SerialUSB.println("3: TCU");
  SerialUSB.println("4: PDU");
}
