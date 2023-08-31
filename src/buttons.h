#pragma once

#include "myLeds.h"
#include "defines.h"

// Buttons are WHITE while they are pressed.
// They return to their assigned color (which defaults to black) when
// an the first event in their lifecylce is returned.

// PRESS is sent out as soon as the button is pressed.
// CLICK is sent out on the button being released.
// LONG_CLICK is sent out after the button is pressed for a certain amount of time.
// Buttons with PRESS cannot be CLICK or LONG_CLICK.
// RELEASE and REPEAT can can only be used with PRESS.


#define BUTTON_EVENT_CLICK          0x0001          // returned upon release
#define BUTTON_EVENT_LONG_CLICK     0x0010          // returned after a period of time
#define BUTTON_EVENT_PRESS          0x0100          // returned right away
#define BUTTON_EVENT_RELEASE        0x0200          // returned upon PRESS being released
#define BUTTON_MASK_REPEAT          0x0400          // possibly returned after PRESS multiple times



class buttonArray;
    // forward

class arrayedButton
{
    public:

        arrayedButton();

        void init();                    // clears mask, color, and all state

        //uint16_t getMask()              { return m_mask; }
        //void setMask(uint16_t mask)     { m_mask = mask; }
        //uint32_t getColor()             { return m_color; }
        //void setColor(uint32_t color)   { m_color = color; }
        //
        //void setBlink(bool blink)       { m_blink = blink; }

    private:

        friend class buttonArray;

        bool     m_blink;

        uint16_t m_mask;
        uint32_t m_color;
        uint16_t m_state;

        uint32_t m_press_time;
        uint32_t m_debounce_time;
        uint32_t m_repeat_time;

};




class buttonArray
{
    public:

        buttonArray();

        void begin();       // called once
        void clear();       // called on new windows
        void task();        // called by the_system timer

        // arrayedButton *getButton(int num)            { return &m_buttons[num]; }

        void setButtonType(int num, uint16_t mask, uint32_t color = LED_BLACK);

        void setButtonMask(int num, uint16_t mask)      { m_buttons[num].m_mask = mask; }
        void setButtonColor(int num, uint32_t color)    { m_buttons[num].m_color = color; }
        void setButtonBlink(int num, bool blink)        { m_buttons[num].m_blink = blink; }
        uint16_t getButtonMask(int num)                 { return m_buttons[num].m_mask; }
        uint32_t getButtonColor(int num)                { return m_buttons[num].m_color; }
        bool getButtonBlink(int num)                    { return m_buttons[num].m_blink; }

    private:

        arrayedButton m_buttons[NUM_BUTTONS];
        void showButton(int num);

};



extern buttonArray the_buttons;
