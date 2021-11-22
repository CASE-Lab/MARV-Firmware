#ifndef CONFIG_H
#define CONFIG_H

//DEBUG
//----------------------------------------------------------------------------
//If defined the display is showed in terminal, preventing other debug messages
//#define DISPLAY_DEBUG
//If defined some simple debug messages are shown
#define SIMPLE_DEBUG
//----------------------------------------------------------------------------

//Units & Status
//----------------------------------------------------------------------------
#define nbrOfOtherUnitsInSystem 6
#define ACU_unit_ID             0
#define RCU_unit_ID             1
#define PDU_unit_ID             2
#define TCU_unit_ID             3
#define NCU_unit_ID             4
#define UCU_unit_ID             5
#define status_timeout          3 //seconds
//----------------------------------------------------------------------------

//MENU
//----------------------------------------------------------------------------
#define nbrOfMenus              27
#define nbrOfCanScenarios       8
#define nbrOfCanScenarioObjects 7
#define reqExtMenu_TimeLimit    2500 //ms 
#define startupMenu_TimeLimit   5000 //ms 
#define ACU_powerMenu_TimeLimit 5000 //ms
#define stateChange_TimeLimit   3000 //ms
#define noScenarios_TimeLimit   4000 //ms
#define resetScenarioVariables_TimeLimit 5000 //ms
#define notificationMessageMenu_TimeLimit 4000 //ms
//MENU IDS
//----------------------------------------------------------------------------
#define mainMenu_ID             0
#define scenarioMenu_ID         1
#define aboutMenu_ID            2
//#define settingsMenu_ID         3
#define reqExtMenu_ID           3
#define statusMenu_ID           4
#define startupMenu_ID          5
#define reqManMenu_ID           6
#define powerMenu_ID            7
#define ACUrebootMenu_ID        8
#define ACUshutdownMenu_ID      9
#define DBWrebootMenu_ID        10
#define SYSrebootMenu_ID        11
#define SYSshutdownMenu_ID      12
#define NCstateChangeMenu_ID    13
#define ERstateChangeMenu_ID    14
#define logDataMenu_ID          15
#define noScenariosMenu_ID      16
#define notificationMenu_ID     17
#define sysPwrExtPopupMenu_ID   18
#define canMenuStart_ID         19
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//PINS
//----------------------------------------------------------------------------
//External mode activated/deativated indicator
#define STM_12_INDIC_pin        PA_1

//RX and TX
#define RX2_pin                 PA_15
#define TX2_pin                 PA_2

//Display
#define DISP_DC_pin             PA_3
#define DISP_CS_pin             PA_4
#define DISP_SCLK_pin           PA_5
#define DISP_RST_pin            PA_6
#define DISP_DIN_pin            PA_7

//Buttons and scroll wheel input
#define WHEEL_pin               PA_0
#define BTN_GRE_pin             PB_7
#define BTN_RED_pin             PB_6
//----------------------------------------------------------------------------

//CAN COMMUNICATION
//----------------------------------------------------------------------------
#define canBufSize              32
#define canStatusReq_ID_ACU     1281 //Message ID 40 ACU
#define canStatusReq_ID_RCU     1282 //Message ID 40 RCU
#define canStatusReq_ID_PDU     1283 //Message ID 40 PDU
#define canStatusReq_ID_TCU     1284 //Message ID 40 TCU
#define canStatusReq_ID_NCU     1285 //Message ID 40 NCU
#define canStatusReq_ID_UCU     1286 //Message ID 40 UCU
#define canStatusReq_ID_OCU     1287 //Message ID 40 OCU
#define canStartScenario_ID     647  //Message ID 20 OCU
#define canStateScenario_ID     673  //Message ID 21 ACU
#define canSoundBuzzer_ID_OCU   711  //Message ID 22 OCU 
#define canACUPower_ID_OCU      583  //Message ID 18 OCU         
#define canPDUPower_ID_OCU      615  //Message ID 19 OCU
#define canLogData_ID           743  //Message ID 23 OCU
#define canLoggingMarker_ID     769  //Message ID 24 ACU
#define canLoggingState_ID      801  //Message ID 25 ACU
#define canSystemTime_ID        737  //Message ID 23 ACU
#define canScenarioConfig_ID    929  //Message ID 29 ACU
#define canNotifiMsg_ID         897  //Message ID 28 ACU
#define canEkfStatus_ID         833  //Message ID 26 ACU
#define canLog2PDU_ID           995  //Message ID 34 PDU
#define insStatus_ID            865  //Message ID 30 ACU
#define status12V_Auto_TEMP_ID  39   //Message ID 1 OCU

//----------------------------------------------------------------------------

//DISPLAY SETTINGS
//----------------------------------------------------------------------------
#define nbrOfMenuCols           11
#define nbrOfMenuRows           16
#define updateTime              75  //Display refresh time
#define newLineLength           5
//----------------------------------------------------------------------------

//BUTTON AND WHEEL CONFIG
//----------------------------------------------------------------------------
#define wheelNoInput            0.10
#define wheelDownInput          0.01
#define wheelUpInput            0.12          
#define wheelMiddle             0.39
#define btnDebounceTime         250
#define doubleClickTime         750
#define wheelDebounceTime       100
//----------------------------------------------------------------------------

//Defines string indexes
//----------------------------------------------------------------------------
#define ER_str                  24
#define OK_str                  25
#define NC_str                  26
#define WAITING_str             28
#define EXECUTING_str           29
#define FAULT_str               30
#define STOPPED_str             31
#define FINISHED_str            32
#define REQ_SENT_str            37
//----------------------------------------------------------------------------

//Defines scenario states
//----------------------------------------------------------------------------
#define WAITING                 0   //Waiting for external mode to start
#define EXECUTING               1   //Executing scenario
#define FAULT                   2   //Some error has occured
#define STOPPED                 3   //Scenario stopped
#define FINISHED                4   //Scenario has finished
#define REQ_SENT                5   //Request to start sent
#define scenarioStateTimeout    5000 //ms before automatically changing to another state
//----------------------------------------------------------------------------

//Defines unit states
#define ER_state                0
#define OK_state                1
#define NC_state                2
//----------------------------------------------------------------------------

//Defines can scenario config locations
//----------------------------------------------------------------------------
#define scenario_header         0
#define scenario_progress       1
#define scenario_data_header_1  2
#define scenario_data_1         3
#define scenario_data_header_2  4
#define scenario_data_2         5
//----------------------------------------------------------------------------

//Defines can notification message length
//----------------------------------------------------------------------------
#define notificationMessageLength 70
//----------------------------------------------------------------------------

//Defines temporary status12V_Auto can message constants
//----------------------------------------------------------------------------
#define status12V_Auto_Update_Time 100 //ms can message send time
#define status12V_Auto_variable_update_delay 200 //ms delay before updating the variable, to prevent race condition
//----------------------------------------------------------------------------

#endif //CONFIG_H