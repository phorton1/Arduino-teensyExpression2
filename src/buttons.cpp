#include <myDebug.h>
#include "buttons.h"
#include "theSystem.h"
#include "myLeds.h"

#define dbg_btn   1
	// 0 = show initialization
	// -1 = show actions

#define BUTTON_STATE_PRESSED       0x0001
    // is the button currently pressed
#define BUTTON_STATE_WHITE	   	   0x0010
	// the button is WHITE
#define BUTTON_STATE_HANDLED       0x0100
	// LONG_CLICK event was sent

#define DEBOUNCE_MILLIS		30
#define LONG_PRESS_TIME		800
#define DOUBLE_CLICK_TIME	360
#define BLINK_INTERVAL		300


buttonArray theButtons;
int row_pins[NUM_BUTTON_COLS] = {PIN_BUTTON_OUT0,PIN_BUTTON_OUT1,PIN_BUTTON_OUT2,PIN_BUTTON_OUT3,PIN_BUTTON_OUT4};
int col_pins[NUM_BUTTON_ROWS] = {PIN_BUTTON_IN0,PIN_BUTTON_IN1,PIN_BUTTON_IN2,PIN_BUTTON_IN3,PIN_BUTTON_IN4};


//--------------------------------------
// arrayedButton
//--------------------------------------


arrayedButton::arrayedButton()
{
	m_state = 0;
    init();
}


void arrayedButton::init()
{
    m_mask = 0;
	m_color = LED_BLACK;
    m_blink = 0;
	m_press_time = 0;
    m_debounce_time = 0;
    m_repeat_time = 0;
}




//--------------------------------------
// buttonArray
//--------------------------------------


buttonArray::buttonArray()
{}


void buttonArray::begin()
{
    display(dbg_btn,"buttonArray::begin()",0);
    for (int row=0; row<NUM_BUTTON_ROWS; row++)
    {
        pinMode(row_pins[row],OUTPUT);
        digitalWrite(row_pins[row],0);
    }
    for (int col=0; col<NUM_BUTTON_COLS; col++)
        pinMode(col_pins[col],INPUT_PULLDOWN);            // guessing that pins 7 and 8 doent have pulldowns
}


void buttonArray::clear()
{
    for (int num=0; num<NUM_BUTTONS; num++)
		m_buttons[num].init();
}


void buttonArray::setButtonType(int num, uint16_t mask, uint32_t color)
{
	display(dbg_btn,"setButtonType(%d, 0x%04X, 0x%06x)",num,mask,color);
    arrayedButton *pb = &m_buttons[num];
    pb->m_mask = mask;
    pb->m_color = color;
}



void buttonArray::showButton(int num)
	// forces a button to show the correct color before an event
{
    uint32_t use_color = (m_buttons[num].m_state & BUTTON_STATE_WHITE) ? LED_WHITE : m_buttons[num].m_color;
	display(dbg_btn+1,"showButton(%d, color=0x%06x, use_color=0x%06x)",num,m_buttons[num].m_color,use_color);
	setLED(num,use_color);
	showLEDs(1);	// force
}


//--------------------------------------
// task
//--------------------------------------

void buttonArray::task()
{
	// invariantly toggle the blink_state every BLINK_INTERVAL ms

	static uint32_t blink_time = 0;
	static bool blink_state = 0;
	uint32_t now = millis();
	if (now - blink_time >= BLINK_INTERVAL)
	{
		blink_time = now;
		blink_state = !blink_state;
	}

	// poll the buttons

    for (int row=0; row<NUM_BUTTON_ROWS; row++)
    {
        digitalWrite(row_pins[row],1);

        for (int col=0; col<NUM_BUTTON_COLS; col++)
        {
            // only process registered buttons

            int num = row * NUM_BUTTON_COLS + col;
            arrayedButton *pb = &m_buttons[num];
            if (!pb->m_mask) continue;

			// skip further processing if during a debounce window

			uint32_t now = millis();
			if (pb->m_debounce_time && now - pb->m_debounce_time < DEBOUNCE_MILLIS)
				continue;
			pb->m_debounce_time = 0;

            // if state changed, process the button


            bool is_pressed = digitalRead(col_pins[col]);
            bool was_pressed = pb->m_state & BUTTON_STATE_PRESSED;

            if (is_pressed != was_pressed)
            {
				pb->m_debounce_time = now;
				display(dbg_btn+1,"BUTTON(%d,%d) %-6s  mask=%04x  state=%04x",
                    row,col,is_pressed?"DOWN":"UP",pb->m_mask,pb->m_state);

                //---------------------------
                // pressed
                //---------------------------

                if (is_pressed)     // button pressed
                {
                    pb->m_press_time = now;
                    pb->m_state = BUTTON_STATE_PRESSED;
                    if (pb->m_mask & BUTTON_EVENT_PRESS)		// PRESS buttons don't turn white
                    {
                        display(dbg_btn+1,"BUTTON_EVENT_PRESS(%d,%d)",row,col);
                        the_system.onButton(row, col, BUTTON_EVENT_PRESS);
                    }
					else
					{
						pb->m_state |= BUTTON_STATE_WHITE;
					}
                }

                //---------------------------
                // released
                //---------------------------

                else	// was_pressed && !is_pressed
                {
					if (!(pb->m_state & BUTTON_STATE_HANDLED))
					{
						pb->m_state = 0;
						showButton(num);
						if (pb->m_mask & BUTTON_EVENT_RELEASE)
						{
							display(dbg_btn+1,"BUTTON_EVENT_RELEASE(%d,%d)",row,col);
							the_system.onButton(row, col, BUTTON_EVENT_RELEASE);
						}
						if (pb->m_mask & BUTTON_EVENT_CLICK)
						{
							display(dbg_btn+1,"BUTTON_EVENT_CLICK(%d,%d)",row,col);
							the_system.onButton(row, col, BUTTON_EVENT_CLICK);
						}
                    }

					pb->m_state = 0;
					pb->m_press_time = 0;
					pb->m_repeat_time = 0;
                }
            }

            //--------------------------------
            // state did not change
            //--------------------------------

            else if (is_pressed &&
					 pb->m_state & BUTTON_STATE_PRESSED &&
					 !(pb->m_state & BUTTON_STATE_HANDLED))
            {
                // repeat generates PRESS events

                uint32_t dif = now - pb->m_press_time;
                if ((pb->m_mask & BUTTON_MASK_REPEAT) && dif > 300)
                {
                    // starts repeating after 300ms
                    // starts at 10 per second and accelerates to 100 per second over one second

                    dif -= 300;
                    if (dif > 1000) dif = 1000;

                    uint32_t interval = 100;
                    interval -= 90*dif/1000;

                    if (now - pb->m_repeat_time > interval)
                    {
                        display(dbg_btn+1,"repeat BUTTON_EVENT_PRESS(%d,%d)",row,col);
                        the_system.onButton(row, col, BUTTON_EVENT_PRESS);
                        pb->m_repeat_time = now;
                    }
                }

                // which is generally exclusive of long clicks

                else if ((pb->m_mask & BUTTON_EVENT_LONG_CLICK) && dif > LONG_PRESS_TIME)
                {
					pb->m_state |= BUTTON_STATE_HANDLED;
					pb->m_state &= ~BUTTON_STATE_WHITE;
					showButton(num);
                    display(dbg_btn+1,"BUTTON_EVENT_LONG_CLICK(%d,%d)",row,col);
                    the_system.onButton(row, col, BUTTON_EVENT_LONG_CLICK);
                }

            }   // pressed and not handled yet

			// check if the button LED needs to be written

			uint32_t use_color =
				pb->m_state & BUTTON_STATE_WHITE ? LED_WHITE :
				pb->m_blink && !blink_state ? LED_BLACK :
				pb->m_color;
			if (getLED(num) != use_color)
				setLED(num,use_color);

        }   // for each col

        digitalWrite(row_pins[row],0);

    }   // for each row

	// let the leds show any changes with force = 0

	showLEDs(0);

}
