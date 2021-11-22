#include "mbed.h"
#include "MenuItem.h"
#include "Menu.h"
#include "Globals.h"
#include "Display.h"
#include "CANCom.h"
#include <vector>
#include "heartbeat.h"
using namespace std;
using namespace std::chrono;

#define MBED_BOOT_STACK_SIZE  0x400

//For serial menu debug
#ifdef DISPLAY_DEBUG
    static BufferedSerial pc(USBTX,USBRX,115200); 
    char buf[1] = {0};
    void serialDebugInput();
#endif
#ifdef SIMPLE_DEBUG
    static BufferedSerial pc(USBTX,USBRX,115200);
#endif

int8_t testVar = 10;

void setupMenus();
void readInput();

//Variables for button and wheel input
Timer wheelDebounceTimer;
Timer btnDebounceTimer;
Timer doubleClickTimer;
bool enableBTNInput = true;
bool enableWheelInput = true;
bool buttonClickedOnce = false;

int main() {
    #ifdef DISPLAY_DEBUG
        printf("init\n");
    #endif
    #ifdef SIMPLE_DEBUG
        printf("init\n");
    #endif
    setupMenus();
    Display display;
    CANCom canRX;
    canRX.setup();
    systemTimeTimer.start();
    heartbeat unit_status(&databus, canStatusReq_ID_OCU);

    Timer displayUpdateTimer;
    displayUpdateTimer.start();

    showNotificationPopup = false; //Force set

    //Temporary status12V_Auto can messages
    Timer status12V_Auto_UpdateTimer;
    status12V_Auto_UpdateTimer.start();
    Timer switch_status12V_Auto_UpdateTimer;

    //read out configuration of CAN master control register
    uint32_t* CAN_MCR = (uint32_t*)0x40006400;
    //set retry  (ABOM bit in CAN_MCR) to let system reset TEC and REC after bus-off
    *CAN_MCR |= 1U << 6;

    while(1){
        #ifdef DISPLAY_DEBUG
            serialDebugInput(); //Update serial input
        #endif
        readInput(); //Read button and wheel input
        canRX.updateVariables();
        activeMenu->updatePeiodicCAN();
        activeMenu->updateActions(); //Update menu (input, buttons etc)
        unit_status.update();

        //Reset all units status variables if they have not sent a heartbeat within the status timeout
        for(uint8_t i=0; i<nbrOfOtherUnitsInSystem; i++){
            if(time(NULL)-last_unit_update_time[i] > status_timeout){
                if(UNIT_status[i] == OK_state){
                    UNIT_status[i] = NC_state; //Not connected
                    unit_state_change_NC = true;
                }  
            }
        }
        
        //Construct and update display efter a specified tinme (refresh rate)
        if(duration_cast<milliseconds>(displayUpdateTimer.elapsed_time()).count() > updateTime){
            activeMenu->updateMenu(); //Update menu output (format display putput etc)
            display.updateDisplay(); //Update display
            displayUpdateTimer.reset();
            //printf("UpdateDisp\n");
        }

        uint8_t temp_msg = 4;
        databus.write(CANMessage(1319,&temp_msg,1)); 

        //Send temporary status12V_Auto messages
        if(duration_cast<milliseconds>(status12V_Auto_UpdateTimer.elapsed_time()).count() > status12V_Auto_Update_Time){
            status12V_Auto_UpdateTimer.reset();
            databus.write(CANMessage(status12V_Auto_TEMP_ID,&status12V_Auto,1)); 
        }

        if(STM_12_INDIC > 0.4 && status12V_Auto == 0){
            if(duration_cast<milliseconds>(switch_status12V_Auto_UpdateTimer.elapsed_time()).count() == 0){
                switch_status12V_Auto_UpdateTimer.start();
            }
            else if(duration_cast<milliseconds>(switch_status12V_Auto_UpdateTimer.elapsed_time()).count() > status12V_Auto_variable_update_delay){
                status12V_Auto = 1;
                switch_status12V_Auto_UpdateTimer.stop();
                switch_status12V_Auto_UpdateTimer.reset();
            }
        }
        else if(STM_12_INDIC < 0.4 && status12V_Auto == 1){
            if(duration_cast<milliseconds>(switch_status12V_Auto_UpdateTimer.elapsed_time()).count() == 0){
                switch_status12V_Auto_UpdateTimer.start();
            }
            else if(duration_cast<milliseconds>(switch_status12V_Auto_UpdateTimer.elapsed_time()).count() > status12V_Auto_variable_update_delay){
                status12V_Auto = 0;
                switch_status12V_Auto_UpdateTimer.stop();
                switch_status12V_Auto_UpdateTimer.reset();
            }
        }
        else if(duration_cast<milliseconds>(switch_status12V_Auto_UpdateTimer.elapsed_time()).count() > 0){
            switch_status12V_Auto_UpdateTimer.stop();
            switch_status12V_Auto_UpdateTimer.reset();
        }

        thread_sleep_for(1);
    }
}

void setupMenus(){

//###### MAIN MENU ######
    mainMenuItems[0].setup(1,5);
    mainMenuItems[0].setMenuLinkId(scenarioMenu_ID);
    //mainMenuItems[1].setup(10,5);
    //mainMenuItems[1].setMenuLinkId(settingsMenu_ID);
    mainMenuItems[1].setup(17, 5);
    mainMenuItems[1].setMenuLinkId(statusMenu_ID);
    mainMenuItems[2].setup(42, 5);
    mainMenuItems[2].setMenuLinkId(powerMenu_ID);
    mainMenuItems[3].setup(49, 5);
    mainMenuItems[3].setMenuLinkId(logDataMenu_ID);
    mainMenuItems[4].setup(3,5); 
    mainMenuItems[4].setMenuLinkId(aboutMenu_ID);
    menus[mainMenu_ID] = Menu(mainMenu_ID,0,mainMenuItems,5,true,false); //Create main menu
    
    printf("A1\n\r");
    //CAN messages
    //menus[0].setPeriodicCanMsgUpdateTime(2500); //250 milliseconds between sending can messages
    //CANMessage* mainMenuCanMessages = new CANMessage[4];
    //mainMenuCanMessages[0] = CANMessage(1, &set_NCU_offset,1);
    //mainMenuCanMessages[1] = CANMessage(1, &set_NCU_left_limit,1);
    //mainMenuCanMessages[2] = CANMessage(1, &set_NCU_right_limit,1);
    //mainMenuCanMessages[3] = CANMessage(1, &request_NCU_variables,1);
    //menus[0].setPeriodicCanMsg(mainMenuCanMessages,4);
    

//#######################

//#### SCENARIO MENU ####  
    for(uint8_t i=0; i<nbrOfCanScenarios; i++){
        scenarioMenuItems[i].setup(53, 5);
        scenarioMenuItems[i].setMenuLinkId(mainMenu_ID);
    }
    
    menus[scenarioMenu_ID] = Menu(scenarioMenu_ID,1,scenarioMenuItems,0,true,false);
//#######################
    printf("A2\n\r");
//#### SETTINGS MENU ####
    /*settingsMenuItems[0].setup(11,2);
    settingsMenuItems[0].setItemDispInt32(&NCU_angle);
    settingsMenuItems[1].setup(7,3);
    settingsMenuItems[1].setItemDispFloat(&NCU_left_limit);
    settingsMenuItems[1].setFloatPrecision(5);
    settingsMenuItems[2].setup(8,3);
    settingsMenuItems[2].setItemDispFloat(&NCU_right_limit);
    settingsMenuItems[2].setFloatPrecision(5);
    settingsMenuItems[3].setup(9,2);
    settingsMenuItems[3].setItemDispInt32(&NCU_offset);
    menus[settingsMenu_ID] = Menu(settingsMenu_ID,10,settingsMenuItems,4,true,false);

    //CAN messages
    menus[settingsMenu_ID].setPeriodicCanMsgUpdateTime(250); //250 milliseconds between sending can messages
    CANMessage* settingsMenuCanMessages = new CANMessage[1];
    settingsMenuCanMessages[0] = CANMessage(request_NCU_variables_ID, &request_NCU_variables_msg,1);
    menus[settingsMenu_ID].setPeriodicCanMsg(settingsMenuCanMessages,1);*/
//#######################

//##### ABOUT MENU ######
    aboutMenuItems[0].setup(6, 6);
    menus[aboutMenu_ID] = Menu(aboutMenu_ID,3,aboutMenuItems,1,false,true);
//#######################

//#### REQ EXT POPUP ####
    reqExtMenuItems[0].setup(13, 6);
    menus[reqExtMenu_ID] = Menu(reqExtMenu_ID,16,reqExtMenuItems,1,false,true);

    //Return to previous menu (current scenario when external mode has been activated)
    menus[reqExtMenu_ID].setReturnOnExtModeActive(true);
    //CAN messages when returning (signal with buzzer that scenario has been started)
    CANMessage* extMenuManOrExtCanMessages = new CANMessage[1];
    extMenuManOrExtCanMessages[0] = CANMessage(canSoundBuzzer_ID_OCU,&startScenarioBuzzer_msg,1);
    reqExtMenuItems[0].setOnManOrExtCanMsg(extMenuManOrExtCanMessages,1);
//#######################

//#### REQ MAN POPUP ####
    reqManMenuItems[0].setup(36, 6);
    menus[reqManMenu_ID] = Menu(reqManMenu_ID,16,reqManMenuItems,1,false,true);
   
    //Return to previous menu (current scenario when manual mode has been activated)
    menus[reqManMenu_ID].setReturnOnManModeActive(true);
//#######################

//#### SYSTEM STATUS ####
    systemStatusMenuItems[0].setup(19, 7);
    systemStatusMenuItems[0].setItemDispStatus(&UNIT_status[ACU_unit_ID]);
    systemStatusMenuItems[1].setup(20, 7);
    systemStatusMenuItems[1].setItemDispStatus(&UNIT_status[RCU_unit_ID]);
    systemStatusMenuItems[2].setup(18, 7);
    systemStatusMenuItems[2].setItemDispStatus(&UNIT_status[PDU_unit_ID]);
    systemStatusMenuItems[3].setup(23, 7);
    systemStatusMenuItems[3].setItemDispStatus(&UNIT_status[TCU_unit_ID]);
    systemStatusMenuItems[4].setup(21, 7);
    systemStatusMenuItems[4].setItemDispStatus(&UNIT_status[NCU_unit_ID]);
    systemStatusMenuItems[5].setup(22, 7);
    systemStatusMenuItems[5].setItemDispStatus(&UNIT_status[UCU_unit_ID]);
    systemStatusMenuItems[6].setup(59, 5);
    systemStatusMenuItems[7].setup(60, 2);
    systemStatusMenuItems[7].setItemDispInt32(&ekfStatus);
    //systemStatusMenuItems[8].setup(61, 0); // Battery
    //systemStatusMenuItems[8].setItemDispStringIndex(26); //DONT FORGET TO CHANGE CANCom index if changing item order
    systemStatusMenuItems[8].setup(65, 2);
    systemStatusMenuItems[8].setItemDispInt32(&insStatus);

    menus[statusMenu_ID] = Menu(statusMenu_ID,17,systemStatusMenuItems,9,false,false);

    //CAN messages
    /*
    menus[statusMenu_ID].setPeriodicCanMsgUpdateTime(250); //250 milliseconds between sending can messages
    CANMessage* statusMenuCanMessages = new CANMessage[1];
    statusMenuCanMessages[0] = CANMessage(canStatusReq_ID, &status_req_msg,1);
    menus[statusMenu_ID].setPeriodicCanMsg(statusMenuCanMessages,1);
    */

//#######################
    printf("A3\n\r");

//#### STARTUP POPUP ####
    startupMenuItems[0].setup(35, 6);
    menus[startupMenu_ID] = Menu(startupMenu_ID,12,startupMenuItems,1,false,true);

    //Time limited menu
    menus[startupMenu_ID].setTimeLimit(startupMenu_TimeLimit);
//#######################

    //mainMenuItems[0] = MenuItem("Scenario",(uint8_t)4);
    //uint8_t uintVar = 1;
    //float floatVar = 1.0;
    
    //mainMenuItems[0].setItemDispInt(&uintVar);
    //mainMenuItems.setItemDispFloat(&floatVar);
    printf("A4\n\r");
    //mainMenuItems[1] = MenuItem("Settings",4);
    //mainMenuItems[1].setMenuLinkId(2);
    //mainMenuItems[2] = MenuItem("Logs",4); 
    //mainMenuItems[2].setMenuLinkId(3);

    //activeMenu = &menus[0]; //Set active menu pointer to main menu
    activeMenu = &menus[startupMenu_ID]; //Set active menu pointer to main menu
    printf("A5\n\r"); printf("A5.5\n\r");
    previousActiveMenuStack.push(&menus[mainMenu_ID]);
    printf("A6\n\r");
    printf("A6.1\n\r");
//#### SHUTDOWN MENU ####
    shutdownMenuItems[0].setup(38, 5);
    shutdownMenuItems[0].setMenuLinkId(ACUrebootMenu_ID);
    shutdownMenuItems[1].setup(39, 5);
    shutdownMenuItems[1].setMenuLinkId(ACUshutdownMenu_ID);
    shutdownMenuItems[2].setup(56, 5);
    shutdownMenuItems[2].setMenuLinkId(DBWrebootMenu_ID);
    shutdownMenuItems[3].setup(40, 5);
    shutdownMenuItems[3].setMenuLinkId(SYSrebootMenu_ID);
    shutdownMenuItems[4].setup(41, 5);
    shutdownMenuItems[4].setMenuLinkId(SYSshutdownMenu_ID);
    menus[powerMenu_ID] = Menu(powerMenu_ID,42,shutdownMenuItems,5,true,false);

    //CAN messages on green click
    CANMessage* shutdownMenuGreCanMessages = new CANMessage[5];
    shutdownMenuGreCanMessages[0] = CANMessage(canACUPower_ID_OCU,&ACUreboot_msg,1);
    shutdownMenuGreCanMessages[1] = CANMessage(canACUPower_ID_OCU,&ACUshutdown_msg,1);
    shutdownMenuGreCanMessages[2] = CANMessage(canPDUPower_ID_OCU,&DBWreboot_msg,1);
    shutdownMenuGreCanMessages[3] = CANMessage(canPDUPower_ID_OCU,&SYSreboot_msg,1);
    shutdownMenuGreCanMessages[4] = CANMessage(canPDUPower_ID_OCU,&SYSshutdown_msg,1);
    shutdownMenuItems[0].setOnGreClickCanMsg(&shutdownMenuGreCanMessages[0],1);
    shutdownMenuItems[1].setOnGreClickCanMsg(&shutdownMenuGreCanMessages[1],1);
    shutdownMenuItems[2].setOnGreClickCanMsg(&shutdownMenuGreCanMessages[2],1);
    shutdownMenuItems[3].setOnGreClickCanMsg(&shutdownMenuGreCanMessages[3],1);
    shutdownMenuItems[4].setOnGreClickCanMsg(&shutdownMenuGreCanMessages[4],1);
//#######################

//## ACU REBOOT POPUP ###
    ACUrebootMenuItems[0].setup(43, 6);
    menus[ACUrebootMenu_ID] = Menu(ACUrebootMenu_ID,16,ACUrebootMenuItems,1,false,true);
    menus[ACUrebootMenu_ID].setTimeLimit(ACU_powerMenu_TimeLimit);
//#######################

//## ACU SHUTDOWN CMD ###
    ACUshutdownMenuItems[0].setup(44, 6);
    menus[ACUshutdownMenu_ID] = Menu(ACUshutdownMenu_ID,16,ACUshutdownMenuItems,1,false,true);
    menus[ACUshutdownMenu_ID].setTimeLimit(ACU_powerMenu_TimeLimit);
//#######################

//## DBW REBOOT POPUP ###
    DBWrebootMenuItems[0].setup(57, 6);
    menus[DBWrebootMenu_ID] = Menu(DBWrebootMenu_ID,16,DBWrebootMenuItems,1,false,true);
//#######################

//## SYS REBOOT POPUP ###
    SYSrebootMenuItems[0].setup(45, 6);
    menus[SYSrebootMenu_ID] = Menu(SYSrebootMenu_ID,16,SYSrebootMenuItems,1,false,true);
//#######################

//## SYS SHUTDOWN CMD ###
    SYSshutdownMenuItems[0].setup(46, 6);
    menus[SYSshutdownMenu_ID] = Menu(SYSshutdownMenu_ID,16,SYSshutdownMenuItems,1,false,true);
//#######################

//# UNIT STATE CHANGE, NC #
    NCstateChangeMenuItems[0].setup(47, 6);
    menus[NCstateChangeMenu_ID] = Menu(NCstateChangeMenu_ID,16,NCstateChangeMenuItems,1,false,true);

    //Time limited menu
    menus[NCstateChangeMenu_ID].setTimeLimit(stateChange_TimeLimit);

    //CAN messages on enter 
    CANMessage* stateChangeCanMessages = new CANMessage[1];
    stateChangeCanMessages[0] = CANMessage(canSoundBuzzer_ID_OCU,&stateChange_msg,1);
    menus[NCstateChangeMenu_ID].setOnEnterCanMsg(stateChangeCanMessages,1);
//#########################

//# UNIT STATE CHANGE, ER #
    ERstateChangeMenuItems[0].setup(48, 6);
    menus[ERstateChangeMenu_ID] = Menu(ERstateChangeMenu_ID,16,ERstateChangeMenuItems,1,false,true);

    //Time limited menu
    menus[ERstateChangeMenu_ID].setTimeLimit(stateChange_TimeLimit);

    //CAN messages on enter 
    menus[ERstateChangeMenu_ID].setOnEnterCanMsg(stateChangeCanMessages,1);
//#########################

//#### LOGGING MENU #####
    loggingMenuMenuItems[0].setup(51, 0); //Start log command
    loggingMenuMenuItems[0].setItemDispStringIndex(14);
    loggingMenuMenuItems[1].setup(51, 0); //Stop log command
    loggingMenuMenuItems[1].setItemDispStringIndex(52);
    loggingMenuMenuItems[2].setup(27, 4); //Show logging state, on off
    loggingMenuMenuItems[2].setItemDispSetting(&loggingState);
    loggingMenuMenuItems[2].setSelectable(false);
    loggingMenuMenuItems[3].setup(50, 2); //Show logging marker
    loggingMenuMenuItems[3].setItemDispInt32(&loggingMarker);
    loggingMenuMenuItems[3].setSelectable(false);

    menus[logDataMenu_ID] = Menu(logDataMenu_ID,49,loggingMenuMenuItems,4,true,false);

    //CAN messages on start
    CANMessage* loggingStartCanMessages = new CANMessage[1];
    loggingStartCanMessages[0] = CANMessage(canLogData_ID,&startToLog_msg,1);
    loggingMenuMenuItems[0].setOnGreClickCanMsg(loggingStartCanMessages,1);

    //CAN messages on stop
    CANMessage* loggingStopCanMessages = new CANMessage[1];
    loggingStopCanMessages[0] = CANMessage(canLogData_ID,&stopToLog_msg,1);
    loggingMenuMenuItems[1].setOnGreClickCanMsg(loggingStopCanMessages,1);

//#######################

//#### CAN SCENARIOS ####
    for(uint8_t i=0; i<nbrOfCanScenarios; i++){
        canUpdateMenuItems[i][0].setup(27, 8); //Show scenario state
        canUpdateMenuItems[i][0].setItemDispStatus(&scenarioState);
        canUpdateMenuItems[i][0].setReqManualMode(true);
        canUpdateMenuItems[i][1].setup(33, 9); //Show timer
        canUpdateMenuItems[i][1].setItemDispStatus(&scenarioState);
        canUpdateMenuItems[i][2].setup(34, 10); //Show system time
        canUpdateMenuItems[i][3].setup(55, 11); //Display progress string
        canUpdateMenuItems[i][3].setDispCustomString(progressString);
        canUpdateMenuItems[i][4].setup(50, 2); //Show logging marker
        canUpdateMenuItems[i][4].setItemDispInt32(&loggingMarker);
        canUpdateMenuItems[i][5].setup(53, 11);
        canUpdateMenuItems[i][5].setDispCustomString(scenarioData_1[i]);
        canUpdateMenuItems[i][6].setup(53, 11);
        canUpdateMenuItems[i][6].setDispCustomString(scenarioData_2[i]);

        //CAN messages on green click
        canUpdateScenariosStart_msg[i] = i;
        CANMessage* canUpdateMenuGreCanMessages = new CANMessage[1];
        canUpdateMenuGreCanMessages[0] = CANMessage(canStartScenario_ID,&canUpdateScenariosStart_msg[i],1);
        canUpdateMenuItems[i][0].setOnGreClickCanMsg(canUpdateMenuGreCanMessages,1);
    }

    for(uint8_t i=0; i<nbrOfCanScenarios; i++){
        menus[canMenuStart_ID+i] = Menu(canMenuStart_ID+i,53,canUpdateMenuItems[i],nbrOfCanScenarioObjects-2,false,false);
    }
    printf("A8\n\r");
//#######################

//# NO SCENARIOS POPUP ##
    noScenarioMenuItems[0].setup(54, 6);
    menus[noScenariosMenu_ID] = Menu(noScenariosMenu_ID,16,noScenarioMenuItems,1,false,true);

    //Time limited menu
    menus[noScenariosMenu_ID].setTimeLimit(noScenarios_TimeLimit);
    printf("A9\n\r");
//#######################

//# NOTIFICATION POPUP ##
    notificationMessageMenuItems[0].setup(0,12); //programString not needed here, only item type
    menus[notificationMenu_ID] = Menu(notificationMenu_ID,16,notificationMessageMenuItems,1,false,true);
    printf("A9.5\n\r");

    //Time limited menu
    menus[notificationMenu_ID].setTimeLimit(notificationMessageMenu_TimeLimit);
    printf("A9.6\n\r");

    //CAN messages on enter 
    CANMessage* notificationCanMessages = new CANMessage[1];
    notificationCanMessages[0] = CANMessage(canSoundBuzzer_ID_OCU,&stateChange_msg,1);
    menus[notificationMenu_ID].setOnEnterCanMsg(notificationCanMessages,1);
    printf("A10\n\r");
//#######################

//# SYS PWR EXT MODE POPUP #
    sysPwrExtPopupMenuItems[0].setup(58, 6);
    menus[sysPwrExtPopupMenu_ID] = Menu(sysPwrExtPopupMenu_ID,16,sysPwrExtPopupMenuItems,1,false,true);

    //Time limited menu
    menus[sysPwrExtPopupMenu_ID].setTimeLimit(noScenarios_TimeLimit);
//##########################

}

#ifdef DISPLAY_DEBUG
void serialDebugInput(){
    if(pc.readable()){
        if(pc.read(buf, sizeof(buf))){
            if(*buf=='8'){scrWheelUp=true;}
            else if(*buf=='2'){scrWheelDown=true;}
            else if(*buf=='6'){greBtnClicked=true;}
            else if(*buf=='4'){redBtnClicked=true;}
        }  
    } 
}   
#endif

void readInput(){
    
    //Read buttons
    uint8_t BTN_GRE_reading = BTN_GRE.read();
    uint8_t BTN_RED_reading = BTN_RED.read();
    //##########################################################################################
    if(enableBTNInput && BTN_GRE_reading==1 && duration_cast<milliseconds>(btnDebounceTimer.elapsed_time()).count() == 0){
        greBtnClicked=true;
        enableBTNInput=false;
        btnDebounceTimer.start();
        if(duration_cast<milliseconds>(doubleClickTimer.elapsed_time()).count() == 0){
            doubleClickTimer.start();
            buttonClickedOnce = true;
        }
        else if(buttonClickedOnce && duration_cast<milliseconds>(doubleClickTimer.elapsed_time()).count() <= doubleClickTime){
            doubleClickedButton = true;
            buttonClickedOnce = false;
            doubleClickTimer.stop(); doubleClickTimer.reset();    
        }
    }
    else if(enableBTNInput && BTN_RED_reading==1 && duration_cast<milliseconds>(btnDebounceTimer.elapsed_time()).count() == 0){
        redBtnClicked=true;
        enableBTNInput=false;
        btnDebounceTimer.start();
        if(duration_cast<milliseconds>(doubleClickTimer.elapsed_time()).count() == 0){
            doubleClickTimer.start();
            buttonClickedOnce = true;
        }
        else if(buttonClickedOnce && duration_cast<milliseconds>(doubleClickTimer.elapsed_time()).count() <= doubleClickTime){
            doubleClickedButton = true;
            buttonClickedOnce = false;
            doubleClickTimer.stop(); doubleClickTimer.reset();    
        }
    }
    else if(!enableBTNInput && BTN_GRE_reading==0 && BTN_RED_reading==0 && duration_cast<milliseconds>(btnDebounceTimer.elapsed_time()).count() > btnDebounceTime){
        enableBTNInput=true;
        btnDebounceTimer.stop(); btnDebounceTimer.reset();
    }
    else if(duration_cast<milliseconds>(doubleClickTimer.elapsed_time()).count() > doubleClickTime){
        doubleClickTimer.stop(); doubleClickTimer.reset();
    }

    //##########################################################################################
    
    //Read wheel (now two buttons on one single input), sample multiple times
    float WHEEL_reading = 0;
    for(uint8_t i = 0; i<5; i++){
        WHEEL_reading += WHEEL.read();
    }
    WHEEL_reading = WHEEL_reading/5;
    
    //##########################################################################################
    if(enableWheelInput && WHEEL_reading > wheelMiddle && WHEEL_reading < wheelMiddle+wheelUpInput && duration_cast<milliseconds>(wheelDebounceTimer.elapsed_time()).count() == 0){ //If input enabled and wheel up, then block input
        enableWheelInput=false;
        scrWheelUp=true;
        wheelDebounceTimer.start();
        //printf("value: %f",WHEEL_reading);
    }
    else if(enableWheelInput && WHEEL_reading < wheelMiddle && WHEEL_reading > wheelMiddle-wheelDownInput && duration_cast<milliseconds>(wheelDebounceTimer.elapsed_time()).count() == 0){ //wheel down
        enableWheelInput=false;
        scrWheelDown=true;
        wheelDebounceTimer.start();
        //printf("value: %f",WHEEL_reading);
    }
    else if(!enableWheelInput && WHEEL_reading < wheelNoInput && duration_cast<milliseconds>(wheelDebounceTimer.elapsed_time()).count() > wheelDebounceTime){
        enableWheelInput = true;
        wheelDebounceTimer.stop(); wheelDebounceTimer.reset();
    }

    //##########################################################################################
    
}