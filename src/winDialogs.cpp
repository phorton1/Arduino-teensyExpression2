//-------------------------------------------
// winDialogs.cpp
//-------------------------------------------

#include <myDebug.h>
#include "winDialogs.h"
#include "buttons.h"
#include "myTFT.h"


okDialog	ok_dlg;
yesNoDialog	yes_no_dlg;



//----------------------
// okDialog
//----------------------

#define BUTTON_OK    17

void okDialog::begin(bool cold)
{
    sysWindow::begin(cold);
    the_buttons.setButtonType(BUTTON_OK, BUTTON_EVENT_CLICK, LED_GREEN);
    mylcd.setFont(Arial_16_Bold);
	mylcd.printJustified(
		client_rect.xs + 30,
		client_rect.ys + 30,
		client_rect.xe - 60,
		client_rect.ye - 60,
        LCD_JUST_CENTER,
		m_color,
		TFT_BLACK,
		1,	// use bc
		m_msg);
}

void okDialog::onButton(int row, int col, int event)
{
    endWindow(0);
}


//----------------------
// yesNoDialog
//----------------------

#define BUTTON_NO    16
#define BUTTON_YES   18

void yesNoDialog::begin(bool cold)
{
    sysWindow::begin(cold);
    the_buttons.setButtonType(BUTTON_NO, BUTTON_EVENT_CLICK, LED_RED);
    the_buttons.setButtonType(BUTTON_YES,BUTTON_EVENT_CLICK, LED_GREEN);
    mylcd.setFont(Arial_16_Bold);
	mylcd.printJustified(
		client_rect.xs + 30,
		client_rect.ys + 30,
		client_rect.xe - 60,
		client_rect.ye - 60,
        LCD_JUST_CENTER,
		TFT_YELLOW,
		TFT_BLACK,
		1,	// use bc
		m_msg);
}

void yesNoDialog::onButton(int row, int col, int event)
{
    int num = row * NUM_BUTTON_COLS + col;
    endWindow(num == BUTTON_YES ? m_id : 0);
}


#if 0	// removed from compilation 2025-03-08 while normalizing
		// fileSystem to TE3

	formatSDDialog format_sd_dialog;

	//----------------------
	// formatSDDialog
	//----------------------
	// If they choose to format, control is never returned
	// to the system.  The SD Card is formatted with explicit
	// knowledge to write to this window. This window then
	// sleeps for 5 seconds and invariantly reboots the machine.

	#include "fileSystem.h"

	#define dbg_format 0


	#define LINE_MARGIN     10
	#define DOT_MARGIN      10

	#define START_Y (client_rect.ys + LINE_OFFSET)

	extern void reboot(int num);
		// in winConfig.cpp
	extern bool eraseCard();
		// in fileFormat.cpp
		// erases but does not format card
	extern bool formatCard();
		// in fileFormat.cpp

	static int confirm_count;




	void sdFormatDot()
	{
		int16_t x,y;
		mylcd.getCursor(&x, &y);
		if (x > mylcd.width() - DOT_MARGIN)
		{
			mylcd.println();
			mylcd.getCursor(&x, &y);
			mylcd.setCursor(DOT_MARGIN, y);
		}
		mylcd.print('.');
	}


	void sdFormatMsg(const char *msg, bool err = false)
	{
		mylcd.setTextColor(err?TFT_RED:TFT_YELLOW);
		int16_t x,y;
		mylcd.getCursor(&x, &y);

		if (x != LINE_MARGIN)
		{
			mylcd.println();
			mylcd.getCursor(&x, &y);
			mylcd.setCursor(LINE_MARGIN, y);
		}

		mylcd.print(msg);
	}



	void formatSDDialog::begin(bool cold)
	{
		confirm_count = 0;

		sysWindow::begin(cold);
		the_buttons.setButtonType(BUTTON_NO, BUTTON_EVENT_CLICK, LED_RED);
		the_buttons.setButtonType(BUTTON_YES,BUTTON_EVENT_CLICK, LED_GREEN);

		uint8_t type = SD.card()->type();
		int fat_type = type == SD_CARD_TYPE_SDHC ? 32 : 16;
		uint32_t blocks = SD.card()->cardSize();

		// see fileFormat.cpp for notes on this next statement

		if (blocks > 30435 * 2048)
			blocks = 30435 * 2048;
		uint32_t mb = (blocks + 2047)/2048;

		display(dbg_format,"cardCapacityMB(%u)",mb);

		uint16_t color = TFT_YELLOW;
		const char *msg = "Format %d MB as FAT%d ??";

		if (mb <= 6)
		{
			color = TFT_RED;
			msg = "%d MB is too small to format!";
			the_buttons.setButtonType(BUTTON_YES,0);
		}

		mylcd.setFont(Arial_16_Bold);
		mylcd.printfJustified(
			client_rect.xs + 30,
			client_rect.ys + 30,
			client_rect.xe - 60,
			client_rect.ye - 60,
			LCD_JUST_CENTER,
			color,
			TFT_BLACK,
			1,	// use bc
			msg, mb, fat_type);
	}



	void formatSDDialog::onButton(int row, int col, int event)
	{
		int num = row * NUM_BUTTON_COLS + col;
		if (num == BUTTON_YES)
		{
			if (!confirm_count)
			{
				confirm_count++;
				mylcd.setFont(Arial_16_Bold);
				mylcd.printJustified(
					client_rect.xs + 30,
					client_rect.ys + 60,
					client_rect.xe - 60,
					client_rect.ye - 60,
					LCD_JUST_CENTER,
					TFT_WHITE,
					TFT_BLACK,
					1,	// use bc
					"Confirm format SD Card??");
				the_buttons.setButtonBlink(BUTTON_YES,1);
			}
			else
			{
				fillRect(full_client_rect, TFT_BLACK);
				mylcd.setCursor(client_rect.xs + LINE_MARGIN,client_rect.ys + LINE_MARGIN);
				mylcd.setFont(Arial_14_Bold);
				sdFormatMsg("Formatting SD Card");

				bool ok =formatCard();
				sdFormatMsg(ok ?
					"REBOOTING ..." :
					"FORMAT FAILED!! ... REBOOTING",
					!ok);
				delay(5000);
				reboot(BUTTON_YES);
				while (1) {};
			}
		}
		else
		{
			endWindow(0);
		}
		display(0,"onButton finished",0);
	}

#endif	// 0 removed formatSDDialog from build