//-------------------------------------------------
// the_system.cpp
//-------------------------------------------------

#include <myDebug.h>
#include "theSystem.h"
#include "prefs.h"
#include "myTFT.h"
#include "myLeds.h"
#include "buttons.h"
#include "pedals.h"
#include "rotary.h"
#include "fileSystem.h"
#include "midiQueue.h"
#include "ftp.h"
#include "rigMachine.h"
#include "rigExpression.h"		// for EXP_MIDI_PORT_SERIAL
#include "midiHost.h"
#include "winConfig.h"
#include <TeensyThreads.h>


#define dbg_sys   0
	// 0 = show system startup, etc
#define dbg_raw_midi 1
	// 0 = show received usb and serial midi
	// may create timing problems
#define dbg_win	  1
	// 0 = debug the window stack
#define dbg_file_command -1
	// 0 = show thread starting
	// -1 = show parse steps
	// -2 = show each character

#define MIDI_ACTIVITY_TIMEOUT 			150
#define BATTERY_CHECK_TIME  			30000
#define UI_UPDATE_INTERVAL				30			// 33 frames per second


//----------------------------------
// Timers Setup
//----------------------------------

#define EXP_TIMER_INTERVAL 5000
    // 5000 us = 5 ms == 200 times per second
#define EXP_TIMER_PRIORITY  240                     // lowest priority
    // compared to default priority of 128

#define EXP_CRITICAL_TIMER_INTERVAL 1000
	// 1000 us = 1 ms = 1000 times per second
#define EXP_CRITICAL_TIMER_PRIORITY  192
    // Available priorities:
    // Cortex-M4: 0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240
    // Cortex-M0: 0,64,128,192


//----------------------------------------
// constant screen layout
//----------------------------------------

#define BATTERY_X       435
#define BATTERY_Y       12
#define BATTERY_FRAME   2
#define BATTERY_WIDTH   36
#define BATTERY_HEIGHT  16

#define INDICATOR_Y      	 	20
#define INDICATOR_RADIUS  	  	5
#define INDICATOR_X 			320
#define INDICATOR_PAIR_SPACING  40
#define INDICATOR_SPACING    	15

#define PEDAL_TEXT_AREA_HEIGHT  30


//--------------------------------
// global variables
//--------------------------------

theSystem the_system;

int_rect tft_rect(0,0,479,319);				// full screen
int_rect title_rect(0,0,479,35);			// not including line
int_rect full_client_rect(0,37,479,319);	// under line to bottom of screen
int_rect client_rect(0,37,479,235);			// under line to above pedals
int_rect pedal_rect(0,235,479,319);			// 89 high, starting at 230



//----------------------------------------
// sysWindow (base class)
//----------------------------------------

// virtual
void sysWindow::endWindow(uint16_t param)
{
	display(dbg_win,"sysWindow::endWindow(%s,0x%08x)",name(),param);
	the_system.endWindow(this,param);
}

// virtual
void sysWindow::begin(bool cold)
	// windows are responsible for setting the system button
{
	display(dbg_win,"sysWindow::begin(%s,%d)",name(),cold);
	the_system.setTitle(name());
	the_buttons.clear();
	fillRect(full_client_rect, TFT_BLACK);
}


//----------------------------------------
// theSystem
//----------------------------------------


theSystem::theSystem()
{
	m_battery_level = 0;

	m_draw_pedals = 1;
	m_draw_title = 1;
	m_title = 0;
}


void theSystem::setTitle(const char *title, bool draw_pedals)
{
	m_title = title;
	m_draw_title = 1;
	m_draw_pedals |= draw_pedals;
}


void theSystem::begin()
{
	display(dbg_sys,"theSystem::begin()",0);

	for (int i=0; i<NUM_ACTIVITY_INDICATORS; i++)
	{
		m_midi_activity[i] = millis();
		m_last_midi_activity[i] = 0;
	}

    the_buttons.begin();
	the_pedals.init();
	initRotary();

    m_timer.priority(EXP_TIMER_PRIORITY);
    m_timer.begin(timer_handler,EXP_TIMER_INTERVAL);
    m_critical_timer.priority(EXP_CRITICAL_TIMER_PRIORITY);
    m_critical_timer.begin(critical_timer_handler,EXP_CRITICAL_TIMER_INTERVAL);

    the_buttons.setButtonType(THE_SYSTEM_BUTTON, BUTTON_EVENT_LONG_CLICK, LED_ORANGE);
	// the_buttons.setButtonType(9, BUTTON_EVENT_CLICK, LED_PURPLE);

	display(dbg_sys,"returning from theSystem::begin()",0);
}


//-----------------------------------------
// timer handlers
//-----------------------------------------

// static
void theSystem::timer_handler()
{
	the_system.handleSerialData();
	the_pedals.task();
	pollRotary();
	dequeueMidi();
}


// static
void theSystem::critical_timer_handler()
{
	uint32_t msg32 = usb_midi_read_message();  // read from device
	if (msg32)
	{
		if (dbg_raw_midi <= 0)
			display_level(dbg_raw_midi,0,"usb:    0x%08x",msg32);

		// We only write through to the midi host if spoof FTP
		// in which case we also manually diddle the indicator

		if (prefs.SPOOF_FTP)
		{
			midi_host.write_packed(msg32);
			the_system.midiActivity(ACTIVITY_INDICATOR_HOST_IN);
		}

		// (possibly) enqueue the message from the USB port

		enqueueMidi(false, msg32 & MIDI_PORT_NUM_MASK, msg32);
	}
}


//--------------------------------------------------------
// Command Parse
//--------------------------------------------------------
// File text start with file_command or file_message,
// followed by the req_num, size, and data
//
// file_command|file_message	\t req_num \t size \t data


#define SERIAL_TIMEOUT  200

#define FILE_COMMAND_SIG		"file_command"
#define FILE_MESSAGE_SIG		"file_message"

#define FILE_COMMAND_SIG_LEN	12
#define MAX_NUM_LENGTH			10

#define MIN_COMMAND_BUF_LEN		3   	// OK
#define MAX_COMMAND_BUF_LEN		15000   // BASE64 size offset ENCODED_CONTENT
	// probably have to tune this last one


typedef struct
{
	int state;
		// 0 = looking for SIG
		// 1 = got SIG, parsing req_num
		// 2 = got req_num parsing size
		// 3 = got size, created buf, adding bytes
	int off;
		// offset within the item being parsed, depending on state

	uint32_t timeout;

	// parsed contents

	int type;
		// 0 == file_command
		// 1 == file_message

	char req_num[MAX_NUM_LENGTH+1];
	char size[MAX_NUM_LENGTH+1];

	int  len;   // length of allocated buffer which is size + 1
	char *buf;

} parseCommand_t;


static parseCommand_t parse_command[2];


static void initParseCommand(parseCommand_t *pcmd)
{
	if (pcmd->buf)
		free(pcmd->buf);
	memset(pcmd,0,sizeof(parseCommand_t));
}


static void handleChar(bool is_serial, char c)
{
	if (!ACTIVE_FILE_SYS_DEVICE)
	{
		my_error("No ACTIVE_FILE_SYS_DEVICE in handleChar()",0);
		return;
	}

	parseCommand_t *pcmd = &parse_command[is_serial];

	display_level(dbg_file_command+2,1,"state(%d) off(%d) char=%c 0x%02x",
		pcmd->state,
		pcmd->off,
		c>=' '?c:'.',c);

	bool ok = 1;
	bool done = 0;

	if (pcmd->state == 0)									// parsing signature
	{
		char expected = pcmd->type ?
			FILE_MESSAGE_SIG[pcmd->off]:
			FILE_COMMAND_SIG[pcmd->off];

		if (c == '\t' && pcmd->off == FILE_COMMAND_SIG_LEN)	// got the signature
		{
			pcmd->off = 0;		// reset for next state
			pcmd->state++;       // advance to next state
			display_level(dbg_file_command+1,2,"handleChar() got %s",
				pcmd->type ? "file_message" : "file_command");
		}
		else if (c == expected)
		{
			pcmd->off++;
		}
		else if (!pcmd->type && pcmd->off == 5 && c == 'm')
		{
			pcmd->type = 1;
			pcmd->off++;
		}
		else
		{
			ok = 0;
			if (pcmd->off)
				my_error("handleChar() type(%d) off(%d) illegal char in SIG '%c'=0x%02x",
					pcmd->type,
					pcmd->off,
					c>=' '?c:'.',c);
		}
	}
	else if (pcmd->state == 1)						// parsing req_num
	{
		if (c == '\t')								// got length terminator
		{
			pcmd->req_num[pcmd->off] = 0;
			display_level(dbg_file_command+1,2,"handleChar(%s) got req_num(%s)",
				pcmd->type?"file_message":"file_command",
				pcmd->req_num);

			pcmd->off = 0;		// reset for next state
			pcmd->state++;		// next state
		}
		else if (pcmd->off >= MAX_NUM_LENGTH)		// too big
		{
			ok = 0;
			my_error("handleChar(%s) off(%d) req_num overflow",
				pcmd->type?"file_message":"file_command",
				pcmd->off);
		}
		else if (c >= '0' && c <= '9')
		{
			pcmd->req_num[pcmd->off++] = c;
		}
		else
		{
			ok = 0;
			my_error("handleChar(%s) off(%d) illegal char in req_num '%c'=0x%02x",
				pcmd->type?"file_message":"file_command",
				pcmd->off,
				c>=' '?c:'.',c);
		}
	}
	else if (pcmd->state == 2)						// parsing size
	{
		if (c == '\t')								// got length terminator
		{
			pcmd->size[pcmd->off] = 0;
			display_level(dbg_file_command+1,2,"handleChar(%s,%s) got size(%s)",
				pcmd->type?"file_message":"file_command",
				pcmd->req_num,
				pcmd->size);

			pcmd->len = atoi(pcmd->size);
			if (pcmd->len < MIN_COMMAND_BUF_LEN ||
				pcmd->len > MAX_COMMAND_BUF_LEN)
			{
				ok = 0;
				my_error("handleChar(%s,%s) len(%d) must be between %d and %d",
					pcmd->type?"file_message":"file_command",
					pcmd->req_num,
					pcmd->len,
					MIN_COMMAND_BUF_LEN,
					MAX_COMMAND_BUF_LEN);
			}
			else
			{
				pcmd->buf = (char *) malloc(pcmd->len + 1);		// allocate buffer
				if (!pcmd->buf)
				{
					ok = 0;
					my_error("handleChar(%s,%s) unable to allocate buffer of len(%d)",
						pcmd->type?"file_message":"file_command",
						pcmd->req_num,
						pcmd->len);
				}
				else
				{
					pcmd->off = 0;		// reset for next state
					pcmd->state++;		// next state
				}
			}
		}
		else if (pcmd->off >= MAX_NUM_LENGTH)		// too big
		{
			ok = 0;
			my_error("handleChar(%s,%s) off(%d) size overflow",
				pcmd->type?"file_message":"file_command",
				pcmd->req_num,
				pcmd->off);
		}
		else if (c >= '0' && c <= '9')
		{
			pcmd->size[pcmd->off++] = c;
		}
		else
		{
			ok = 0;
			my_error("handleChar(%s,%s) off(%d) illegal char in size '%c'=0x%02x",
				pcmd->type?"file_message":"file_command",
				pcmd->req_num,
				pcmd->off,
				c>=' '?c:'.',c);
		}
	}
	else if (pcmd->state == 3)			// adding characters to the file buffer
	{
		if (pcmd->off > pcmd->len)
		{
			ok = 0;
			my_error("handleChar(%s,%s) buffer overflow at off(%d)",
				pcmd->type?"file_message":"file_command",
				pcmd->req_num,
				pcmd->off);
		}
		else if (c == '\n')
		{
			if (pcmd->off == pcmd->len)
			{
				done = 1;
				pcmd->buf[pcmd->off] = 0;		// terminate the buffer
			}
			else
			{
				ok = 0;
				my_error("handleChar(%s,%s) length mismatch off(%d) != len(%d)",
					pcmd->type?"file_message":"file_command",
					pcmd->req_num,
					pcmd->off,
					pcmd->len);
			}
		}
		else		// add character to file_command cuffer
		{
			pcmd->buf[pcmd->off++] = c;
		}
	}

	if (done)	// start the command or send the message
	{
		pcmd->timeout = 0;
		display_level(dbg_file_command,0,"handleChar() %s %s(%s) len=%d",
				pcmd->type?"queing":"starting",
				pcmd->type?"file_message":"file_command",
				pcmd->req_num,
				pcmd->len);
		int req_num = atoi(pcmd->req_num);
		if (pcmd->type)
		{
			if (!hasFileSystem())
			{
				warning(dbg_file_command,"handleChar() noFileSytem file_message(%d) dropping buffer of len(%d)",
					req_num,
					pcmd->len);
				free(pcmd->buf);
			}
			else if (!getCommand(req_num))
			{
				warning(dbg_file_command,"handleChar() find queue for file_message(%d) dropping buffer of len(%d)",
					req_num,
					pcmd->len);
				free(pcmd->buf);
			}
			else
			{
				if (!addCommandQueue(req_num,pcmd->buf))
				{
					warning(dbg_file_command,"handleChar() could not queue file_message(%d) dropping buffer of len(%d)",
						req_num,
						pcmd->len);
					free(pcmd->buf);
				}
			}
		}
		else if (!startCommand(req_num,pcmd->buf))
		{
			warning(dbg_file_command,"handleChar() could not start file_command(%d) dropping buffer of len(%d)",
				req_num,
				pcmd->len);
			free(pcmd->buf);
		}

		pcmd->buf = 0;
	}

	// !ok or done - init for new parse

	if (done || !ok)
		initParseCommand(pcmd);
	else if (ok)
		pcmd->timeout = millis();

}	// handleChar



//--------------------------------------------------------
// Serial Port Handler
//--------------------------------------------------------
// Polls Serial and SERIAL_DEVICE for data.
//
// At this time TE2 only allows Serial Midi over the SERIAL
// device or file_commands over either USB or SERIAL.
//
// Midi Packets start with 0x0B which should never be in plain text.
//
// File_commands start with file_command \t length \t reqnum \t data
// When a file_command is received, this routine assumes a full
// packet is following (4 bytes for midi, or <cr-lf> for text)
// and reads the whole packet with  a timeout


void theSystem::handleSerialData()
{
	// The main USB Serial is only expected to contain lines of text
	// SERIAL_DEVICE may contain either text or serial midi data

	for (int i=0; i<2; i++)
	{
		parseCommand_t *pcmd = &parse_command[i];
		if (pcmd->timeout && millis() - pcmd->timeout > SERIAL_TIMEOUT)
		{
			my_error("%s command timeout",i ? "SERIAL" : "USB");
			initParseCommand(pcmd);
		}
	}

	while (Serial.available())
	{
		int c = Serial.read();
		handleChar(0,c);
	}

	// only serialMidi sends 0x0B!!
	// and they can be sent in the middle of regular lines of text

	while (SERIAL_DEVICE.available())
	{
		int c = SERIAL_DEVICE.read();
		if (c == 0x0B)
		{
			uint8_t midi_buf[4];
			midi_buf[0] = c;

			int i = 1;
			uint32_t midi_timeout = millis();
			while (i<4 && millis() - midi_timeout < SERIAL_TIMEOUT)
			{
				if (SERIAL_DEVICE.available())
				{
					midi_buf[i++] = SERIAL_DEVICE.read();
					midi_timeout = millis();
				}
			}

			if (i < 4)
				my_error("serial midi timeout(%d)",i);
			else
			{
				if (dbg_raw_midi <= 0)
					display_level(dbg_raw_midi,0,"serial: 0x%02x%02x%02x%02x",
						midi_buf[3],
						midi_buf[2],
						midi_buf[1],
						midi_buf[0]);
				enqueueMidi(false, MIDI_PORT_SERIAL,midi_buf);
			}
		}
		else
		{
			handleChar(1,c);
		}
	}	// SERIAL_DEVICE.available();
}	// handleSerialData()



//-----------------------------------------
// windows
//-----------------------------------------

sysWindow *theSystem::getTopWindow()
{
	if (m_num_windows)
		return m_window_stack[m_num_windows-1];
	return 0;
}


void theSystem::startWindow(sysWindow *win)
{
	display(dbg_win,"startWindow(%d:%s)",m_num_windows,win->name());
	if (m_num_windows >= MAX_WINDOW_STACK)
	{
		my_error("WINDOW STACK OVERFLOW!!!",0);
		return;
	}
	m_window_stack[m_num_windows++] = win;
	win->begin(true);
}


void theSystem::swapWindow(sysWindow *win, uint16_t param)
{
	display(dbg_win,"swapWindow(%d:%s,0x%04x)",m_num_windows,win->name(),param);
	if (!m_num_windows)		// hmmmm ... who would call this
	{
		warning(0,"swapWindow(%s,0x%04x) called with no windows on stack!",win->name(),param);
		startWindow(win);
		return;
	}
	sysWindow *old = getTopWindow();
	old->end();
	m_window_stack[m_num_windows-1] = win;
	win->begin(true);
}


void theSystem::endWindow(sysWindow *cur, uint16_t param)
	// cur is currently only to verify caller's correctness
	// as windows typically end themselves.  It *could*
	// eventually be used to exit known subwindows.
{
	display(dbg_win,"theSystem::endWindow(%d:%s,0x%04x)",m_num_windows,cur->name(),param);
	if (!m_num_windows)
	{
		my_error("WINDOW STACK UNDERFLOW!!!",0);
		return;
	}

	// sanity check

	sysWindow *tos = getTopWindow();
	if (cur != tos)
	{
		my_error("Attempt to endWindow(%s) when TOS is win(%s)!",cur->name(), tos->name());
		return;
	}

	// we don't want to decrement m_num_windows to zero until
	// after we call restartRig() so the loop doesn't start prematurely,
	// so we look under the current tos with >1 and [-2] to get the parent

	sysWindow *parent = m_num_windows > 1 ? m_window_stack[m_num_windows-2] : 0;

	// call parent onChildEnd() if there's a perent and end the
	// current tos if there's no parent, or the parent says ok.

	bool parent_ok = !parent || parent->onChildEnd(param);
	if (parent_ok)
	{
		cur->end();

		// begin the parent if it exists

		if (parent)
			parent->begin(false);
	}

	// now restart the rig if IT was the parent window
	// restartRig() will correctly set m_rig_loaded or an RIG_LOAD_STATE_ERROR_START
	// and thus it becomes safe to start calling rig_machine.updateUI again.

	if (!parent)
	{
		m_draw_pedals = 1;
		fillRect(full_client_rect, TFT_BLACK);
		rig_machine.restartRig();
	}

	// and finally decrement the stack pointer if there
	// was no parent, or if the parent onChildEnd returned true

	if (parent_ok)
		m_num_windows--;

}	// endWindow()



//-----------------------------------------
// events
//-----------------------------------------

void theSystem::onButton(int row, int col, int event)
{
	int num = row * NUM_BUTTON_COLS + col;
	if (!m_num_windows &&
		num == THE_SYSTEM_BUTTON &&
		event == BUTTON_EVENT_LONG_CLICK)
	{
		startWindow(&win_config);
		// rig_machine.loadRig("default");
	}
	else if (m_num_windows)
	{
		getTopWindow()->onButton(row,col,event);
	}
	else
	{
		rig_machine.onButton(row,col,event);
	}

}


//--------------------------------------
// loop()
//--------------------------------------


void theSystem::loop()
	//  called from Arduino loop()
{
    the_buttons.task();
		// cannot be called from timer_handler()
		// because buttons can change colors and/or
		// bump the window number, and we don't want to
		// be in the middle of a rigMachine button update
		// at that point.

	initQueryFTP();
		// query the FTP battery level on a timer

	if (!m_num_windows)
	{
		bool draw_pedal_values = false;
		int pedal_width = pedal_rect.width() / NUM_PEDALS;

		if (m_draw_pedals)
		{
			m_draw_pedals = false;
			draw_pedal_values = true;

			mylcd.fillRect(
				pedal_rect.xs,
				pedal_rect.ys,
				pedal_rect.width(),
				PEDAL_TEXT_AREA_HEIGHT,
				TFT_YELLOW);

			mylcd.setFont(Arial_18_Bold);   // Arial_16);
			mylcd.setTextColor(TFT_BLACK);
			mylcd.setDrawColor(TFT_YELLOW);

			for (int i=0; i<NUM_PEDALS; i++)
			{
				mylcd.printfJustified(
					i*pedal_width,
					pedal_rect.ys + 5,
					pedal_width,
					PEDAL_TEXT_AREA_HEIGHT,
					LCD_JUST_CENTER,
					TFT_BLACK,
					TFT_YELLOW,
					false,
					"%s",
					the_pedals.getPedal(i)->getName());

				if (i && i<NUM_PEDALS)
					mylcd.drawLine(
						i*pedal_width,
						pedal_rect.ys + PEDAL_TEXT_AREA_HEIGHT,
						i*pedal_width,
						pedal_rect.ye);
			}
		}

		// draw the pedal numbers if they've changed

		for (int i=0; i<NUM_PEDALS; i++)
		{
			expressionPedal *pedal = the_pedals.getPedal(i);
			if (draw_pedal_values || pedal->displayValueChanged())
			{
				pedal->clearDisplayValueChanged();
				int v = pedal->getDisplayValue();

				mylcd.setFont(Arial_40_Bold);   // Arial_40);
				mylcd.setTextColor(TFT_WHITE);

				mylcd.printfJustified(
					12+i*pedal_width,
					pedal_rect.ys + PEDAL_TEXT_AREA_HEIGHT + 7,
					100,
					pedal_rect.height() - PEDAL_TEXT_AREA_HEIGHT + 20,
					LCD_JUST_CENTER,
					TFT_WHITE,
					TFT_BLACK,
					true,
					"%d",
					v);
			}
		}
	}

	//---------------------------
	// Top Title Frame
	//---------------------------

	bool draw_title_frame = m_draw_title;

	if (m_draw_title)
	{
		m_draw_title = false;
		fillRect(title_rect,TFT_BLACK);

		// title text

        mylcd.setFont(Arial_16_Bold);
        mylcd.setCursor(5,10);
        mylcd.setTextColor(TFT_YELLOW);
        mylcd.print(m_title);

		// horizontal line

        mylcd.setDrawColor(TFT_YELLOW);
	    mylcd.drawLine(0,36,mylcd.width()-1,36);

		// midi indicator frames

        mylcd.setDrawColor(TFT_WHITE);
		for (int i=0; i<NUM_ACTIVITY_INDICATORS; i++)
		{
			mylcd.fillCircle(
				INDICATOR_X + (i/2)*INDICATOR_PAIR_SPACING + (i&1)*INDICATOR_SPACING,
				INDICATOR_Y,
				INDICATOR_RADIUS + 1);
		}
	}

	//----------------------------------------
	// battery indicator frame and value
	//----------------------------------------

	if (draw_title_frame || m_battery_level != ftp_battery_level)
	{
		//  battery indicator frame

		int battery_frame_color = ftp_battery_level == -1 ?
			TFT_DARKGREY :
			TFT_YELLOW;

		mylcd.fillRect(
			BATTERY_X,
			BATTERY_Y,
			BATTERY_WIDTH,
			BATTERY_HEIGHT,
			battery_frame_color);
		mylcd.fillRect(
			BATTERY_X + BATTERY_WIDTH -1,
			BATTERY_Y + (BATTERY_HEIGHT/4),
			4,
			(BATTERY_HEIGHT/2),
			battery_frame_color);
		mylcd.fillRect(
			BATTERY_X + BATTERY_FRAME,
			BATTERY_Y + BATTERY_FRAME,
			BATTERY_WIDTH - 2*BATTERY_FRAME,
			BATTERY_HEIGHT - 2*BATTERY_FRAME,
			TFT_BLACK);

		float pct = getFTPBatteryPct();
		int color = ftp_battery_level == -1 ? TFT_LIGHTGREY : (pct <= .15 ? TFT_RED : TFT_DARKGREEN);

		// display(0,"battery_level=%d   pct=%0.2f",ftp_battery_level,pct);

		float left_width = pct * ((float) BATTERY_WIDTH - 2*BATTERY_FRAME);
		float right_width = (1-pct) * ((float) BATTERY_WIDTH - 2*BATTERY_FRAME);
		int left_int = left_width;
		int right_int = right_width;

		mylcd.fillRect(
			BATTERY_X + BATTERY_FRAME,
			BATTERY_Y + BATTERY_FRAME,
			left_int,
			BATTERY_HEIGHT - 2*BATTERY_FRAME,
			color);

		if (right_int > 0)
			mylcd.fillRect(
				BATTERY_X + BATTERY_FRAME + left_int,
				BATTERY_Y + BATTERY_FRAME,
				right_int,
				BATTERY_HEIGHT - 2*BATTERY_FRAME,
				TFT_BLACK);

		m_battery_level = ftp_battery_level;
	}


	// MIDI INDICATORS (always if changed)

	uint32_t now = millis();
	for (int i=0; i<NUM_ACTIVITY_INDICATORS; i++)
	{
		bool midi_on =
			now > m_midi_activity[i] &&
			now < m_midi_activity[i] + MIDI_ACTIVITY_TIMEOUT;

		if (draw_title_frame ||  midi_on != m_last_midi_activity[i])
		{
			m_last_midi_activity[i] = midi_on;
			int color = midi_on ?
				(i & 1) ? TFT_RED : TFT_GREEN :
				TFT_BLACK;

			mylcd.setDrawColor(color);
			mylcd.fillCircle(
				INDICATOR_X + (i/2)*INDICATOR_PAIR_SPACING + (i&1)*INDICATOR_SPACING,
				INDICATOR_Y,
				INDICATOR_RADIUS);
		}
	}

	if (m_num_windows)
		getTopWindow()->updateUI();
	else
		rig_machine.updateUI();

}	// theSystem::loop()
