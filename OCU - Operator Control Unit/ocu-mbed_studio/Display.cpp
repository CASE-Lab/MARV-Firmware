#include "Display.h"
#include "Globals.h"
#include "Types.h"
#include "mbed.h"
#include <cstdio>
#include <cstring>
#include "Menu.h"
#include "spk_oled_gfx.h"

Display::Display():
screen(DISP_DIN_pin, DISP_SCLK_pin, DISP_CS_pin, DISP_DC_pin, DISP_RST_pin){
    screen.fontStartCharacter = &characterBytesStartChar;
    screen.fontEndCharacter = &characterBytesEndChar;
    screen.fontCharacters = characterBytes;
}

void Display::updateDisplay(){
    /*#ifdef DISPLAY_DEBUG
        //serial_port
        //for(uint8_t i=0; i<)
        printf("|"); for(uint8_t i=0;i<nbrOfMenuCols;i++){printf("-");} printf("|\n");
        for(uint8_t i=0;i<nbrOfMenuRows;i++){
            printf("|"); printf("%.*s",nbrOfMenuCols,completeMenu[i]); printf("|\n");
        }
        printf("|"); for(uint8_t i=0;i<nbrOfMenuCols;i++){printf("-");} printf("|\n");
    #endif*/

    //Clear screen buffer
    screen.clearBuffer();
    //Box in menus
    if(activeMenu != &menus[startupMenu_ID]){ //Custom startup screen
        screen.horizLineToBuffer(0);
        screen.horizLineToBuffer(8);
        screen.vertLineToBuffer(0);
        screen.vertLineToBuffer(63);
        screen.horizLineToBuffer(127);
    }

    //Write header
    screen.characterArrayToBuffer(completeMenu[0], 0, 1, 0, true);
    //screen.textToBuffer(std::string(completeMenu[0]).substr(0,nbrOfMenuCols), 0, 1, 2, true);

    //Wirte menu lines
    if(activeMenu->getCentering()){
        for(uint8_t i=1;i<nbrOfMenuRows;i++){
            screen.characterArrayToBuffer(completeMenu[i], i, 4, 0, true);
        }
    }
    else{
        for(uint8_t i=1;i<nbrOfMenuRows;i++){
            screen.characterArrayToBuffer(completeMenu[i], i, 4, 2, false);
        }
    }
    
    //Send to buffer
    screen.sendBuffer();

}
