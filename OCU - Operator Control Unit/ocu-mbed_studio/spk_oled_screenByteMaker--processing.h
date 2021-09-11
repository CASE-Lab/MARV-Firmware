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

int w = 132;
int h = 64;
int scaleFactor = 10;

PImage imgPixel;
color white = color(255, 255, 255, 255);
color black = color(0, 0, 0, 255);

void setup()
{
  size(w*scaleFactor, h*scaleFactor);
  imgPixel = loadImage("spk_dvimxr_screen.png");

  noSmooth();
}

void draw()
{
  background(0);
  image(imgPixel, 0, 0, w*scaleFactor, h*scaleFactor);
}

void mouseDragged() 
{
  int x = mouseX/scaleFactor;
  int y = mouseY/scaleFactor;

  imgPixel.set(x, y, white);
}

void mousePressed()
{
  int x = mouseX/scaleFactor;
  int y = mouseY/scaleFactor;

  if (imgPixel.get(x, y) == white)
  {
    imgPixel.set(x, y, black);
  }
  else
  {
    imgPixel.set(x, y, white);
  }
}

void keyPressed()
{
  println("{");
  for (int page = 0; page < 8; page++)
  {
    for (int i = 0; i < w; i++)
    {
      byte theByte = 0;
      for (int j = 0; j < 8; j++)
      {
       if (imgPixel.get(i, (page*8)+j) == white)
       {
          theByte += pow(2, j); 
       }
      }
      print("0x" + hex(theByte, 2));
      print(", ");
    }
    println();
  }
  println("}");
}
