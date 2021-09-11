// OLED display using SSD1305 driver
// A library by *spark audio-visual

/* Copyright (c) 2011 Toby Harris, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
 
#include "spk_oled_ssd1305.h"
#include "mbed.h"
#include "Config.h"
#include <cstdio>

SPKDisplay::SPKDisplay(PinName mosiPin, PinName clkPin, PinName csPin, PinName dcPin, PinName resPin) :
    _cs(csPin),
    _dc(dcPin),
    _res(resPin),
    _spi(mosiPin, PB_4, clkPin){
    bufferHasChanged = false;
    
    fontStartCharacter = NULL;
    fontEndCharacter = NULL;
    fontCharacters = NULL;
    
    spi = &_spi;;
    spi->format(8,3);
    spi->frequency(2000000);
    
    cs = &_cs;
    dc = &_dc;
    res = &_res;
    
    // Link up debugSerial Serial object
    // Passing in shared object as debugSerialging is shared between all DVI mixer functions
    
    setup();
    
    clearBuffer();
    
    //printf("Display loaded\n\r");
}

void SPKDisplay::clearBuffer()
{
    memset(buffer, 0, bufferCount);
    bufferHasChanged = true;
}

void SPKDisplay::imageToBuffer(const uint8_t* image)
{
    memcpy(buffer, image, bufferCount);
    bufferHasChanged = true;
}

void SPKDisplay::clearBufferRow(int row)
{
    // Range check
    if (row >= nbrOfMenuRows)
    {
        printf("SPKDisplay::clearBufferRow sent out of bounds row");
        return;
    }
    int bStart = row*bufferWidth;
    int bEnd = bStart + pixelWidth;

    for (int bPos = bStart; bPos <= bEnd; bPos++)
    {
        buffer[bPos] = 0x00;
    }
    
    bufferHasChanged = true;
}

void SPKDisplay::vertLineToBuffer(int x)
{
    if (x >= pixelHeight)
    {
        printf("SPKDisplay::clearBufferRow sent out of bounds y");
        return;
    }
    
    int row = (x*pixInPage) / pixelHeight;
    int posInRow = x % pixInPage;
    
    int bStart = row*bufferWidth;
    int bEnd = bStart + pixelWidth;
    
    for (int bPos = bStart; bPos <= bEnd; bPos++)
    {
        // Need to bitwise OR as setting single bit (the line) in byte (the row)
        buffer[bPos] = buffer[bPos] | 0x01 << posInRow;
    } 
    
    bufferHasChanged = true;
}

void SPKDisplay::horizLineToBuffer(int y){
    for(uint8_t x=0;x<pixelWidth;x++){
        setPixel(buffer, getXCoord(y), getYCoord(x), 1);
    }
}

//void SPKDisplay::verticalLineToBuffer

void SPKDisplay::textToBuffer(std::string message, int row, uint8_t offsetY, uint8_t offsetX){
    // Font check
    if (NULL == fontCharacters) return;
    if (NULL == fontStartCharacter || NULL == fontEndCharacter) return;    

    int bPos = 0;
    
    for (int i = 0; i < message.size(); i++)
    {
        char character = message.at(i);
            
        characterToBuffer(character, bPos, row, offsetY, offsetX);
        
        //Shift into our array's indexing
        character -= *fontStartCharacter;
        bPos += fontCharacters[character][0]+1;
    }
    
    bufferHasChanged = true;
}

void SPKDisplay::characterArrayToBuffer(char* characterArray, int row, uint8_t offsetY, uint8_t offsetX, bool center){
    // Font check
    if (NULL == fontCharacters) return;
    if (NULL == fontStartCharacter || NULL == fontEndCharacter) return;    

    bool noBreak = true;

    if(center){
        uint8_t totalLength=0;
        for(uint8_t i=0;i<nbrOfMenuCols;i++){
            char currentCharacter = characterArray[i];
            currentCharacter -= *fontStartCharacter;

            totalLength += fontCharacters[currentCharacter][0]+1;

            if(i == nbrOfMenuCols-1){ //Odd case
                if(characterArray[i] == ' '){
                    totalLength -= fontCharacters[currentCharacter][0]+1;
                    offsetX = offsetX + (pixelHeight-totalLength)/2;
                    break;
                }
                else if(characterArray[i] != ' '){
                    offsetX = offsetX + (pixelHeight-totalLength)/2;
                    break;
                }
            }
            else{ //Even space
                if(characterArray[i] == ' ' && characterArray[i+1] == ' '){
                    totalLength -= fontCharacters[currentCharacter][0]+1;
                    offsetX = offsetX + (pixelHeight-totalLength)/2; 
                    break;
                }
            }
        }       
    }

    int bPos = 0;
    
    for (int i = 0; i < nbrOfMenuCols; i++)
    {
        char character = characterArray[i];
            
        characterToBuffer(character, bPos, row, offsetY, offsetX);
        
        //Shift into our array's indexing
        character -= *fontStartCharacter;
        bPos += fontCharacters[character][0]+1;
    }
    
    bufferHasChanged = true;
}

void SPKDisplay::characterToBuffer(char character, int x, int row, uint8_t offsetY, uint8_t offsetX)
{
        // Font check
    if (NULL == fontCharacters) return;
    if (NULL == fontStartCharacter || NULL == fontEndCharacter) return;    

    //Keep pixel pos
    int bPos = x;
        
    // Is it outside the range we have glyphs for?
    if ((character < *fontStartCharacter) || (character > *fontEndCharacter))
    {

        if (character != ' ') printf("No glyph for character %c", character);

    }
    // If not, typeset it!
    else 
    {
        // Shift into our array's indexing
        character -= *fontStartCharacter;
        
        // Write each byte's vertical column of 8bits into the buffer.
        // Write each byte's vertical column of 8 bits into the buffer horizontally (to have the display standing)

        for(uint8_t i=0;i<fontCharacters[character][0];i++){
            for(uint8_t j=0;j<8;j++){
                uint8_t pixel = readPixel(fontCharacters[character][i+1], j);
                if(pixel==1){
                    setPixel(buffer, getXCoord(row*8+j+offsetY), getYCoord(bPos+offsetX), 1);  
                }
            }
            bPos++;     
        }
        
    }

    bufferHasChanged = true;
}

void SPKDisplay::sendBuffer()
{

    if (bufferHasChanged)
    {
        // Select the device by seting chip select low
        *cs = 0;
        // Set to receive DATA not commands
        *dc = 1;
        for (int i = 0; i < bufferCount; i++)
        {
            spi->write(buffer[i]);
        }
        // Deselect the device
        *cs = 1;
        
        bufferHasChanged = false;
    }
}

void SPKDisplay::setup()
{
    // TASK: SCREEN OFF, Run pre-flight
    
    // Hard reset the OLED
    *res = 0;
    thread_sleep_for(1);
    *res = 1;

    // Select the device by seting chip select low
    *cs = 0;
    
    // Set to receive COMMANDS not data
    *dc = 0;

    spi->write(0xAE); // set display off
    spi->write(0xD5); // set display clock divide ratio
    spi->write(0xA0);
    spi->write(0xA8); // set multiplex ratio
    spi->write(0x3F); 
    spi->write(0xD3); // set display offset
    spi->write(0x00);
    spi->write(0x40); // set display start line
    spi->write(0xAD); // set master configuration
    spi->write(0x8E);
    spi->write(0xD8); // set area color mode
    spi->write(0x05);
    spi->write(0xA1); // set segment re-map
    spi->write(0xC8); // set com output scan direction
    spi->write(0xDA); // set com pins hardware configuration
    spi->write(0x12);
    spi->write(0x91); // set look-up table
    spi->write(0x3F);
    spi->write(0x3F);
    spi->write(0x3F);
    spi->write(0x3F);
    spi->write(0x81); // set current control for bank 0
    spi->write(0x8F);
    spi->write(0xD9); // set pre-charge period
    spi->write(0xD2);
    spi->write(0xDB); //set vcomh deselect level
    spi->write(0x34);
    spi->write(0xA4); // set entire display on/off
    spi->write(0xA6); // set normal/inverse display
    
    spi->write(0x20); // page mode
    spi->write(0x00);
    
    // TASK: Clear screen's content buffer
    
    // Is this neccessary when switching command/data?
    *cs = 1;
    thread_sleep_for(1);
    *cs = 0;
    
    // Set to receive DATA not commands
    *dc = 1;
        
    for (int i = 0; i < bufferCount; i++)
    {
        spi->write(0x00);
    }
    
    // TASK: SCREEN ON
    
    // Is this neccessary when switching command/data?
    *cs = 1;
    thread_sleep_for(1);
    *cs = 0;
    
    // Set to receive COMMANDS not data
    *dc = 0;
    
    spi->write(0xAF); // set display on
  
    // Deselect the device
    *cs = 1;
}

/*
char* rotateCharacter(char fontCharacter){
    uint8_t length = static_cast<int>fontCharacter[0];
    char rotatedCharacter[length];

    debugSerial->printf("length %d",length);

    
        
    return rotatedCharacter;
}*/
    
void SPKDisplay::setPixel(uint8_t* buffer, uint8_t x, uint8_t y, uint8_t set){
    uint8_t mask;
    
    if((x<128) && (y<64)){
        uint16_t index = (y/8*bufferWidth)+x;
        
        switch(y%8){
            case 0: mask = 1; break;
            case 1: mask = 2; break;
            case 2: mask = 4; break;
            case 3: mask = 8; break;
            case 4: mask = 0x10; break;
            case 5: mask = 0x20; break;
            case 6: mask = 0x40; break;
            case 7: mask = 0x80; break;
        }
        
        if(set==0) mask=~mask;
        
        if(set==1){
            buffer[index] |= mask;
        }
        else{
            buffer[index] &= mask;
        }
        
    }
}

uint8_t SPKDisplay::getYCoord(uint8_t x){
    return pixelHeight-1-x;
}

uint8_t SPKDisplay::getXCoord(uint8_t y){
    return y;
}

uint8_t SPKDisplay::readPixel(char character, uint8_t x){
    uint8_t mask;
    
    switch(x%8){
        case 0: mask = 1; break;
        case 1: mask = 2; break;
        case 2: mask = 4; break;
        case 3: mask = 8; break;
        case 4: mask = 0x10; break;
        case 5: mask = 0x20; break;
        case 6: mask = 0x40; break;
        case 7: mask = 0x80; break;
    }
    
    uint8_t pixelValue = character & mask;
    if(pixelValue>0){
        return 1;
    }
    else{
        return 0;
    }
}

void SPKDisplay::printDebug(){
    debug("Adress debug init\n\r");
    debug("cs adr: %p\n\r", cs);
    debug("dc adr: %p\n\r", dc);
    debug("res adr: %p\n\r", res);
}