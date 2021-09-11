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

#ifndef SPK_OLED_SSD1305_h
#define SPK_OLED_SSD1305_h

#include "mbed.h"
#include <string> 

//#define bufferCount 1056
//#define bufferWidth 132
#define bufferCount 1024
#define bufferWidth 128
#define pixelWidth 128
#define pixelHeight 64
#define pixInPage 8
#define pageCount 8

/** Display class for 128x64 OLEDs using the SSD1305 driver, connected via SPI
  *
  * Display ie. DENSITRON - DD-12864YO-3A
  *
  * This is a ground-up, minimal library. Further functionality as and when its needed or anybody wants to contribute.
  * 
  * This library includes two processing sketches to create a font and full-screen image in the required byte representations.
  * Without creating your font and any images, all this library will do is blank the screen and draw horizontal lines. But at least you'll know its working!
  * 
  * Terminology:
  * 'rows' are 8 pixel high rows across the display, 0 being the topmost and 7 the bottom.
  * 'lines' are single pixel lines, origin top left. 
  *
  * Example:
  * @code
  * // Create object and load font
  * SPKDisplay screen(p5, p7, p8, p10, p9)
  * screen.fontStartCharacter = &myStartChar;
  * screen.fontEndCharacter = &myEndChar;
  * screen.fontCharacters = myFontArray;
  * // Draw
  * screen.imageToBuffer(myImageByteArray);
  * screen.textToBuffer("*spark OLED SSD1305",0);
  * screen.textToBuffer("v01",1);
  * screen.sendBuffer
  * @endcode
  */
class SPKDisplay
{
  public:
    /** Create a display object connected via SPI
     *
     * @param mosi  SPI MOSI
     * @param clk   SPI SCK
     * @param cs    Chip Select - a digital out pin
     * @param dc    Data/Command - a digital out pin
     * @param res   Reset - a digital out pin
     * @param debugSerial An optional serial object to log to
     */
    SPKDisplay(PinName mosi, PinName clk, PinName cs, PinName dc, PinName res);

    /** Font - Assign the ASCII value of the character at the start of the implemented range */
    const int *fontStartCharacter;
    
    /** Font - Assign the ASCII value of the character at the end of the implemented range */
    const int *fontEndCharacter;
    
    /** Font - Assign the font, an array of 8x8px characters
     *
     * @note The processing sketch spk_oled_fontByteMaker--processing takes characterCount*8px x 8px images and creates the code to declare the font array needed by this method
     */
    uint8_t const **fontCharacters;
    
    /** Completely clear the object's display representation */
    void clearBuffer();
    
    /** Clear a row of the object's display representation
     *
     * @param row The row to clear.
     */
    void clearBufferRow(int row);
    
    /** Replace the object\s display representation with the contents of image
     *
     * @param image An array of 1056 bytes representing an image.
     * @note The processing sketch spk_oled_screenByteMaker--processing takes 132x64 images and creates the code to declare such arrays
     */ 
    void imageToBuffer(const uint8_t* image);
    
    void vertLineToBuffer(int x);

    /** Draw a horizontal line in the object's display representation 
     * 
     * @param y The y position of the line to draw
     */
    void horizLineToBuffer(int y);
    
    /** Write a line of text in the object's display representation
     * Requires the font to have been set
     * 
     * @param message   The text to write. The text will be truncated if longer than the screen's width.
     * @param row       The row in which to write the text
     */ 
    void textToBuffer(std::string message, int row, uint8_t offsetY, uint8_t offsetX);
    
    /** Write a single character in the object's display representation
     * Requires the font to have been set
     * 
     * @param character The character to write.
     * @param x         The x position to draw the character
     * @param row       The row in which to write the character
     */ 
    void characterToBuffer(char character, int x, int row, uint8_t offsetY, uint8_t offsetX);
    
    void characterArrayToBuffer(char* character, int row, uint8_t offsetY, uint8_t offsetX, bool center);

    /** Send the object's display representation to the OLED
     * 
     * You can safely call this once per main loop, it will only transmit the buffer contents if there has been an update
     */
    void sendBuffer();

    void printDebug();
    
  private:
    DigitalOut _cs;
    DigitalOut _dc;
    DigitalOut _res;
    SPI _spi;

    DigitalOut* cs;
    DigitalOut* dc;
    DigitalOut* res;
    SPI *spi;
    
    uint8_t buffer[bufferCount];
    
    bool bufferHasChanged;
    BufferedSerial *debugSerial;
    void setup();
    void setPixel(uint8_t* buffer, uint8_t x, uint8_t y, uint8_t set);
    uint8_t getYCoord(uint8_t x);
    uint8_t getXCoord(uint8_t y);
    uint8_t readPixel(char character, uint8_t x); 
};

#endif
