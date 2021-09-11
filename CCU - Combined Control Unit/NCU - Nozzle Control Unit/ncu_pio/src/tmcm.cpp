#include "tmcm.h"

Tmcm::Tmcm(PinName TX, PinName RX, PinName DE, int baud, uint8_t adress, uint8_t replyAddress) :
    _tmclSerial(TX, RX, baud),
    _driverEnable(DE),
    _tmclState (TmcmState::idle),
    _address (adress),
    _replyAddress (replyAddress),
    _replyWaiting (false),
    _command{},
    _reply{},
    _replyBuffer{}{
        _messageTimer.start();
    }

// Fill command with 9 byte serial message according to trinamic standard. type deafault value is 0
void Tmcm::generateCommand(uint8_t instruction, uint32_t value, int8_t type){

    _command[0]=_address;                 // RS485 address of trinamic driver
    _command[1]=instruction;              // Trinamic instruction code
    _command[2]=type;                     // Instruction type (Not used for all commands)
    _command[3]=_MOTOR;                   // Motor number (always 0 as the TMCL-1260 only has one motor driver)
    _command[4]=value>>24;                // MSB of instruction value
    _command[5]=(value&(0x00FF0000))>>16;
    _command[6]=(value&(0x0000FF00))>>8;
    _command[7]=(value&(0x000000FF));     // LSB of instruction value

    _command[8]=calculateChecksum(_command);
}

uint8_t Tmcm::calculateChecksum(uint8_t* packet){
    uint8_t checksum = 0;
    for(uint8_t i=0; i<8; i++){
        checksum+=packet[i];
    }
    return checksum;
}

int Tmcm::commandTransactBlocking(){
    //busy wait for tmcm to be ready for next message
    while(_messageTimer.elapsed_time().count() < TmcmConfig::INTER_MSG_DELAY_US){
        //debug("Waiting... Timer at: %lu\n\r", (uint32_t)std::chrono::duration_cast<std::chrono::microseconds>(_messageTimer.elapsed_time()).count());
        wait_us(10);
    }
    //Note: Assumes command to be sent allready in _command
    memset(_replyBuffer, 0, sizeof(_replyBuffer));  //clearing reply buffer
    memset(_reply, 0, sizeof(_reply));              //clearing aligned reply
    _tmclState = TmcmState::sending;
    _driverEnable = 1;
    wait_us(10);
    _tmclSerial.write(_command, sizeof(_command));
    wait_us(TmcmConfig::DE_DELAY_US);
    _driverEnable = 0;
    wait_us(10);
    _tmclState = TmcmState::reciving;
    wait_us(TmcmConfig::RECIVE_DELAY_US);
    int result = readReply();
    //reset and restart timer (needs to be stopped before reset)
    _messageTimer.stop();
    _messageTimer.reset();
    _messageTimer.start();
    //debug("Reply result: %d\r\n", result);
    return result;
}

void Tmcm::commandTransactAsync(){
    // check if there is a reply waiting from last transaction
    if(_replyWaiting){

    }

}

int Tmcm::readReply(){
            _replyWaiting = 0;
            uint8_t* alignedReply = nullptr;
        if(_tmclSerial.readable()){
            int readResult = _tmclSerial.read(_replyBuffer, sizeof(_replyBuffer));
            if(readResult < 8){
                //complete packet not read
                debug("ERROR: To few bytes\n\r");
                return -5;
            }
            //debug("\n\rBytes read: %d\n\r", readResult);
            // search replyBuffer for Reply adress which indicates start of reply message
            for(uint8_t i = 0; i < readResult - sizeof(_reply); i++){
                if(_replyBuffer[i] == _replyAddress){
                    alignedReply = _replyBuffer+i;
                    break;
                }
            }
            // if aligned reply was not found try correcting (jitter related error...)
            if(!alignedReply)
            {
                //debug("Jitter EC (buffer before): ");
                //printMessageHex(_replyBuffer, sizeof(_replyBuffer));
                //Manually set first two bytes too expected response
                _replyBuffer[0] = Tmcm::_replyAddress;
                _replyBuffer[1] = Tmcm::_address;
                alignedReply = _replyBuffer;
                //printMessageHex(_replyBuffer, sizeof(_replyBuffer));
            }
        }
        else{
            //serial port was not readable
            debug("Reply serial not readable\n\r");
            return -2;
        }
        // check if start of reply message was found (i.e pointer was initialized)
        if(alignedReply){
            //debug("Aligned reply: ");
            //printMessageHex(alignedReply, 9); //debug
            if(calculateChecksum(alignedReply) == alignedReply[8]){
                memcpy(_reply, alignedReply, sizeof(_reply));   //copy aligned reply to _reply
                return alignedReply[2];                         //return reply status message
            }
            else{
                debug("ERROR: Incorrect cheksum\n\r");
                //debug("ERROR: Incorrect checksum. Calculated checksum: %02X\n\r", calculateChecksum(alignedReply));
                //printMessageHex(_replyBuffer, sizeof(_replyBuffer));
                return -3;
            }
            
        }
        else{
            //start of message was not found
            debug("Start of reply not found\n\r");
            printMessageHex(_replyBuffer, sizeof(_replyBuffer));
            return -1;
        }
        return -4;  //get warning without this
}

void Tmcm::printMessageHex(uint8_t* message, uint8_t len){
    printf("%02X", message[0]);
    for(uint8_t i = 1; i < len; i++){
        printf("-%02X", message[i]);
    }
    printf("\r\n");
}

int Tmcm::sendRawCommand(uint8_t instruction, uint8_t type, uint32_t value){
    generateCommand(instruction, value, type);
    //debug("Cmd: ");
    //printMessageHex(_command, sizeof(_command));
    return commandTransactBlocking();
}

int Tmcm::sendRawCommand(const TmcmCommand* cmd){
    return sendRawCommand(cmd->instruction, cmd->type, cmd->value);
}

int Tmcm::sendRawCommandRetry(uint8_t instruction, uint8_t type, uint32_t value){
    int sendResult;
    for(unsigned int retries = 0; retries <= TmcmConfig::RETRIES; retries++){
        sendResult = sendRawCommand(instruction, type, value);
        if ( (sendResult == TmcmStatus::SUCCESS) || (sendResult == TmcmStatus::EEPROM_LOAD_SUCCESS) )
            return sendResult;
    }
    debug("ERROR: All retries failed when sending command\n\r");
    return sendResult;
}

int Tmcm::sendRawCommandRetry(const TmcmCommand* cmd){
    return sendRawCommandRetry(cmd->instruction, cmd->type, cmd->value);
}

int Tmcm::getReply(){
    uint8_t replyLsb[4] = {_reply[7], _reply[6], _reply[5], _reply[4]};
    uint32_t rawReply = *((uint32_t*)replyLsb);
    //debug("Reply hex:");
    //printMessageHex(_reply, sizeof(_reply));
    //debug("Raw reply: %ld\n\r", (int)rawReply);
    return rawReply;
}
