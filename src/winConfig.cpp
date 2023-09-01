//--------------------------------------
// winConfig.cpp
//--------------------------------------
#include <myDebug.h>
#include "winConfig.h"
#include "prefs.h"
#include "myTFT.h"
#include "myLeds.h"
#include "buttons.h"
#include "configOptions.h"
#include "winDialogs.h"

#define dbg_cfg  0
	// 0 == show important operations (begin, reboot, onButton)
	// -1 == show drawing details


#define BUTTON_BRIGHTNESS_DOWN  0
#define BUTTON_BRIGHTNESS_UP    1
#define BUTTON_EXIT_CANCEL      3
#define BUTTON_EXIT_DONE        4

#define BUTTON_MOVE_UP          12
#define BUTTON_MOVE_LEFT        16
#define BUTTON_MOVE_RIGHT       18
#define BUTTON_MOVE_DOWN        22
#define BUTTON_SELECT           17


configOption *rootOption = 0;
configOption *cur_menu = 0;
configOption *cur_option = 0;
configOption *last_option = 0;
configOption *display_menu = 0;
configOption *display_option = 0;
configOption *optBrightness = 0;

int num_menu_children = 0;

winConfig win_config;

// terminal node modal dialog functions
// will be called with the menu number
// ENABLED_CONFIG(ftpPortOption,!getPref8(PREF_SPOOF_FTP));

static void setDbgDeviceFromPref()
{
	dbgSerial =
		(prefs.DEBUG_DEVICE == DEBUG_DEVICE_SERIAL) ? (Stream *) &SERIAL_DEVICE :
		(prefs.DEBUG_DEVICE == DEBUG_DEVICE_USB)	? (Stream *) &Serial : NULL;
}



//---------------------------------
// ctor and functions
//---------------------------------


// virtual
void winConfig::begin(bool cold)
{
    display(dbg_cfg,"winConfig::begin(%d)",cold);
    sysWindow::begin(cold);

	rootOption = the_options.getOption(0);
	optBrightness = the_options.getOption(BRIGHTNESS_OPT_NUM);

	if (cold)
	{
		the_options.init();
		cur_menu = rootOption;
		num_menu_children = cur_menu->getNumChildren();
		cur_option = rootOption->getFirstChild();
		cur_option->select(1);
	}

	display_menu = 0;
	display_option = 0;
	last_option = 0;

    // setup buttons and leds

    the_buttons.setButtonType(BUTTON_BRIGHTNESS_DOWN,	BUTTON_EVENT_PRESS | BUTTON_MASK_REPEAT,		LED_RED);
    the_buttons.setButtonType(BUTTON_BRIGHTNESS_UP,		BUTTON_EVENT_PRESS | BUTTON_MASK_REPEAT,		LED_GREEN);
    the_buttons.setButtonType(BUTTON_EXIT_DONE,			BUTTON_EVENT_CLICK | BUTTON_EVENT_LONG_CLICK,	LED_PURPLE);
    the_buttons.setButtonType(BUTTON_EXIT_CANCEL,		BUTTON_EVENT_CLICK | BUTTON_EVENT_LONG_CLICK,	LED_ORANGE);

	the_buttons.setButtonType(BUTTON_MOVE_UP,			BUTTON_EVENT_PRESS | BUTTON_MASK_REPEAT,		LED_BLUE);
	the_buttons.setButtonType(BUTTON_MOVE_DOWN,			BUTTON_EVENT_PRESS | BUTTON_MASK_REPEAT,		LED_BLUE);
	the_buttons.setButtonType(BUTTON_MOVE_LEFT,			BUTTON_EVENT_PRESS,								LED_BLUE);
	the_buttons.setButtonType(BUTTON_MOVE_RIGHT,		BUTTON_EVENT_PRESS,								LED_BLUE);
	the_buttons.setButtonType(BUTTON_SELECT,			BUTTON_EVENT_CLICK, 							LED_GREEN);

	display(dbg_cfg,"winConfig::begin() finished",0);

}   // winConfig::begin



void reboot(int num)
	// general purpose static reboot method
{
    display(dbg_cfg,"winConfig::reboot(%d)",num);
    if (dbgSerial == &Serial)
        Serial.end();
    else
        SERIAL_DEVICE.end();

    for (int i=0; i<21; i++)
    {
        setLED(num,i & 1 ? LED_RED : 0);
        showLEDs();
        delay(80);
    }
    #define RESTART_ADDR 0xE000ED0C
    #define READ_RESTART() (*(volatile uint32_t *)RESTART_ADDR)
    #define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))
    WRITE_RESTART(0x5FA0004);
}


// virtual
void winConfig::onChildEnd(uint16_t param)
	// called when modal dialogs are ended
{
    display(dbg_cfg,"winConfig::onChildEnd(%d)",param);
	if (param == OPTION_FACTORY_RESET)
	{
		reset_prefs();
		reboot(THE_SYSTEM_BUTTON);
	}
	else if (param & OPTION_SPOOF_FTP)
	{
		uint8_t spoof_ftp_value = param & 0xff;
		prefs.SPOOF_FTP = spoof_ftp_value;
		save_prefs();
		reboot(THE_SYSTEM_BUTTON);
	}
}




//---------------------------------------------
// BUTTONS
//---------------------------------------------

// virtual
void winConfig::onButton(int row, int col, int event)
{
    int num = row * NUM_BUTTON_COLS + col;
    display(dbg_cfg,"winConfig::onButton(%d) cur(%d:%s) enabled(%d)",
		num,
		cur_option->getNum(),
		cur_option->getTitle(),
		cur_option->getEnabled());

    if (num == BUTTON_MOVE_UP)
    {
        if (cur_option->getPrev())
        {
            cur_option->select(0);
            cur_option = cur_option->getPrev();
            cur_option->select(1);
        }
    }
    else if (num == BUTTON_MOVE_DOWN)
    {
        if (cur_option->getNext())
        {
            cur_option->select(0);
            cur_option = cur_option->getNext();
            cur_option->select(1);
        }
    }
    else if (num == BUTTON_MOVE_LEFT)
    {
        if (cur_option->getParent() != rootOption)
        {
		    cur_option->select(0);
			cur_option = cur_option->getParent();
			cur_menu = cur_option->getParent();
			cur_option->select(1);
        }
    }

    // exit / cancel

    else if (num == BUTTON_EXIT_CANCEL)
    {
        if (event == BUTTON_EVENT_LONG_CLICK)
        {
            reboot(num);
        }
        else
        {
			read_prefs();
			setDbgDeviceFromPref();
            setLEDBrightness(prefs.BRIGHTNESS);
			endWindow(0);
        }
    }
    else if (num == BUTTON_EXIT_DONE)
    {
        if (event == BUTTON_EVENT_LONG_CLICK)
			save_prefs();
		endWindow(0);
    }

	// do something

    else if (cur_option->getEnabled())
	{
		if (num == BUTTON_SELECT)
		{
			// normal behavior - go into child menu,
			// increment the value, call a dialog window,
			// or do some special function

			if (cur_option->getType() & OPTION_FACTORY_RESET)
			{
				yes_no_dlg.setId(OPTION_FACTORY_RESET);
				yes_no_dlg.setName("Confirm Factory Reset");
				yes_no_dlg.setMessage("Are you sure you want to do a\nfactory reset?");
				the_system.startWindow(&yes_no_dlg);
			}
			else if (cur_option->getType() & OPTION_SPOOF_FTP)
			{
				char display_buf[255];
				bool new_value = !((bool)cur_option->getValue());
				sprintf(display_buf,"Turning FTP Spoofing %s requires\nthat we save the prefs and reboot.\nAre you sure you want to do that?",
					new_value ? "ON" : "OFF");
				// or the option value into the id==return value
				yes_no_dlg.setId(OPTION_SPOOF_FTP | new_value);
				yes_no_dlg.setName("Spoof FTP Change!");
				yes_no_dlg.setMessage(display_buf);
				the_system.startWindow(&yes_no_dlg);
			}
			else if (cur_option->getFirstChild())
			{
				display_option = 0;
				cur_option->select(0);
				cur_menu = cur_option;
				cur_option = cur_menu->getFirstChild();
				cur_option->select(1);
			}
			else if (cur_option->hasValue())
			{
				display(0,"incValue()",0);
				cur_option->incValue();
				if (!strcmp(cur_option->getTitle(),DEBUG_DEVICE_NAME))
					setDbgDeviceFromPref();
			}
			else if (cur_option->getSetterFxn())
			{
				display(dbg_cfg,"calling setter fxn for %s",cur_option->getTitle());
				(cur_option->getSetterFxn())(cur_option->getNum());
			}
		}

		else if (num == BUTTON_BRIGHTNESS_UP ||
				 num == BUTTON_BRIGHTNESS_DOWN)
		{
			int inc = num == BUTTON_BRIGHTNESS_UP ? 5 : -5;
			int bright = prefs.BRIGHTNESS + inc;

			if (bright < cur_option->getMin())
				bright = cur_option->getMin();
			if (bright > cur_option->getMax())
				bright = cur_option->getMax();
			optBrightness->setValue(bright);
		}
	}	// enabled
}


//---------------------------------------------
// DRAW
//---------------------------------------------

#define LINE_HEIGHT     45
#define TOP_OFFSET      50
#define TEXT_OFFSET     10
#define HIGHLIGHT_OFFSET 3

#define LEFT_OFFSET     20
#define RIGHT_OFFSET    20

#define NUMBER_WIDTH    120
#define MID_OFFSET      (TFT_WIDTH/2)


#define OPTIONS_PER_PAGE   6


void winConfig::updateUI()
{
    bool draw_all = false;

	// redraw title and clear screen if we changed levels

    if (display_menu != cur_menu)
    {
		display(dbg_cfg+1,"winConfig::updateUI() cur_menu(%d:%s) != display_menu(%d:%s)",
			cur_menu->getNum(),
			cur_menu->getTitle(),
			display_menu ? display_menu->getNum() : 238,
			display_menu ? display_menu->getTitle() : "null");

        display_menu = cur_menu;
        draw_all = true;

        fillRect(full_client_rect,TFT_BLACK);

        if (cur_menu == rootOption)
            the_system.setTitle(name());
        else
            the_system.setTitle(cur_menu->getTitle());
    }

	// calculate the item that should show at top of screen

	if (cur_option != last_option)
	{
		display(dbg_cfg+1,"winConfig::updateUI() cur_option(%d:%s) != last_option(%d:%s) m_scroll_top=%d",
			cur_option->getNum(),
			cur_option->getTitle(),
			last_option ? last_option->getNum() : 237,
			last_option ? last_option->getTitle() : "null",
			m_scroll_top);

		last_option = cur_option;
		int num = cur_option->getNum();

		// on a level change, start by assuming that we will
		// just show the first n items

		if (draw_all)
		{
			m_scroll_top = cur_menu->getFirstChild()->getNum();
			display(dbg_cfg+1,"init m_scroll_top=%d",m_scroll_top);
		}

		// then if the thing wont's show, set the scrolling
		// position so that it is the end or the first one
		// depending on the direction

		int scroll_top = m_scroll_top;
		if (num < scroll_top)
			scroll_top = num;
		else if (num >= scroll_top + OPTIONS_PER_PAGE)
			scroll_top = num - OPTIONS_PER_PAGE + 1;

		// if the scroll posititon changed
		// 		and we didn't already clear the screen,
		// 			clear it now ..
		//      and set draw_all to redraw all the items

		if (scroll_top != m_scroll_top)
		{
			if (!draw_all)
				fillRect(full_client_rect,TFT_BLACK);

			m_scroll_top = scroll_top;
			draw_all = true;
			display(dbg_cfg+1,"new m_scroll_top=%d",m_scroll_top);
		}
	}


	// items

	int y = TOP_OFFSET;
    int count = cur_menu->getNumChildren();
    configOption *opt = cur_menu->getFirstChild();

	mylcd.setFont(Arial_20);

    while (opt && count--)
    {
		int num = opt->getNum();

		if (num >= m_scroll_top && num < m_scroll_top + OPTIONS_PER_PAGE)
		{
			bool enabled = opt->getEnabled();
			bool selected = opt->getSelected();
			bool draw_value = opt->needsValueDisplay();
			bool draw_selected = opt->selectedChanged();
			bool draw_enabled = opt->enabledChanged();

			opt->clearDisplayValue();

			if (draw_all || draw_selected || draw_enabled)
			{
				display(dbg_cfg+1,"draw(%d) opt(%d:%s) enabled(%d) selected(%d)  draw_value(%d) draw_sel(%d) draw_en(%d)",
					y,
					opt->getNum(),
					opt->getTitle(),
					enabled,
					selected,
					draw_value,
					draw_selected,
					draw_enabled);

				int color = TFT_BLACK;
				if (selected)
					color = TFT_BLUE;

				// don't need to draw black on a full redraw

				if (color != TFT_BLACK || !draw_all)
					mylcd.fillRect(0,y,TFT_WIDTH,LINE_HEIGHT-HIGHLIGHT_OFFSET,color);

				uint16_t fc = enabled ? TFT_YELLOW : TFT_DARKGREY;

				mylcd.setTextColor(fc);
				mylcd.setCursor(LEFT_OFFSET,y + TEXT_OFFSET);
				mylcd.print(opt->getTitle());
			}

			if (draw_all || draw_value || draw_selected || draw_enabled)
			{
				display(dbg_cfg+1,"draw_value(%d) opt(%d:%s) value(%s_",
					y,
					opt->getNum(),
					opt->getTitle(),
					opt->getValueString());

				uint16_t fc = enabled ? TFT_WHITE : TFT_DARKGREY;

				mylcd.printfJustified(
					MID_OFFSET,
					y + TEXT_OFFSET,
					MID_OFFSET - RIGHT_OFFSET,
					LINE_HEIGHT - TEXT_OFFSET - HIGHLIGHT_OFFSET,
					LCD_JUST_RIGHT,
					fc,
					opt->getSelected() ? TFT_BLUE : TFT_BLACK,
					"%s",
					opt->getValueString());
			}
	        y += LINE_HEIGHT;
		}

        opt = opt->getNext();
    }

}	// updateUI()
