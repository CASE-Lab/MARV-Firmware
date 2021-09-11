#include "mbed.h"
#include "heartbeat.h"

#define  sbusOffset  -172
#define  sbusMultiplier  1000/(1811+sbusOffset)

const uint32_t stackSize = 3;
const uint32_t maxTime = 150; //us
constexpr auto failsafeMaxTime = 250000us; //ms (converted to us)
const uint32_t failsafeMessageInterval = 1000; //ms

const uint32_t canBufSize = 3;
const uint32_t canFilterId = 0x01;
const uint32_t canFilterMask = 0x0f;
const uint32_t canMessageIDData1 = 34;
const uint32_t canMessageIDData2 = 66;
const uint16_t canHeartbeatID = 1282;

UnbufferedSerial sbus(PA_9,PA_10);  //tx, rx
BufferedSerial pc(USBTX, USBRX); // tx, rx

CAN databus1(PA_11, PA_12,1000000);     // CAN Interface for databus 1 (communication between RCU/ICU).
DigitalOut RsPin(PA_9);                 // CAN Tranceiver slew rate limitation

// Global variables for CAN
CANMessage canBuf[canBufSize];
volatile uint32_t canBufWriteIndex=0;

// Heartbeat
Heartbeat canHeartbeat(&databus1, canHeartbeatID);

Timer packTimer;
Timer failsafeTimer;
Timer failsafeMessageTimer;
uint8_t packIndex=0;
volatile uint8_t stackIndexW=0;
char stack[stackSize][25];

void canRxIsr()
{
    if(databus1.read(canBuf[canBufWriteIndex])) {
        canBufWriteIndex++;
        if (canBufWriteIndex==canBufSize)
            canBufWriteIndex=0;
        }
}

void isrPack()
{
    while(sbus.readable()) {
        //uint8_t c = sbus.getc(); getc() is depreceated
        uint8_t byte;
        sbus.read(&byte, 1);
        uint32_t currTime = packTimer.read_us();
        if( (currTime > maxTime)) {   // Check if new package
            if(packIndex==25) {
                stackIndexW++;  //Only write new frame if all bytes were received.
                if (stackIndexW==stackSize) {
                    stackIndexW=0;
                }
            }
            packIndex=0;
        } else {
        }
        //If not new package, fill current package
        stack[stackIndexW][packIndex]=byte;
        packIndex++;
        packTimer.reset();
    }
}

bool decodeSBus(char data[], uint16_t aChannel[], bool dChannel[], bool * failsafe, bool * frameLost)
{

    bool validMessage = (data[0]==0x0F) && (data[24]==0x00);
    // Check that
    if (validMessage) {
        aChannel[0] = data[1] + (uint16_t(data[2]&0b00000111)<<8);
        aChannel[1] = ((data[2]&0b11111000)>>3) + (uint16_t(data[3]&0b00111111)<<5);
        aChannel[2] = ((data[3]&0b11000000)>>6) + (uint16_t(data[4]&0b11111111)<<2) + (uint16_t(data[5]&0b00000001)<<10);
        aChannel[3] = ((data[5]&0b11111110)>>1) + (uint16_t(data[6]&0b00001111)<<7);
        aChannel[4] = ((data[6]&0b11110000)>>4) + (uint16_t(data[7]&0b01111111)<<4);
        aChannel[5] = ((data[7]&0b10000000)>>7) + (uint16_t(data[8]&0b11111111)<<1) + (uint16_t(data[9]&0b00000011)<<9);
        aChannel[6] = ((data[9]&0b11111100)>>2) + (uint16_t(data[10]&0b00011111)<<6);
        aChannel[7] = ((data[10]&0b11100000)>>5) + (uint16_t(data[11]&0b11111111)<<3);
        aChannel[8] = data[12] + (uint16_t(data[13]&0b00000111)<<8);
        aChannel[9] = ((data[13]&0b11111000)>>3) + (uint16_t(data[14]&0b00111111)<<5);
        aChannel[10] = ((data[14]&0b11000000)>>6) + (uint16_t(data[15]&0b11111111)<<2) + (uint16_t(data[16]&0b00000001)<<10);
        aChannel[11] = ((data[16]&0b11111110)>>1) + (uint16_t(data[17]&0b00001111)<<7);
        aChannel[12] = ((data[17]&0b11110000)>>4) + (uint16_t(data[18]&0b01111111)<<4);
        aChannel[13] = ((data[18]&0b10000000)>>7) + (uint16_t(data[19]&0b11111111)<<1) + (uint16_t(data[20]&0b00000011)<<9);
        aChannel[14] = ((data[20]&0b11111100)>>2) + (uint16_t(data[21]&0b00011111)<<6);
        aChannel[15] = ((data[21]&0b11100000)>>5) + (uint16_t(data[22]&0b11111111)<<3);
        dChannel[1] = (data[23]&0b00000001) >> 0;
        dChannel[2] = (data[23]&0b00000010) >> 1;
         
        *frameLost = (data[23]&0b00000100) >> 2;
        *failsafe = (data[23]&0b00001000) >> 3;
        
        for (int i=0; i<16; i++) {
            aChannel[i]=(aChannel[i]+sbusOffset)*sbusMultiplier;
        }
        printf("%4d %4d %4d  \n\r",aChannel[0], aChannel[1], aChannel[2]);
    }

    // Nollställ start- och stopbytes för att se så att hela meddelandet skrivs nästa gång
    data[0]=0xf0;
    data[24]=0xff;
    return validMessage;
}

void init()
{
    sbus.format(8,SerialBase::Even,2);
    sbus.baud(100000);
    sbus.attach(&isrPack);
    
    packTimer.start();                  // Start timer
    failsafeTimer.start();                  // Start timer
    failsafeMessageTimer.start();                  // Start timer
    
    RsPin = 0; // Disable slew rate limitation
    databus1.attach(&canRxIsr,CAN::RxIrq);
    databus1.filter(canFilterId,canFilterMask,CANFormat(CANStandard),0); //filters for can msg id
}

// Function for printing a CANMessage object to a Serial port object (passed to function by pointers)
void canPrintMsg(CANMessage* msg){
    debug("ID %d : ",msg->id);
    for(int i=0; i<msg->len ; i++) {
        debug("%02x ",msg->data[i]);
    }
    debug("\n\r");
}

bool varannan=true; 
int main(){
    //CAN
    bool canStatusReq=false;
    uint32_t canBufReadIndex=0;
    char status=2;  //starts at idle state (1 indicates running, 0 indicates failsafe)

    //SBUS
    int stackIndexR=0;
    uint16_t aChannel[16];
    bool dChannel[2];
    bool failsafe;
    bool frameLost;
    bool newSbusData=false;

    init();
    while(1) {
        //Decode incoming message
        if (canBufReadIndex != canBufWriteIndex) {  //Check if buffer indices are different which indicates that the position at the read index has been filled.
            switch (canBuf[canBufReadIndex].id) {
                case canFilterId: //Status request, respond with ok
                    canStatusReq=true;
                    break;
                default : //Not a message to the RCU..
                    break;
            }
            if(++canBufReadIndex == canBufSize) //Increment buffer index and check for 'wrap around'.
                canBufReadIndex = 0;
        }
        
        //Respond to status message
        if (canStatusReq) {
            if (databus1.write(CANMessage(canFilterId+16,&status,1)))
                canStatusReq=false;
        }

        if (stackIndexR != stackIndexW) {   //If arrayindex (pointer) not synced -> new sbus data to decode
            if (decodeSBus(&stack[stackIndexR][0],aChannel,dChannel,&failsafe,&frameLost)) {
                if (!failsafe) {
                    failsafeTimer.reset();
                    status=1;
                    newSbusData=true;
                }
            }
            stackIndexR++;
            if (stackIndexR == stackSize) {
                stackIndexR=0;
            }
        }
        // failsafe not implemented on CAN DB2 yet
        if (failsafeTimer.elapsed_time() > failsafeMaxTime && (status == 1)) { //Send error message on can continously if in failsafe state.
            newSbusData=false;
            status=0;
            printf("ENTERED FAILSAFE MODE!\n\r");

        }
        //if new SbusData and every other iterate. (skip every other SbusData)
        if (newSbusData&&varannan) {
            //Send to can bus
            int16_t channelThree = ((int16_t)aChannel[2]-500)*5;

            aChannel[0]*=10; //scale according to db1 documentation.
            aChannel[1]*=10;
            char data[6];
            data[0] = (char)(aChannel[0]>>8);
            data[1] = (char)aChannel[0];
            data[2] = (char)(aChannel[1]>>8);
            data[3] = (char)aChannel[1];
            data[4] = (char)(channelThree>>8);
            data[5] = (char)channelThree;
            CANMessage msg1(canMessageIDData1,data,6);    
            
            if(databus1.write(msg1)){
                printf("CAN message1 sent\n\r");
                }
                else{
                    printf("Failed to send CAN message1\n\r");
                    }
                    
            newSbusData=false;
            canPrintMsg(&msg1);
        }
        varannan=!varannan;

        //poll heartbeat function:
        canHeartbeat.update();

        //set error if RCU status is failsafe and error is not current state for heartbeat
        if( (status == 0) && (canHeartbeat.getStatus() == heartbeatStatus::OK) )
            canHeartbeat.setStatus(heartbeatStatus::ERROR);

        if( (status == 1) && (canHeartbeat.getStatus() == heartbeatStatus::ERROR) )
            canHeartbeat.setStatus(heartbeatStatus::OK);
        
    }
}