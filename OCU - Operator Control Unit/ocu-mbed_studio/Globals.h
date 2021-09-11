#ifndef GLOBALS_H
#define GLOBALS_H

//#include "lib.h" or <lib.h> 
#include "Config.h"
#include "mbed.h"
#include "Menu.h"
#include <string.h>
//#include <stack>
#include "fixstack.h"

//extern "variable"
extern char completeMenu[nbrOfMenuRows][nbrOfMenuCols];
extern Menu* menus;
extern const char* const programStrings[];
extern Menu *activeMenu;
//extern std::stack<Menu*> previousActiveMenuStack;
extern fixstack previousActiveMenuStack;

//Unit Variables
extern int32_t NCU_angle;
extern int32_t NCU_offset;
extern float NCU_left_limit;
extern float NCU_right_limit;
extern char set_NCU_offset;
extern char set_NCU_left_limit;
extern char set_NCU_right_limit;
extern uint8_t request_NCU_variables_ID;
extern char request_NCU_variables_msg;

extern CAN databus;
extern Timer periodicCanMsgTimer;

extern CANMessage canBuf[canBufSize];
extern uint32_t canBufWriteIndex;

extern volatile bool greBtnClicked;
extern volatile bool redBtnClicked;
extern volatile bool scrWheelDown;
extern volatile bool scrWheelUp;
extern volatile bool doubleClickedButton;

extern AnalogIn STM_12_INDIC;
extern DigitalIn BTN_GRE;
extern DigitalIn BTN_RED;
extern AnalogIn WHEEL;

extern Timer timeLimitTimer;

extern char loggingScenarioStart_msg;  

extern char loggingScenarioStop_msg;

extern char startScenarioBuzzer_msg;

extern char ACUreboot_msg;
extern char ACUshutdown_msg;
extern char DBWreboot_msg;
extern char SYSreboot_msg;
extern char SYSshutdown_msg;

extern char stateChange_msg;

extern uint8_t status_req_msg;
extern int32_t UNIT_status[nbrOfOtherUnitsInSystem];
extern time_t last_unit_update_time[nbrOfOtherUnitsInSystem];
extern bool unit_state_change_ER;
extern bool unit_state_change_NC;
extern bool onEnterCanMsgSend;
extern int32_t ekfStatus;
extern int32_t insStatus;

extern int32_t scenarioState;
extern Timer menuItemTimer;
extern uint32_t systemTime;
extern Timer systemTimeTimer; 
extern bool popupShownOnce;
extern Timer stateResetTimer;
extern bool faultStateSignalSent;
extern char faultStateBuzzer_msg;
extern bool finishedStateSignalSent;
extern char finishedStateBuzzer_msg;

extern int32_t loggingState;
extern int32_t loggingMarker;
extern char startToLog_msg;
extern char stopToLog_msg;

extern char menuHeaderCAN[nbrOfCanScenarios][8];
extern char progressString[8];
extern bool scenarioVariablesReset;
extern Timer resetScenarioVariablesTimer;
extern char canUpdateScenariosStart_msg[nbrOfCanScenarios];
extern char scenarioDataHeader_1[nbrOfCanScenarios][8];
extern char scenarioData_1[nbrOfCanScenarios][8];
extern char scenarioDataHeader_2[nbrOfCanScenarios][8];
extern char scenarioData_2[nbrOfCanScenarios][8];

extern MenuItem mainMenuItems[5];
extern MenuItem scenarioMenuItems[2];
//extern MenuItem settingsMenuItems[4];
extern MenuItem aboutMenuItems[1];
extern MenuItem reqExtMenuItems[1];
extern MenuItem reqManMenuItems[1];
extern MenuItem loggingScenarioMenuItems[3];
extern MenuItem systemStatusMenuItems[9];
extern MenuItem startupMenuItems[1];
extern MenuItem shutdownMenuItems[5];
extern MenuItem ACUrebootMenuItems[1];
extern MenuItem ACUshutdownMenuItems[1];
extern MenuItem DBWrebootMenuItems[1];
extern MenuItem SYSrebootMenuItems[1];
extern MenuItem SYSshutdownMenuItems[1];
extern MenuItem NCstateChangeMenuItems[1];
extern MenuItem ERstateChangeMenuItems[1];
extern MenuItem loggingMenuMenuItems[4];
extern MenuItem canUpdateMenuItems[nbrOfCanScenarios][nbrOfCanScenarioObjects];
extern MenuItem noScenarioMenuItems[1];
extern MenuItem notificationMessageMenuItems[1];
extern MenuItem sysPwrExtPopupMenuItems[1];

extern char notificationMessage[notificationMessageLength];
extern char notification_msg;
extern bool showNotificationPopup;

extern uint8_t status12V_Auto;

#endif //GLOBALS_H