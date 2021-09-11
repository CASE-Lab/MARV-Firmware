#include "Menu.h"
#include "mbed.h"
#include "Types.h"
#include "MenuItem.h"
#include "Config.h"
#include "Globals.h"
#include <algorithm>
#include <cstdint>
#include <string.h>
#include <string>

using namespace std;
using namespace std::chrono;

//Variables
/*uint8_t menuId;
uint8_t menuHeaderIndex;
bool menuHeaderCANSet;
uint8_t menuCANIndex;
MenuItem *menuItems;
uint8_t menuNbrOfItems;
bool scrollable; //false removes marker, true show marker and allow scrolling through the menu
uint8_t menuItemPos;
uint32_t timeLimit; //Automatically go back to previous menu after a certain specified time 
bool reqDoubleClickedButton; //Set to true if double clicking button is required, else false
bool returnOnExtModeActive; //Returns from menu if external mode has been activated
bool returnOnManModeActive; //Returns from menu if manual mode has been activated

//Periodic CAN messages
CANMessage *periodicCanMsg;
uint8_t nbrOfPeriodicCanMsg;
uint16_t periodicCanMsgUpdateTime;
//Can messages on entering menu
CANMessage *onEnterCanMsg;
uint8_t nbrOfOnEnterCanMsg;

//Center menu text
bool centering;*/

//Constructor
Menu::Menu(){}
Menu::Menu(uint8_t menuId, uint8_t menuHeaderIndex, MenuItem *menuItems, uint8_t menuNbrOfItems, bool scrollable, bool centering){
    this->menuId = menuId;
    this->menuHeaderIndex = menuHeaderIndex;
    this->menuItems = menuItems;
    this->menuNbrOfItems = menuNbrOfItems;
    this->scrollable = scrollable;
    this->menuItemPos=0;
    this->periodicCanMsgUpdateTime=0;
    this->timeLimit = 0;
    this->centering = centering;
    this->redBtnClickedOnce=false;
    this->reqDoubleClickedButton=false;
    this->returnOnExtModeActive=false;
    this->returnOnManModeActive=false;
    this->nbrOfOnEnterCanMsg=0;
    this->menuHeaderCANSet=false;
    this->scenarioDataHeaderSet=0;
}

//Periodic CAN messages
void Menu::setPeriodicCanMsg(CANMessage *periodicCanMsg,uint8_t nbrOfPeriodicCanMsg){
    this->periodicCanMsg = periodicCanMsg;
    this->nbrOfPeriodicCanMsg = nbrOfPeriodicCanMsg;
}
void Menu::setPeriodicCanMsgUpdateTime(uint16_t periodicCanMsgUpdateTime){
    this->periodicCanMsgUpdateTime = periodicCanMsgUpdateTime;
}
void Menu::setTimeLimit(int32_t timeLimit){
    this->timeLimit = timeLimit;
}

void Menu::setReqDoubleClickedButton(bool reqDoubleClickedButton){
    this->reqDoubleClickedButton = reqDoubleClickedButton;
}

void Menu::setReturnOnExtModeActive(bool returnOnExtModeActive){
    this->returnOnExtModeActive = returnOnExtModeActive;
}

void Menu::setReturnOnManModeActive(bool returnOnManModeActive){
    this->returnOnManModeActive = returnOnManModeActive;
}

void Menu::setMenuHeaderCAN(uint8_t menuCANIndex){
    this->menuCANIndex = menuCANIndex;
    this->menuHeaderCANSet = true;
    //printf("ID: %d\n",this->menuCANIndex);
    //printf("CAN_SET: %d\n",this->menuHeaderCANSet);
}

void Menu::setMenuNbrOfItems(uint8_t menuNbrOfItems){
    this->menuNbrOfItems = menuNbrOfItems;
}

void Menu::setScenarioDataHeaderSet(uint8_t scenarioDataHeaderSet){
    this->scenarioDataHeaderSet = scenarioDataHeaderSet;
}

uint8_t Menu::getMenuId(){
    return menuId;
}

bool Menu::getMenuHeaderCANSet(){
    return menuHeaderCANSet;
}

uint8_t Menu::getMenuNbrOfItems(){
    return menuNbrOfItems;
}
    
void Menu::setOnEnterCanMsg(CANMessage *onEnterCanMsg,uint8_t nbrOfOnEnterCanMsg){
    this->onEnterCanMsg = onEnterCanMsg;
    this->nbrOfOnEnterCanMsg = nbrOfOnEnterCanMsg;
}

uint8_t Menu::getScenarioDataHeaderSet(){
    return scenarioDataHeaderSet;
}

//Send messages if enough time has passed
void Menu::updatePeiodicCAN(){
    if(periodicCanMsgUpdateTime != 0){
        if(duration_cast<milliseconds>(periodicCanMsgTimer.elapsed_time()).count() == 0){
            periodicCanMsgTimer.start();
        }
        else if(duration_cast<milliseconds>(periodicCanMsgTimer.elapsed_time()).count() > periodicCanMsgUpdateTime){
            periodicCanMsgTimer.reset();
            for(uint8_t i=0;i<nbrOfPeriodicCanMsg;i++){
                databus.write(periodicCanMsg[i]);
            }  
        }
    }
    //printf("%d\n",(int)(duration_cast<milliseconds>(periodicCanMsgTimer.elapsed_time()).count()));
}

//Update menu depending on inputs
void Menu::updateActions(){
    //Send CAN message on entering menu
    if(onEnterCanMsgSend && nbrOfOnEnterCanMsg != 0){
        for(uint8_t i=0;i<nbrOfOnEnterCanMsg;i++){
                databus.write(onEnterCanMsg[i]);
        }
        onEnterCanMsgSend = false;
    }
    //printf("MenuID: %d\n",menuId);
    //Show notification message popup
    if((showNotificationPopup && timeLimit == 0)){
        //printf("Noti Bool: %d",showNotificationPopup);
        showNotificationPopup = false;

        //Switch menu
        previousActiveMenuStack.push(activeMenu); //Keep this menu in history
        activeMenu=&menus[notificationMenu_ID]; //Switch to new menu
        //Stop and reset periodic can messages
        periodicCanMsgTimer.stop(); //Stop can message timer 
        periodicCanMsgTimer.reset();
        onEnterCanMsgSend = true;
        printf("Showing notification\n");
    }

    //If in stopped state and enough time has passed, reset scenario values to default
    if(!scenarioVariablesReset && scenarioState == STOPPED && duration_cast<milliseconds>(resetScenarioVariablesTimer.elapsed_time()).count() == 0){
        resetScenarioVariablesTimer.start();
    }
    else if(!scenarioVariablesReset && scenarioState == STOPPED && duration_cast<milliseconds>(resetScenarioVariablesTimer.elapsed_time()).count() > resetScenarioVariables_TimeLimit){
        resetScenarioVariablesTimer.stop();
        resetScenarioVariablesTimer.reset();
        scenarioVariablesReset = true;
        
        //Set progressString to default value
        memset(progressString,' ',sizeof(progressString)-1);
        progressString[0] = '-'; progressString[1] = '-';
        //Set scenarioData to default value
        for(uint8_t i=0;i<nbrOfCanScenarios;i++){
            memset(scenarioData_1[i],' ',sizeof(scenarioData_1[i])-1);
            scenarioData_1[i][0] = '-'; scenarioData_1[i][1] = '-';
            memset(scenarioData_2[i],' ',sizeof(scenarioData_2[i])-1);
            scenarioData_2[i][0] = '-'; scenarioData_2[i][1] = '-';
        }  
    }
        
    //Update states
    if(scenarioState == STOPPED && duration_cast<milliseconds>(stateResetTimer.elapsed_time()).count() > 0){
        stateResetTimer.stop();
        stateResetTimer.reset();
        popupShownOnce = false;
    }
    //If we have not gotten any response from the ACU within the scenarioStateTimeout, change back to STOPPED
    else if(scenarioState == REQ_SENT && duration_cast<milliseconds>(stateResetTimer.elapsed_time()).count() > scenarioStateTimeout){
        scenarioState = STOPPED;
    }
    //Show waiting for EXT mode popup
    else if(scenarioState == WAITING && STM_12_INDIC < 0.4 && !popupShownOnce){ //WAITING is only set from the ACU, confirming the request to start scenario
        //If WAITING and in man mode, then show switch to ext mode popup

        popupShownOnce = true; //Show ext popup once, and not every menu update

        //Switch menu
        previousActiveMenuStack.push(activeMenu); //Keep this menu in history
        activeMenu=&menus[reqExtMenu_ID]; //Switch to new menu
        //Stop and reset periodic can messages
        periodicCanMsgTimer.stop(); //Stop can message timer 
        periodicCanMsgTimer.reset();
        onEnterCanMsgSend = true;
    }
    /*else if(scenarioState == WAITING && STM_12_INDIC > 0.4){
        scenarioState = EXECUTING;
    }
    else if((scenarioState == EXECUTING || scenarioState == FINISHED || scenarioState == FAULT) && STM_12_INDIC < 0.4){
        scenarioState = STOPPED;
    }*/
    else if(menuId == reqExtMenu_ID && scenarioState == STOPPED && STM_12_INDIC < 0.4){
        //Switch menu
        activeMenu = previousActiveMenuStack.top();
        previousActiveMenuStack.pop();
        //Stop and reset time limit menu timer
        timeLimitTimer.stop();
        timeLimitTimer.reset();
        //Stop and reset periodic can messages
        periodicCanMsgTimer.stop(); //Stop can message timer 
        periodicCanMsgTimer.reset();
        //Send can messages on click if specified
    }
    else if(menuId == powerMenu_ID && STM_12_INDIC > 0.4){ // Go back from power menu to main menu if external mode is activated
        //Switch menu
        activeMenu = previousActiveMenuStack.top();
        previousActiveMenuStack.pop();
        //Stop and reset time limit menu timer
        timeLimitTimer.stop();
        timeLimitTimer.reset();
        //Stop and reset periodic can messages
        periodicCanMsgTimer.stop(); //Stop can message timer 
        periodicCanMsgTimer.reset();
    }
    
    //Show popup state change NC menu
    if(unit_state_change_NC){
        if(menuId != statusMenu_ID && menuId != NCstateChangeMenu_ID && menuId != ERstateChangeMenu_ID && menuId != startupMenu_ID){ 
            previousActiveMenuStack.push(activeMenu); //Keep this menu in history
            activeMenu=&menus[NCstateChangeMenu_ID];
            //Stop and reset periodic can messages
            periodicCanMsgTimer.stop(); //Stop can message timer 
            periodicCanMsgTimer.reset();
            onEnterCanMsgSend = true;   
            #ifdef SIMPLE_DEBUG
                printf("NC POPUP\n");
            #endif   
        }
        unit_state_change_NC = false;          
    }
    //Show popup state change ER menu
    else if(unit_state_change_ER){
        if(menuId != statusMenu_ID && menuId != NCstateChangeMenu_ID && menuId != ERstateChangeMenu_ID && menuId != startupMenu_ID){
            previousActiveMenuStack.push(activeMenu); //Keep this menu in history
            activeMenu=&menus[ERstateChangeMenu_ID];
            //Stop and reset periodic can messages
            periodicCanMsgTimer.stop(); //Stop can message timer 
            periodicCanMsgTimer.reset();
            onEnterCanMsgSend = true; 
            #ifdef SIMPLE_DEBUG
                printf("ER POPUP\n");
            #endif
        }
        unit_state_change_ER = false;      
    }
    //If green button has been pressed
    else if(greBtnClicked){
        greBtnClicked=false;
        if(menuItems[menuItemPos].getItemType() == 8){ //If scenario menuitem type
            if(scenarioState == STOPPED || scenarioState == REQ_SENT){ //Prevents unnessecary start requests if scenario is running
                if(menuItems[menuItemPos].getReqManualMode() && STM_12_INDIC > 0.4){ //Request user to switch to manual mode before starting a scenario
                //Switch menu

                previousActiveMenuStack.push(activeMenu); //Keep this menu in history
                activeMenu=&menus[reqManMenu_ID];
                //Stop and reset periodic can messages
                periodicCanMsgTimer.stop(); //Stop can message timer 
                periodicCanMsgTimer.reset();
                onEnterCanMsgSend = true;
            }
            else if(menuItems[menuItemPos].getNbrOfOnGreClickCanMsg() != 0){
                for(uint8_t i=0;i<menuItems[menuItemPos].getNbrOfOnGreClickCanMsg();i++){
                        databus.write((menuItems[menuItemPos].getOnGreClickCanMsg())[i]);
                }

                scenarioState = REQ_SENT;
                stateResetTimer.reset();
                stateResetTimer.start();
                popupShownOnce = false;
                }
            }   
        }
        else{
            if(menuItems[menuItemPos].getNbrOfOnGreClickCanMsg() != 0){
                for(uint8_t i=0;i<menuItems[menuItemPos].getNbrOfOnGreClickCanMsg();i++){
                        databus.write((menuItems[menuItemPos].getOnGreClickCanMsg())[i]);
                }  
            }
            if(menuItems[menuItemPos].getItemType() == 5){
                //Switch menu
                previousActiveMenuStack.push(activeMenu); //Keep this menu in history
                activeMenu=&menus[menuItems[menuItemPos].getMenuLinkId()]; //Switch to new menu
                //If we are going to change to scenario menu and there are no scenarios 
                if(activeMenu->getMenuId() == scenarioMenu_ID && activeMenu->getMenuNbrOfItems() == 0){
                    activeMenu = &menus[noScenariosMenu_ID];
                }
                if(activeMenu->getMenuId() == powerMenu_ID && STM_12_INDIC > 0.4){
                    activeMenu = &menus[sysPwrExtPopupMenu_ID];
                }
                //Stop and reset periodic can messages
                periodicCanMsgTimer.stop(); //Stop can message timer 
                periodicCanMsgTimer.reset();
                //Send can messages on click if specified
                onEnterCanMsgSend = true;

                //If scenario is running jump past scenario menu
                if(activeMenu->getMenuId() == scenarioMenu_ID && (scenarioState == WAITING || scenarioState == EXECUTING)){
                    greBtnClicked=true;
                }
            }
        }
        doubleClickedButton=false;
    }
    //If red button has been pressed
    else if(redBtnClicked){
        redBtnClicked=false;
        if(menuItems[menuItemPos].getNbrOfOnRedClickCanMsg() != 0 && menuItems[menuItemPos].getSendAlwaysCanOnRed()){
            for(uint8_t i=0;i<menuItems[menuItemPos].getNbrOfOnRedClickCanMsg();i++){
                databus.write((menuItems[menuItemPos].getOnRedClickCanMsg())[i]);
            }
        }
        
        if(menuId != mainMenu_ID){ //If not in Main menu then access previous menu from history
            //Switch menu
            activeMenu = previousActiveMenuStack.top();
            previousActiveMenuStack.pop();
            //Stop and reset time limit menu timer
            timeLimitTimer.stop();
            timeLimitTimer.reset();
            //Stop and reset periodic can messages
            periodicCanMsgTimer.stop(); //Stop can message timer 
            periodicCanMsgTimer.reset();
            //Send can messages on click if specified

            //Avoid scenario menu if returning from ongoing scenario while the state is waiting or executing
            if(menuItems[menuItemPos].getItemType() == 8 && (scenarioState == WAITING || scenarioState == EXECUTING)){
                redBtnClicked=true;
            }
        }
        else{
            while(!previousActiveMenuStack.empty()) previousActiveMenuStack.pop(); //Empty stack if it still contains something in the main menu
        }
        
        doubleClickedButton=false;      
    }
    else if(scrWheelUp){
        scrWheelUp=false;
        if(scrollable){
            if(menuItemPos > 0){
                for(uint8_t i = (menuItemPos-1); i>=0; i--){
                    if(menuItems[i].getSelectable()){
                        menuItemPos = i;
                        break;
                    }
                }
                //menuItemPos-=1;
            }
        }
    }
    else if(scrWheelDown){
        scrWheelDown=false;
        if(scrollable){
            if(menuItemPos < menuNbrOfItems-1){
                for(uint8_t i = (menuItemPos+1); i<=menuNbrOfItems-1; i++){
                    if(menuItems[i].getSelectable()){
                        menuItemPos = i;
                        break;
                    }
                }
                //menuItemPos+=1;
            }
        }
    }
    else if(timeLimit != 0){ //If time limited menu, go back after a certain time
        if(duration_cast<milliseconds>(timeLimitTimer.elapsed_time()).count() == 0){
            timeLimitTimer.start();
        }
        else if (duration_cast<milliseconds>(timeLimitTimer.elapsed_time()).count() > timeLimit){
            //Switch menu
            activeMenu = previousActiveMenuStack.top();
            previousActiveMenuStack.pop();
            //Stop and reset time limit menu timer
            timeLimitTimer.stop();
            timeLimitTimer.reset();
            //Stop and reset periodic can messages
            periodicCanMsgTimer.stop(); //Stop can message timer 
            periodicCanMsgTimer.reset();
        }
    }
    else if(returnOnExtModeActive && STM_12_INDIC > 0.4 || returnOnManModeActive && STM_12_INDIC < 0.4){
        if(menuItems[menuItemPos].getNbrOfOnManOrExtCanMsg() != 0){
            for(uint8_t i=0;i<menuItems[menuItemPos].getNbrOfOnManOrExtCanMsg();i++){
                databus.write((menuItems[menuItemPos].getOnManOrExtCanMsg())[i]);
            }
        }
        //Switch menu
        activeMenu = previousActiveMenuStack.top();
        previousActiveMenuStack.pop();
        //Stop and reset time limit menu timer
        timeLimitTimer.stop();
        timeLimitTimer.reset();
        //Stop and reset periodic can messages
        periodicCanMsgTimer.stop(); //Stop can message timer 
        periodicCanMsgTimer.reset();
    }
}

uint8_t Menu::updateMenu(){

    //Do not update menu if in scenario menu and a scenario is running
    if(!(menuId == scenarioMenu_ID && (scenarioState == WAITING || scenarioState == EXECUTING))){
        //Om grön knapp ta action, kolla volatil variabel om knapp tryckts
        //Om röd knapp ta action, gå tillbaka om möjligt, kolla global variabel
        //Om scrollhjul byt menu item
        //retunera 1 om uppdaterat, 0 om inte
        
        
        //memset(completeMenu,'.',sizeof(completeMenu)); //Reset char output array
        //Only update screen if anything changed

        //Loop through all menu items, add the complete output to a string array
        //############################################################################################
        //Reset entire menu output array
        for(uint8_t i=0;i<nbrOfMenuRows;i++){
            memset(completeMenu[i],' ',sizeof(completeMenu[i]));
        }
        //Display Header
        string menuLineHeader = "";
        float headerMarkersF = float((nbrOfMenuCols - strlen(programStrings[menuHeaderIndex]))/2);
        uint8_t headerMarkers = static_cast<uint8_t>(headerMarkersF);
        //uint8_t headerMarkers = nbrOfMenuCols - strlen(programStrings[menuHeaderIndex])/2;
        /*for(uint8_t j=0;j<headerMarkers;j++){
            menuLineHeader.append(" ");
        }*/
        if(!menuHeaderCANSet){
            menuLineHeader.append(programStrings[menuHeaderIndex]);
        }
        else{
            menuLineHeader.append(menuHeaderCAN[menuCANIndex]);
        }
        
        /*for(uint8_t j=0;j<headerMarkers;j++){
            menuLineHeader.append(" ");
        }*/
        /*if(float(headerMarkersF/headerMarkers)>1){
            menuLineHeader.append("#");
        }*/

        strncpy(completeMenu[0],menuLineHeader.c_str(),menuLineHeader.length());

        //Display menu items
        uint8_t currentLine=0; //Line on which the text should be written to

        for(uint8_t i=0;i<menuNbrOfItems;i++){
            MenuItem currentMenuItem = menuItems[i];
            if(currentMenuItem.getItemType()==6 || currentMenuItem.getItemType()==12){ //Display only a text, or display text from notification can message
                string delim = "#";
                string text = "";

                if(currentMenuItem.getItemType()==6){
                    text.append(programStrings[currentMenuItem.getStringIndex()]);
                } 
                else{
                    text.append(notificationMessage);
                }
                
                for(uint8_t j=1;j<nbrOfMenuRows;j++){
                    size_t delimIndex = 0;
                    if((delimIndex = text.find(delim)) != string::npos){
                        string token = text.substr(0, delimIndex);
                        text.erase(0,delimIndex+delim.length());
                        uint8_t blankspaces = (nbrOfMenuCols - token.length())/2;
                        string menuLineText = "";
                        menuLineText.append(token);
                        strncpy(completeMenu[j],menuLineText.c_str(),menuLineText.length());
                    }
                }
            }
            else{ //Display normal items
                string menuLine = "";
                if(i == menuItemPos && scrollable){
                    menuLine.append(">"); 
                }
                else{
                    //menuLine.append(" ");
                    menuLine.push_back(0x7F); //Shorter blankspace
                }

                if((scenarioDataHeaderSet == 1 || scenarioDataHeaderSet == 2) && i == nbrOfCanScenarioObjects-2){ //show scenario data header
                    menuLine.append(scenarioDataHeader_1[menuId-canMenuStart_ID]);
                }
                else if(scenarioDataHeaderSet == 2 && i == nbrOfCanScenarioObjects-1){ //show scenario data header
                    menuLine.append(scenarioDataHeader_2[menuId-canMenuStart_ID]);
                }
                else if(menuId != scenarioMenu_ID){
                    menuLine.append(programStrings[currentMenuItem.getStringIndex()]); //Add name or text to display
                }
                else{
                    if(!menus[currentMenuItem.getMenuLinkId()].getMenuHeaderCANSet()){
                        menuLine.append(programStrings[currentMenuItem.getStringIndex()]); //Add name or text to display
                    }
                    else{
                        menuLine.append(menuHeaderCAN[i]);
                    }
                }
                
                if(currentMenuItem.getItemType() != 5){//5 link to menu
                    menuLine.append(": ");
                    if(menuLine.length()>newLineLength && currentMenuItem.getItemType() != 7){ //Show unit status
                        if(currentLine<nbrOfMenuRows-1){
                            strncpy(completeMenu[currentLine+1],menuLine.c_str(),menuLine.length());
                            currentLine++;
                            menuLine.clear();
                            menuLine.push_back(0x7F);
                            menuLine.push_back(0x7F);
                        }
                    }
                    menuLine.append(currentMenuItem.getItemValue());
                }     
                //Copy contents of the temporary menuLine string into the char array
                if(currentLine<nbrOfMenuRows-1){
                    strncpy(completeMenu[currentLine+1],menuLine.c_str(),menuLine.length());
                    currentLine++;
                }
            }
        } 
        //############################################################################################
    }
    
    return 0;     
}

bool Menu::getCentering(){
    return centering;
}