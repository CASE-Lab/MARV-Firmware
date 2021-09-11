#include "MenuItem.h"
#include "mbed.h"
#include "Types.h"
#include "Globals.h"
#include <string>
#include "Config.h"

using namespace std::chrono;

//Variables
/*CANMessage *onManOrExtCanMsg;
CANMessage *onRedClickCanMsg;
CANMessage *onGreClickCanMsg;
int8_t *itemDispInt8;
int32_t *itemDispInt32;
float *itemDispFloat;
uint8_t *itemDispSetting;
uint8_t *itemDispStatus;

uint8_t stringIndex; //Name or text to display on oled
uint8_t itemType; //0 disp var string ref, 1 REMOVED, 2 disp var int32, 3 disp var float, 4 on/off set, 5 link to menu, 6 continuous text, 7 unit status, 8 scenario state, 9 timer, 10 system time, 11 disp custom string, 12 continuous text notification from ACU
uint8_t itemDispStringIndex;
uint8_t menuLinkId; //Id to "link to menu"
bool reqExternalMode;
bool reqManualMode;
uint8_t floatPrecision; //Precision to use when showing float (mode 3)
bool sendAlwaysCanOnRed;
bool selectable;

//Can messages on button click
uint8_t nbrOfOnGreClickCanMsg;
uint8_t nbrOfOnRedClickCanMsg;
uint8_t nbrOfOnManOrExtCanMsg;*/

//Constructors
MenuItem::MenuItem(){}

//Setup
void MenuItem::setup(uint8_t stringIndexData, uint8_t itemTypeData){
    stringIndex = stringIndexData;
    itemType = itemTypeData;
    this->reqExternalMode = false;
    this->reqManualMode = false;
    this->nbrOfOnGreClickCanMsg = 0;
    this->nbrOfOnRedClickCanMsg = 0;
    this->nbrOfOnManOrExtCanMsg = 0;
    this->sendAlwaysCanOnRed=false;
    this->selectable = true;
    //this->temp = 0;
}

//Setters
void MenuItem::setItemDispStringIndex(uint8_t itemDispStringIndexData){
    this->itemDispStringIndex = itemDispStringIndexData;
}

void MenuItem::setItemDispInt32(int32_t *dataPointer){
    this->dataPointer = (void*)dataPointer;
}

void MenuItem::setItemDispFloat(float *dataPointer){
    this->dataPointer = (void*)dataPointer;
}

void MenuItem::setItemDispSetting(int32_t *dataPointer){
    this->dataPointer = (void*)dataPointer;
}

void MenuItem::setMenuLinkId(uint8_t menuLinkIdData){
    menuLinkId = menuLinkIdData;
}

void MenuItem::setItemDispStatus(int32_t *dataPointer){
    this->dataPointer = (void*)dataPointer;
}

void MenuItem::setDispCustomString(char *dataPointer){
    this->dataPointer = dataPointer;
}

void MenuItem::setReqExternalMode(bool reqExternalMode){
    this->reqExternalMode = reqExternalMode;
}

void MenuItem::setReqManualMode(bool reqManualMode){
    this->reqManualMode = reqManualMode;
}

void MenuItem::setOnGreClickCanMsg(CANMessage *onGreClickCanMsg,uint8_t nbrOfOnGreClickCanMsg){
    this->onGreClickCanMsg = onGreClickCanMsg;
    this->nbrOfOnGreClickCanMsg = nbrOfOnGreClickCanMsg;
}

void MenuItem::setOnRedClickCanMsg(CANMessage *onRedClickCanMsg,uint8_t nbrOfOnRedClickCanMsg){
    this->onRedClickCanMsg = onRedClickCanMsg;
    this->nbrOfOnRedClickCanMsg = nbrOfOnRedClickCanMsg;
}

void MenuItem::setOnManOrExtCanMsg(CANMessage *onManOrExtCanMsg,uint8_t nbrOfOnManOrExtCanMsg){
    this->onManOrExtCanMsg = onManOrExtCanMsg;
    this->nbrOfOnManOrExtCanMsg = nbrOfOnManOrExtCanMsg;
}

void MenuItem::setFloatPrecision(uint8_t floatPrecision){
    this->floatPrecision = floatPrecision;
}

void MenuItem::setSendAlwaysCanOnRed(bool sendAlwaysCanOnRed){
    this->sendAlwaysCanOnRed = sendAlwaysCanOnRed;
}

void MenuItem::setSelectable(bool selectable){
    this->selectable = selectable;
}

//Getters
uint8_t MenuItem::getStringIndex(){
    return stringIndex;
}

uint8_t MenuItem::getItemType(){
    return itemType;
}

uint8_t MenuItem::getItemDispStringIndex(){
    return itemDispStringIndex;
}

int32_t MenuItem::getItemDispInt32(){
    return *(int32_t*)dataPointer;
}

float MenuItem::getItemDispFloat(){
    return *(float*)dataPointer;
}

int32_t MenuItem::getItemDispSetting(){
    return *(int32_t*)dataPointer;
}

uint8_t MenuItem::getMenuLinkId(){
    return menuLinkId;
}

int32_t MenuItem::getItemDispStatus(){
    return *(int32_t*)dataPointer;
}

char* MenuItem::getDispCustomString(){
    return (char*)dataPointer;
}

string MenuItem::getItemValue(){
    string itemValueString = "";

    if(itemType == 0){ //0 disp var string ref
        itemValueString.append(programStrings[itemDispStringIndex]);
    }
    else if(itemType == 2){ //1 disp var int32
        itemValueString.append(to_string(*(int32_t*)dataPointer));
    }
    else if(itemType == 3){ //2 disp var float
        itemValueString.append(to_string(*(float*)dataPointer).substr(0,floatPrecision));
    }
    else if(itemType == 4){ //3 on/off set
        if(*(uint8_t*)dataPointer == 0){
            itemValueString.append("OFF");
        }
        else if(*(uint8_t*)dataPointer == 1){
            itemValueString.append("ON");
        }
        else{
            itemValueString.append("ER");
        }     
    }
    else if(itemType == 7){ //Show unit status
        switch (*(int32_t*)dataPointer) {
            case ER_state: //ER (error)
                itemValueString.append(programStrings[ER_str]);
                break;
            case OK_state: //OK (Okay)
                itemValueString.append(programStrings[OK_str]);
                break;
            case NC_state: //-- (Not Connected)
                itemValueString.append(programStrings[NC_str]);
                break;
        }
    }
    else if(itemType == 8){ //Show state
        switch (*(int32_t*)dataPointer) {
            case 0: //Waiting
                itemValueString.append(programStrings[WAITING_str]);
                break;
            case 1: //Executing
                itemValueString.append(programStrings[EXECUTING_str]);
                break;
            case 2: //Error
                itemValueString.append(programStrings[FAULT_str]);
                break; 
            case 3: //Stopped
                itemValueString.append(programStrings[STOPPED_str]);
                break;
            case 4: //Finished
                itemValueString.append(programStrings[FINISHED_str]);
                break;
            case 5: //sent
                itemValueString.append(programStrings[REQ_SENT_str]);
                break;
        }
    }
    else if(itemType == 9){ //Timer
        if(*(int32_t*)dataPointer == WAITING && duration_cast<milliseconds>(menuItemTimer.elapsed_time()).count() != 0 || duration_cast<milliseconds>(menuItemTimer.elapsed_time()).count() > 86400000){ //Reset over 24h
            menuItemTimer.stop(); menuItemTimer.reset();
        }
        else if(*(int32_t*)dataPointer == EXECUTING && duration_cast<milliseconds>(menuItemTimer.elapsed_time()).count() == 0){
            menuItemTimer.start();
        }
        else if(*(int32_t*)dataPointer == FAULT || *(int32_t*)dataPointer == STOPPED || *(int32_t*)dataPointer == FINISHED){
            menuItemTimer.stop();
        }
        else if(*(int32_t*)dataPointer == REQ_SENT){
            menuItemTimer.stop(); menuItemTimer.reset();
        }
        uint8_t hours = 0;
        uint8_t minutes = 0;
        uint8_t seconds = 0;
        if(duration_cast<milliseconds>(menuItemTimer.elapsed_time()).count() != 0){
            int16_t currentTime = duration_cast<milliseconds>(menuItemTimer.elapsed_time()).count()/1000;
            
            //Time conversion
            seconds = currentTime%60;
            currentTime /= 60;
            minutes = currentTime%60;
            hours = currentTime/60;
            
            if(hours<10) itemValueString.append("0");
            itemValueString.append(to_string(hours));
            itemValueString.append(":");
            if(minutes<10) itemValueString.append("0");
            itemValueString.append(to_string(minutes));
            itemValueString.append(":");
            if(seconds<10) itemValueString.append("0");
            itemValueString.append(to_string(seconds));
        }
        else{
            itemValueString.append(("--:--:--"));
        }

        
    }
    else if(itemType == 10){ //System time
        uint8_t hours = 0;
        uint8_t minutes = 0;
        uint8_t seconds = 0;
        if(systemTime != 0){
            int32_t currentTime = systemTime;
            
            if(currentTime > 86400){
                currentTime = currentTime%86400;
            }

            //Time conversion
            seconds = currentTime%60;
            currentTime /= 60;
            minutes = currentTime%60;
            hours = currentTime/60;
            
            if(hours<10) itemValueString.append("0");
            itemValueString.append(to_string(hours));
            itemValueString.append(":");
            if(minutes<10) itemValueString.append("0");
            itemValueString.append(to_string(minutes));
            itemValueString.append(":");
            if(seconds<10) itemValueString.append("0");
            itemValueString.append(to_string(seconds));
        }
        else{
            itemValueString.append(("--:--:--"));
        }
    }
    else if(itemType == 11){ //Display custom string
        itemValueString.append((char*)dataPointer);
    }

    return itemValueString;
}

bool MenuItem::getReqExternalMode(){
    return reqExternalMode;
}

bool MenuItem::getReqManualMode(){
    return reqManualMode;
}

CANMessage* MenuItem::getOnGreClickCanMsg(){
    return onGreClickCanMsg;
}

CANMessage* MenuItem::getOnRedClickCanMsg(){
    return onRedClickCanMsg;
}

CANMessage* MenuItem::getOnManOrExtCanMsg(){
    return onManOrExtCanMsg;
}

uint8_t MenuItem::getNbrOfOnGreClickCanMsg(){
    return nbrOfOnGreClickCanMsg;
}

uint8_t MenuItem::getNbrOfOnRedClickCanMsg(){
    return nbrOfOnRedClickCanMsg;
}

uint8_t MenuItem::getNbrOfOnManOrExtCanMsg(){
    return nbrOfOnManOrExtCanMsg;
}

uint8_t MenuItem::getFloatPrecision(){
    return floatPrecision;
}

bool MenuItem::getSendAlwaysCanOnRed(){
    return sendAlwaysCanOnRed;
}

bool MenuItem::getSelectable(){
    return selectable;
}