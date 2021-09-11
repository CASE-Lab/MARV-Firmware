#include "Globals.h"
#include "Config.h"
#include "Menu.h"
#include <mbed.h>
#include <string.h>
//#include <stack>
#include "fixstack.h"
#include <vector>

//Variables
char completeMenu[nbrOfMenuRows][nbrOfMenuCols];
//Menu* menus = new Menu[nbrOfMenus];
Menu staticMenus[nbrOfMenus];
Menu* menus = staticMenus;
const char* const programStrings[] = {
    "MAIN MENU",                //0
    "SCENARIO",                 //1
    "LOGS",                     //2
    "ABOUT",                    //3
    "LEAVE HAR",                //4
    "Int Var",                  //5
    "AUTONOMOUS#WAVERUNNER#--------#NEW SYSTEM#CREATED BY#NOEL#DANIELSSON#AND#VIKTOR#LINDSTROM#--------#CHALMERS#2021#", //6
    "LEFT LMT",                 //7
    "RIGHT LMT",                //8
    "OFFSET",                   //9
    "SETTINGS",                 //10
    "ANGLE",                    //11
    "  ",                       //12
    "ACTIVATE#EXT MODE#TO#START#SCENARIO", //13
    "START",                    //14
    "LOG DATA",                 //15
    "#########",                //16
    "STATUS",                   //17
    "PDU",                      //18
    "ACU",                      //19
    "RCU",                      //20
    "NCU",                      //21
    "UCU",                      //22
    "TCU",                      //23
    "ER",                       //24
    "OK",                       //25
    "--",                       //26
    "STATE",                    //27
    "WAITING",                  //28
    "EXECUTING",                //29
    "ERROR",                    //30
    "STOPPED",                  //31
    "FINISHED",                 //32
    "TIMER",                    //33
    "SYS TIME",                 //34
    "# # #AUTONOMOUS#WAVERUNNER# # #CHALMERS#", //35
    "DEACTIVATE#EXT MODE#BEFORE#STARTING#A#SCENARIO#", //36
    "REQ SENT",                 //37
    "ACU RBOT",                 //38
    "ACU SHDN",                 //39
    "SYS RBOT",                 //40
    "SYS SHDN",                 //41
    "POWER",                    //42
    "ACU#REBOOT#REQUEST#SENT#", //43
    "ACU#SHUTDOWN#REQUEST#SENT#", //44
    "WAITING#FOR#SYSTEM#REBOOT#", //45
    "WAITING#FOR#SYSTEM#SHUTDOWN#", //46
    "STATE#CHANGE!#UNIT NO#LONGER#CONNECTED#", //47
    "STATE#CHANGE!#UNIT#SHOWING#ERROR!#", //48
    "LOGGING",                  //49
    "MARKER",                   //50
    "CMD",                      //51
    "STOP",                     //52
    "NOT SET",                  //53
    "THERE#ARE NO#SCENARIOS# #START THE#SCENARIO#HANDLER#", //54
    "PROGRESS",                 //55
    "DBW RBOT",                 //56
    "WAITING#FOR#DBW#REBOOT#",  //57
    "CANNOT#ACCESS#SYSTEM#POWER#OPTIONS#WHILE IN#EXTERNAL#MODE##DISABLE#EXTERNAL#MODE#", //58
    "===============",          //59
    "EKF",                      //60
    "BATTERY",                  //61
    "LOW",                      //62
    "GOOD",                     //63
    "CRITICAL",                 //64
    "INS: (HVP)",               //65
};

Menu *activeMenu; //Keep pointer of active menu
//std::stack<Menu*> previousActiveMenuStack; //Keep pointers of the previous active menu
fixstack previousActiveMenuStack;

//Settings variables
int32_t NCU_angle=0;
int32_t NCU_offset=0;
float NCU_left_limit=0.85;
float NCU_right_limit=0.25;
char set_NCU_offset=0x1;
char set_NCU_left_limit=0x2;
char set_NCU_right_limit=0x3;
uint8_t request_NCU_variables_ID = 62;
char request_NCU_variables_msg = 0x4;

//CAN
CAN databus(PA_11, PA_12, 1000000);
Timer periodicCanMsgTimer;

//CAN RX
CANMessage canBuf[canBufSize];
uint32_t canBufWriteIndex=0;

//Keep track of buttons
volatile bool greBtnClicked = false;
volatile bool redBtnClicked = false;
volatile bool scrWheelDown = false;
volatile bool scrWheelUp = false;
volatile bool doubleClickedButton = false;

//Pin configurations
AnalogIn STM_12_INDIC(STM_12_INDIC_pin);
DigitalIn BTN_GRE(BTN_GRE_pin);
DigitalIn BTN_RED(BTN_RED_pin);
AnalogIn WHEEL(WHEEL_pin);

//Limited time in menu Timer
Timer timeLimitTimer;

//Scenario start variables messages
char loggingScenarioStart_msg = 0x1;

//Scenario stop variable message
char loggingScenarioStop_msg = 0x1;

//Sound buzzer messages
char startScenarioBuzzer_msg = 0x1;

//Shutdown menu messages
char ACUreboot_msg = 0x1;
char ACUshutdown_msg = 0x2;
char DBWreboot_msg = 0x0;
char SYSreboot_msg = 0x1;
char SYSshutdown_msg = 0x2;

//State change message
char stateChange_msg = 0x1;

//Status variables
uint8_t status_req_msg=1;
int32_t UNIT_status[nbrOfOtherUnitsInSystem] = {NC_state,NC_state,NC_state,NC_state,NC_state,NC_state};
time_t last_unit_update_time[nbrOfOtherUnitsInSystem] = {0,0,0,0,0,0};
bool unit_state_change_ER = false;
bool unit_state_change_NC = false;
bool onEnterCanMsgSend = false;
int32_t ekfStatus = -1;
int32_t insStatus = -1;

//Scenario state
int32_t scenarioState=STOPPED; //For states see config file
Timer menuItemTimer;
uint32_t systemTime=0; //System time (s) from 00:00:00
Timer systemTimeTimer; //Keeps system time once synced with ACU
bool popupShownOnce = false;
Timer stateResetTimer;
bool faultStateSignalSent = false; //Keep track if a buzzer signal has been sent to the ocu, signaling fault scenario state
char faultStateBuzzer_msg = 0x2;
bool finishedStateSignalSent = false;
char finishedStateBuzzer_msg = 0x3;

//Logging menu variables
int32_t loggingState = 0; //The logging state, 0=off, 1=on (we are then placing markers to indicate the logging of data)
int32_t loggingMarker = -1; //Current logging marker, -1 when not logging, x when logging (increases with every new logging interval)
char startToLog_msg = 0x1; //Start logging can command
char stopToLog_msg = 0x0; //Stop logging can command

//CAN scenario variables
char menuHeaderCAN[nbrOfCanScenarios][8] = {0};
char progressString[8] = {'-','-'};
bool scenarioVariablesReset = true;
Timer resetScenarioVariablesTimer;
char canUpdateScenariosStart_msg[nbrOfCanScenarios];
char scenarioDataHeader_1[nbrOfCanScenarios][8];
char scenarioData_1[nbrOfCanScenarios][8] = {'-','-'};
char scenarioDataHeader_2[nbrOfCanScenarios][8];
char scenarioData_2[nbrOfCanScenarios][8] = {'-','-'};

//Menu Items
MenuItem mainMenuItems[5];
MenuItem scenarioMenuItems[2];
//MenuItem settingsMenuItems[4];
MenuItem aboutMenuItems[1];
MenuItem reqExtMenuItems[1];
MenuItem reqManMenuItems[1];
MenuItem loggingScenarioMenuItems[3];
MenuItem systemStatusMenuItems[9];
MenuItem startupMenuItems[1];
MenuItem shutdownMenuItems[5];
MenuItem ACUrebootMenuItems[1];
MenuItem ACUshutdownMenuItems[1];
MenuItem DBWrebootMenuItems[1];
MenuItem SYSrebootMenuItems[1];
MenuItem SYSshutdownMenuItems[1];
MenuItem NCstateChangeMenuItems[1];
MenuItem ERstateChangeMenuItems[1];
MenuItem loggingMenuMenuItems[4];
MenuItem canUpdateMenuItems[nbrOfCanScenarios][nbrOfCanScenarioObjects];
MenuItem noScenarioMenuItems[1];
MenuItem notificationMessageMenuItems[1];
MenuItem sysPwrExtPopupMenuItems[1];

//Notofication variables
char notificationMessage[notificationMessageLength] = {0};
char notification_msg = 0x1;
bool showNotificationPopup = false;

//Send temporary status12V_Auto messages
uint8_t status12V_Auto = 0;
