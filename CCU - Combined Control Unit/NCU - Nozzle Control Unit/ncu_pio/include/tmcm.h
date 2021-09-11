#pragma once

#include <mbed.h>

//Motor max message rate i 1 transaction / 7 ms

// Reason for putting enum in namesapce: https://gamesfromwithin.com/stupid-c-tricks-2-better-enums
namespace TmcmState{
    enum State {idle, sending, reciving};
}

namespace TmcmStatus{
    constexpr int SUCCESS = 100;                          //status code (from table 4 in FW manual)
    constexpr int EEPROM_LOAD_SUCCESS = 101;
    constexpr int WRONG_CHECKSUM = 1;
    constexpr int INVALID_COMMAND = 2;
    constexpr int WRONG_TYPE = 3;
    constexpr int INVALID_VALUE = 4;
    constexpr int CONFIG_EEPROM_LOCKED = 5;
    constexpr int COMMAND_NOT_AVAIABLE = 6;
}

// Not all instructions added, refer to TMCL firmware manual : https://www.trinamic.com/products/modules/details/tmcm-1260/
namespace TmcmInstruction{
    constexpr uint8_t ROR = 1;                      //Rotate right with specified velocity (in microsteps/s)
    constexpr uint8_t ROL = 2;                      //Rotate left with specified velocity (in microsteps/s)
    constexpr uint8_t MST = 3;                      //Stop motor movement
    constexpr uint8_t MVP = 4;                      //Move to position (absolute or relative)
    constexpr uint8_t SAP = 5;                      //Set axis parameter (motion controlspecific settings)
    constexpr uint8_t GAP = 6;                      //Get axis parameter (read out motiong control specific settings)
}

// TMCL Instruction type (prefix is instruction name)
namespace TmcmType{
    constexpr uint8_t NO_TYPE = 0;
    // Axis parameters:
    constexpr uint8_t AP_TARGET_POSITION = 0;           //The desired target position in position mode
    constexpr uint8_t AP_ACTUAL_POSITION = 1;
    constexpr uint8_t AP_TARGET_SPEED = 2;
    constexpr uint8_t AP_ACTUAL_SPEED = 3;
    constexpr uint8_t AP_MAXIMUM_POSITIONING_SPEED = 4;
    constexpr uint8_t AP_MAXIMUN_ACCELERATION = 5;
    constexpr uint8_t AP_MAXIMUM_CURRENT = 6;
    constexpr uint8_t AP_STANDBY_CURRENT = 7;
    constexpr uint8_t AP_ACCELERATION_A1 = 15;
    constexpr uint8_t AP_MAXIMUM_DECELERATION = 17;
    constexpr uint8_t AP_MICROSTEP_RESOLUTION = 140;
    constexpr uint8_t AP_MEASURED_SPEED = 29;
    constexpr uint8_t AP_FULLSTEP_RESOLUTION = 202;
    constexpr uint8_t AP_REVERSE_SHAFT = 251;              //for reversing direction of left/right
}

// TMCL parameter configuration
namespace TmcmConfig{
    constexpr int BAUD = 115200;                            //set via constructor
    constexpr unsigned int DE_DELAY_US = 756;               //based on 115200 baudrate
    constexpr unsigned int RECIVE_DELAY_US = 1700;          //based on 115200 baudrate
    constexpr unsigned int RETRIES = 3;                     //number of times to attempt resending failed messages
    constexpr unsigned int INTER_MSG_DELAY_US = 4400;       //delay to add between messages (added after reply reccived)
    constexpr unsigned int NO_VALUE = 0;
}

//struct for TMCL command
struct TmcmCommand{
    uint8_t instruction;
    uint8_t type;
    uint32_t value;
};

namespace TmcmCmd{
    constexpr TmcmCommand GET_ACTUAL_SPEED = {TmcmInstruction::GAP,
                                              TmcmType::AP_MEASURED_SPEED,
                                              TmcmConfig::NO_VALUE};
}

class Tmcm{
    public:
    Tmcm(PinName TX, PinName RX, PinName DE, int baud, uint8_t address, uint8_t replyAddress);

    TmcmState::State getState(){return _tmclState;}
    void printMessageHex(uint8_t* message, uint8_t len);
    int sendRawCommand(uint8_t instruction, uint8_t type, uint32_t value);
    int sendRawCommand(const TmcmCommand* command);
    int sendRawCommandRetry(uint8_t instruction, uint8_t type, uint32_t value);
    int sendRawCommandRetry(const TmcmCommand* command);

    int getReply();

    private:
    BufferedSerial _tmclSerial;
    DigitalOut _driverEnable;
    TmcmState::State _tmclState;                //stores RS485 communication transaction state (init to idle)
    const uint8_t _address;                     //RS485 driver adress
    const uint8_t _replyAddress;                //RS485 reply adress
    bool _replyWaiting;                         // 1: Indicates there is a reply waiting to be read
    
    uint8_t _command[9];                        // stores command to be sent
    uint8_t _reply[9];                          // stores aligned reply
    uint8_t _replyBuffer[20];                   // buffer for serial read in of reply

    static const uint8_t _MOTOR = 0;            //allways 0 as there is only a single motor

    Timeout _driverEnableTimeout;               //Timeout for how long to enable DE pin
    Timeout _reciveReplyTimeout;                //Timeout for how long to recive reply from motor driver

    Timer _messageTimer;                        //tracks inter message delay time

    // genrates a command in TMCL format for transmission over serial
    void generateCommand(uint8_t instruction, uint32_t value, int8_t type = TmcmType::NO_TYPE);
    // TMCL command checksum calcualtion
    uint8_t calculateChecksum(uint8_t* packet);
    // Enables reciver of RS485 (controlled by _driverEnableTimeout)
    void enableReciver(){_driverEnable = 0;}
    // sets flag indicating that there should be a reply waiting in the serial RX buffer
    void setReplyWaiting(){_replyWaiting = true;}
    // command transaction async
    void commandTransactAsync();
    // command transaction blocking
    int commandTransactBlocking();
    // read and decode reply
    int readReply();
};