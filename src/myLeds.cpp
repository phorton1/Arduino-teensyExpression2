
#include "myLeds.h"
#include "defines.h"
#include <WS2812Serial.h>


//---------------------------------
// LEDS
//---------------------------------
// Usable pins:
//
//   Teensy LC:   1, 4, 5, 24
//   Teensy 3.2:  1, 5, 8, 10, 31   (overclock to 120 MHz for pin 8)
//   Teensy 3.5:  1, 5, 8, 10, 26, 32, 33, 48
//   Teensy 3.6:  1, 5, 8, 10, 26, 32, 33

#define LED_PIN          5
    // Note that choosing to use pin 5 eats up Serial1
    // hence why we have to use SERIAL_DEVICE in teensyExpression.ino
    // Pauls Notes:
    //
    //      Non-blocking performance does come with a cost.  15 bytes of memory are required
    //      per LED, rather than the usual 3 bytes with [FastLED](http://fastled.io/) or
    //      [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel).  One of
    //      the [hardware serial ports](https://www.pjrc.com/teensy/td_uart.html) is also
    //      used to transmit the data, making that port unavailable for other uses.
    //
    //      ## Supported Pins & Serial Ports
    //
    //      | Port    | Teensy LC   | Teensy 3.2 | Teensy 3.5 | Teensy 3.6 |
    //      | :------ | :---------: | :--------: | :--------: | :--------: |
    //      | Serial1 | 1, 4, 5, 24 | 1, 5       | 1, 5, 26   | 1, 5, 26   |
    //      | Serial2 |             | 10, 31     | 10         | 10         |
    //      | SERIAL_DEVICE |             | 8          | 8          | 8          |
    //      | Serial4 |             |            | 32         | 32         |
    //      | Serial5 |             |            | 33         | 33         |
    //      | Serial6 |             |            | 48         |            |
    //
    //      Serial2 & SERIAL_DEVICE on Teensy LC are not supported, due to lack of configurable
    //      oversampling needed to run at the high speed required.
    //
    //      SERIAL_DEVICE-Serial6 should be used only with CPU speeds 120 or 180 MHz.
    //
    //      Serial6 on Teensy 3.6 is not currently supported, due to different hardware
    //      registers.

#define NUM_LEDS            (NUM_BUTTON_ROWS * NUM_BUTTON_COLS)
#define RC_TO_LED_NUM(r,c)  ( (r & 1) ? ((r+1) * NUM_BUTTON_COLS - c - 1) : (r * NUM_BUTTON_COLS + c))
    //  0, 1, 2, 3, 4
    //  9, 8, 7, 6, 5
    // 10,11,12,13,14
    // 19,18,17,16,15
    // 20,21,22,23,24


uint32_t drawingMemory[NUM_LEDS];         //  4 bytes per LED, MINE - independent of brightness
byte renderingMemory[NUM_LEDS*3];         //  3 bytes per LED
DMAMEM byte displayMemory[NUM_LEDS*12];   // 12 bytes per LED

WS2812Serial leds(NUM_LEDS, displayMemory, renderingMemory, LED_PIN, WS2812_GRB);

uint32_t brightness = 100;
bool leds_changed = 0;

void initLEDs()
{
    leds.begin();
}

void clearLEDs()
{
    for (int i=0; i<NUM_BUTTONS; i++)
        setLED(i,LED_BLACK);
}


int getLEDBrightness()
{
    return brightness;
}

void setLEDBrightness(int i)   // 0..100
{
    brightness = i;
    showLEDs(true);
}


uint32_t getLED(int num)
{
    int row = num / NUM_BUTTON_COLS;
    int col = num % NUM_BUTTON_COLS;
    return getLED(row,col);
}

uint32_t getLED(int row, int col)
{
    return drawingMemory[RC_TO_LED_NUM(row,col)];
}


void setLED(int num, uint32_t color)
{
    int row = num / NUM_BUTTON_COLS;
    int col = num % NUM_BUTTON_COLS;
    setLED(row,col,color);
}

void setLED(int row, int col, uint32_t color)
{
    drawingMemory[RC_TO_LED_NUM(row,col)] = color;
    leds_changed = 1;
}



void showLEDs(bool force)
{
    if (force || leds_changed)
    {
        for (int i=0; i<NUM_LEDS; i++)
        {
            uint32_t c = drawingMemory[i];
            uint32_t r = (c >> 16) * brightness;
            uint32_t g = ((c >> 8) & 0xff) * brightness;
            uint32_t b = (c & 0xff) * brightness;
            r /= 100;
            g /= 100;
            b /= 100;
            leds.setPixel(i,(r<<16)+(g<<8)+b);
        }
        leds.show();
    }
    leds_changed = false;
}


void LEDFancyStart()
{
    for (int row=0; row<NUM_BUTTON_ROWS; row++)
    {
        for (int col=0; col<NUM_BUTTON_COLS; col++)
        {
            float c = col;
            float r = row;

            float red = (c/4) * 255.0;
            float blue = ((4-c)/4) * 255.0;
            float green = (r/4) * 255.0;

            uint32_t rr = red;
            uint32_t gg = green;
            uint32_t bb = blue;

            setLED(row,col,(rr << 16) + (gg << 8) + bb);
            showLEDs();
            delay(40);
        }
    }
}
