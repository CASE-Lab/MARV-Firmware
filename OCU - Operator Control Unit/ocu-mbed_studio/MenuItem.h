#ifndef MENU_ITEM_H
#define MENU_ITEM_H

#include "mbed.h"
#include "Types.h"
#include <string> 

class MenuItem{
    private:

        CANMessage *onGreClickCanMsg;
        CANMessage *onRedClickCanMsg;
        CANMessage *onManOrExtCanMsg;
        void* dataPointer; //Pointer to menu item data, replaces previous int8_t, int32_t, float, setting and status

        uint8_t stringIndex;
        uint8_t itemType; 
        uint8_t itemDispStringIndex;
        uint8_t menuLinkId;
        bool reqExternalMode;
        bool reqManualMode;
        uint8_t nbrOfOnGreClickCanMsg;
        uint8_t nbrOfOnRedClickCanMsg;
        uint8_t nbrOfOnManOrExtCanMsg;
        uint8_t floatPrecision;
        bool sendAlwaysCanOnRed;
        bool selectable;

    public:
        //Constructors
        MenuItem();
        //Setup
        void setup(uint8_t stringIndex, uint8_t itemTypeData);
        //Setters
        void setItemDispStringIndex(uint8_t itemDispStringIndexData);
        void setItemDispInt32(int32_t *dataPointer);
        void setItemDispFloat(float *dataPointer);
        void setItemDispSetting(int32_t *dataPointer);
        void setMenuLinkId(uint8_t menuLinkId);
        void setItemDispStatus(int32_t *dataPointer);
        void setDispCustomString(char *dataPointer);
        void setReqExternalMode(bool reqExternalMode);
        void setReqManualMode(bool reqManualMode);
        void setOnGreClickCanMsg(CANMessage *onGreClickCanMsg,uint8_t nbrOfOnGreClickCanMsg);
        void setOnRedClickCanMsg(CANMessage *onRedClickCanMsg,uint8_t nbrOfOnRedClickCanMsg);
        void setOnManOrExtCanMsg(CANMessage *onManOrExtCanMsg,uint8_t nbrOfOnManOrExtCanMsg);
        void setFloatPrecision(uint8_t floatPrecision);
        void setSendAlwaysCanOnRed(bool sendAlwaysCanOnRed);
        void setSelectable(bool selectable);
        //Getters
        uint8_t getStringIndex();
        uint8_t getItemType();
        uint8_t getItemDispStringIndex();
        int32_t getItemDispInt32();
        float getItemDispFloat();
        int32_t getItemDispSetting();
        uint8_t getMenuLinkId();
        int32_t getItemDispStatus();
        char* getDispCustomString();
        string getItemValue();
        bool getReqExternalMode();
        bool getReqManualMode();
        CANMessage *getOnGreClickCanMsg();
        CANMessage *getOnRedClickCanMsg();
        CANMessage *getOnManOrExtCanMsg();
        uint8_t getNbrOfOnGreClickCanMsg();
        uint8_t getNbrOfOnRedClickCanMsg();
        uint8_t getNbrOfOnManOrExtCanMsg();
        uint8_t getFloatPrecision();
        bool getSendAlwaysCanOnRed();
        bool getSelectable();
};

#endif //MENU_ITEM_H