#ifndef MENU_H
#define MENU_H

#include "MenuItem.h"
#include "mbed.h"
#include "Types.h"
#include "MenuItem.h"
#include <string.h>

class Menu{
    private:
        //Variables
        MenuItem *menuItems;
        CANMessage *periodicCanMsg;
        CANMessage *onEnterCanMsg;
        
        uint32_t timeLimit;
        uint16_t periodicCanMsgUpdateTime;
        uint8_t menuId;
        uint8_t menuHeaderIndex;
        bool menuHeaderCANSet;
        uint8_t menuCANIndex;
        uint8_t menuNbrOfItems;
        bool scrollable;
        uint8_t menuItemPos;
        uint8_t nbrOfPeriodicCanMsg;
        uint8_t currentMenuItemPos;
        bool centering;
        bool redBtnClickedOnce;
        bool reqDoubleClickedButton;
        bool returnOnExtModeActive;
        bool returnOnManModeActive; 
        uint8_t nbrOfOnEnterCanMsg;
        uint8_t scenarioDataHeaderSet;
    public:
        //Constructor
        Menu();
        Menu(uint8_t menuId, uint8_t menuHeaderIndex, MenuItem *menuItems, uint8_t menuNbrOfItems, bool scrollable, bool centering);
        //Setters
        void setPeriodicCanMsg(CANMessage *periodicCanMsg, uint8_t nbrOfPeriodicCanMsg);
        void setPeriodicCanMsgUpdateTime(uint16_t periodicCanMsgUpdateTime);
        void setTimeLimit(int32_t timeLimit);
        void updatePeiodicCAN();
        void updateActions();
        uint8_t updateMenu();
        char* getMenu();
        bool getCentering();
        void setReqDoubleClickedButton(bool reqDoubleClickedButton);
        void setReturnOnExtModeActive(bool returnOnExtModeActive);
        void setReturnOnManModeActive(bool returnOnManModeActive);
        void setMenuHeaderCAN(uint8_t menuCANIndex);
        void setMenuNbrOfItems(uint8_t menuNbrOfItems);
        uint8_t getMenuId();
        bool getMenuHeaderCANSet();
        uint8_t getMenuNbrOfItems();
        void setOnEnterCanMsg(CANMessage *onEnterCanMsg,uint8_t nbrOfOnEnterCanMsg);
        void setScenarioDataHeaderSet(uint8_t scenarioDataHeaderSet);
        uint8_t getScenarioDataHeaderSet();
};

#endif //MENU_H