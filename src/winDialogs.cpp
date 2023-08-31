//-------------------------------------------
// winDialogs.cpp
//-------------------------------------------

#include <myDebug.h>
#include "winDialogs.h"
#include "buttons.h"
#include "myTFT.h"

#define BUTTON_NO    16
#define BUTTON_OK    17
#define BUTTON_YES   18

rigErrorDialog rig_error_dlg;
yesNoDialog    yes_no_dlg;


// rigErrorDialog

void rigErrorDialog::begin(bool warm)
{
    sysWindow::begin(warm);
    the_buttons.setButtonType(BUTTON_OK, BUTTON_EVENT_CLICK, LED_GREEN);
    mylcd.setFont(Arial_16_Bold);
	mylcd.printJustified(
		client_rect.xs + 30,
		client_rect.ys + 30,
		client_rect.xe - 60,
		client_rect.ye - 60,
        LCD_JUST_LEFT,
		TFT_RED,
		TFT_BLACK,
		1,	// use bc
		m_msg);
}

void rigErrorDialog::onButton(int row, int col, int event)
{
    endWindow(0);
}


// yesNoDialog

void yesNoDialog::begin(bool warm)
{
    sysWindow::begin(warm);
    the_buttons.setButtonType(BUTTON_NO, BUTTON_EVENT_CLICK, LED_RED);
    the_buttons.setButtonType(BUTTON_YES,BUTTON_EVENT_CLICK, LED_GREEN);
    mylcd.setFont(Arial_16_Bold);
	mylcd.printJustified(
		client_rect.xs + 10,
		client_rect.ys + 10,
		client_rect.xe - 20,
		client_rect.ye - 20,
        LCD_JUST_LEFT,
		TFT_YELLOW,
		TFT_BLACK,
		1,	// use bc
		m_msg);
}

void yesNoDialog::onButton(int row, int col, int event)
{
    int num = row * NUM_BUTTON_COLS + col;
    endWindow(num == BUTTON_YES ? 1 : 0);
}

