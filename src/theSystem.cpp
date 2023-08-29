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

#if WITH_MIDI_HOST
	#include "midiHost.h"
#endif


#define dbg_sys   0
	// show system startup, etc
#define dbg_midi 1
	// show receieved usb and serial midi  bytes


#define MIDI_ACTIVITY_TIMEOUT 			90
#define BATTERY_CHECK_TIME  			30000
#define UI_UPDATE_INTERVAL				30			// 33 frames per second


//----------------------------------
// normal timer loop
//----------------------------------
// The "normal" timer loop does the bulk of the work in the system.
// It is used to
//
//      (a) check the BUTTONS, PEDALS, and ROTARY states and
//          generate events based on their changes.
//      (b) process incoming or outgoing MIDI as necessary
//          to generate program related events based on them.
//      (c) re-enqueue the incoming and outgoing (processed) MIDI
//          messags for display.
//      (d) used variously by other objects to implement key
//          repeats, etc.
// I have more or less determined that the timers don't start again
//    until the handler has returned.
// At some point the timers use so much resources that the rest of
//    the system can become non functional.  The following values
//    work.

#define EXP_TIMER_INTERVAL 1000	// 5000
    // 5000 us = 5 ms == 200 times per second
#define EXP_TIMER_PRIORITY  192	// 240                     // lowest priority
    // compared to default priority of 128


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
#define INDICATOR_X 			280
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
// theSystem
//----------------------------------------


theSystem::theSystem()
{
	last_battery_level = 0;

	draw_pedals = 1;
	draw_title = 1;
	m_title = 0;
}


void theSystem::setTitle(const char *title)
{
	m_title = title;
	draw_title = 1;
}


void theSystem::begin()
{
	display(dbg_sys,"theSystem::begin()",0);

	for (int i=0; i<NUM_MIDI_PORTS; i++)
	{
		m_midi_activity[i] = millis();
		m_last_midi_activity[i] = 0;
	}

    theButtons.begin();
	thePedals.init();
	initRotary();

    m_timer.priority(EXP_TIMER_PRIORITY);
    m_timer.begin(timer_handler,EXP_TIMER_INTERVAL);

	if (!fileSystem::init())
	{
		my_error("theSystem COULD NOT START FILE SYSTEM!!",0);
        mylcd.setTextColor(TFT_YELLOW);
        mylcd.println("");
        mylcd.println("theSystem: COULD NOT START FILE SYSTEM!!");
		delay(5000);
	}

    theButtons.setButtonType(THE_SYSTEM_BUTTON, BUTTON_EVENT_LONG_CLICK, LED_ORANGE);
	// theButtons.setButtonType(9, BUTTON_EVENT_CLICK, LED_PURPLE);

	rig_machine.loadDefaultRig();

	display(dbg_sys,"returning from theSystem::begin()",0);
}



//-----------------------------------------
// events
//-----------------------------------------

void theSystem::onButton(int row, int col, int event)
{
	int num = row * NUM_BUTTON_COLS + col;
	if (num == THE_SYSTEM_BUTTON && event == BUTTON_EVENT_LONG_CLICK)
	{
		if (rig_machine.loadRig("default"))
		{
			draw_pedals = 1;
		}
	}
	else
	{
		rig_machine.onButton(row,col,event);
	}

}



//-----------------------------------------
// timer handlers
//-----------------------------------------

// static
void theSystem::timer_handler()
{
	uint32_t msg = usb_midi_read_message();  // read from device
	if (msg)
	{
		#if WITH_MIDI_HOST
			midi_host.write_packed(msg);
		#endif

		int save_proc_level = proc_level;
		proc_level = 1;
		display(dbg_midi,"usb:    0x%08x",msg);
		proc_level = 2;
		enqueueMidi(msg & MIDI_PORT_NUM_MASK, msg);
		proc_level = save_proc_level;
	}

	the_system.handleSerialData();
}


//--------------------------------------------------------
// Serial Port Handler
//--------------------------------------------------------
// Polls Serial and Serial3 for data.
// Handles incoming serial midi that starts with 0x0B and/or
// fileSystem command lines that start with "file_command:.*"
// Note that this implementation does not care about setting
// of PREF_FILE_SYSTEM_PORT ... it will accept file commands
// from either port.
//
// When a serial byte is received, this routine assumes a full
// packet is following (4 bytes for midi, or <cr-lf> for text)
// and reads the whole packet with blocking and a timeout


#define SERIAL_TIMEOUT  200

#define MAX_BASE64_BUF  10240
	// agreed upon in console.pm
#define MAX_SERIAL_TEXT_LINE (MAX_BASE64_BUF+32)
	// allow for "file_command:BASE64 " (13 + 6 + 1)


void theSystem::handleSerialData()
{
	// The main USB Serial is only expected to contain lines of text
	// Serial3 may contain either text or serial midi data

	int buf_ptr = 0;
	bool is_midi = false;
	bool started = false;
	bool finished = false;
	bool from_serial3 = 0;
	elapsedMillis line_timeout = 0;
	static char static_serial_buffer[MAX_SERIAL_TEXT_LINE+1];

	if (Serial.available())
	{
		started = true;
		while (!finished && buf_ptr<MAX_SERIAL_TEXT_LINE && line_timeout<SERIAL_TIMEOUT)
		{
			if (Serial.available())
			{
				int c = Serial.read();
				if (c == 0x0A)				// LF comes last
				{
					static_serial_buffer[buf_ptr++] = 0;
					finished = 1;

				}
				else if (c != 0x0D)			// skip CR
				{
					static_serial_buffer[buf_ptr++] = c;
					line_timeout = 0;
				}
			}
		}
	}
	else if (Serial3.available())
	{
		started = true;
		from_serial3 = 1;

		int c = Serial3.read();
		if (c == 0x0B)		// ONLY CC commands on channel 0
		{
			is_midi = 1;
			finished = true;
			static_serial_buffer[buf_ptr++] = c;
			for (int i=0; i<3; i++)
			{
				static volatile int fu = 0;
				while (!Serial3.available()) {fu++;}
				c = Serial3.read();
				static_serial_buffer[buf_ptr++] = c;
			}

		}
		else
		{
			line_timeout = 0;
			while (!finished && buf_ptr<MAX_SERIAL_TEXT_LINE)
			{
				if (c == 0x0A)			// LF comesl last
				{
					static_serial_buffer[buf_ptr++] = 0;
					finished = 1;
				}
				else
				{
					if (c != 0x0D)			// skip CR
					{
						static_serial_buffer[buf_ptr++] = c;
						line_timeout = 0;

					}
					while (!Serial3.available())
					{
						if (line_timeout>=SERIAL_TIMEOUT)
							break;
					}
 					c = Serial3.read();
				}
			}
		}
	}	// Serial3.available()


 	if (started && !finished)
	{
		my_error("Could not finish serial input from_serial3(%d) is_midi(%d) buf_ptr(%d) %s",
			from_serial3,
			is_midi,
			buf_ptr,
			line_timeout>SERIAL_TIMEOUT ? "TIMEOUT" : "");
		display_bytes(0,"BUF",(uint8_t*)static_serial_buffer,buf_ptr);
	}
	else if (is_midi)		// is_midi invariantly sets finished
	{
		int save_proc_level = proc_level;
		proc_level = 1;
		uint8_t *p = (uint8_t *) static_serial_buffer;
		display(dbg_midi,"serial: 0x%02x%02x%02x%02x",p[3],p[2],p[1],p[0]);
		proc_level = 2;
		enqueueMidi(MIDI_PORT_SERIAL,p);
		proc_level = save_proc_level;
	}
	else if (finished)
	{
		if (!strncmp(static_serial_buffer,"file_command:",13))
		{
			char *p_command = &static_serial_buffer[13];
			char *p_param = p_command;
			while (*p_param && *p_param != ' ') p_param++;
			if (*p_param == ' ') *p_param++ = 0;
			fileSystem::handleFileCommand(p_command,p_param);
		}
		else
		{
			static_serial_buffer[buf_ptr+1] = 0;
			my_error("theSystem got unexpected serial data from_serial3(%d) is_midi(%d) buf_ptr(%d) %s",
				from_serial3,
				is_midi,
				buf_ptr,
				line_timeout>SERIAL_TIMEOUT ? "TIMEOUT" : "");
			display_bytes(0,"BUF",(uint8_t*)static_serial_buffer,buf_ptr);
		}
	}
}




//--------------------------------------
// loop()
//--------------------------------------


void theSystem::loop()
	//  called from Arduino loop()
{
	// basics

    theButtons.task();
	thePedals.task();
	pollRotary();

	initQueryFTP();
		// query the FTP battery level on a timer

#if 0
	if (win->m_flags & WIN_FLAG_SHOW_PEDALS)
#endif	// 0
	{
		bool draw_pedal_values = false;
		int pedal_width = pedal_rect.width() / NUM_PEDALS;

		if (draw_pedals)
		{
			draw_pedals = false;
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
					thePedals.getPedal(i)->getName());

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
			expressionPedal *pedal = thePedals.getPedal(i);
			if (draw_pedal_values || pedal->displayValueChanged())
			{
				pedal->clearDisplayValueChanged();
				int v = pedal->getDisplayValue();

				#if 1	// #ifdef'd out while working on myLcd stuff

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

				#endif
			}
		}
	}

	//---------------------------
	// Top Title Frame
	//---------------------------

	bool draw_title_frame = draw_title;

	if (draw_title)
	{
		draw_title = false;
		fillRect(title_rect,TFT_BLACK);

		// title text

		draw_title = false;
        mylcd.setFont(Arial_16_Bold);
        mylcd.setCursor(5,10);
        mylcd.setTextColor(TFT_YELLOW);
        mylcd.print(m_title);

		// horizontal line

        mylcd.setDrawColor(TFT_YELLOW);
	    mylcd.drawLine(0,36,mylcd.width()-1,36);

		// midi indicator frames

        mylcd.setDrawColor(TFT_WHITE);
		for (int i=0; i<NUM_MIDI_PORTS; i++)
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

	if (draw_title_frame ||
		last_battery_level != ftp_battery_level)
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

		last_battery_level = ftp_battery_level;
	}


	// MIDI INDICATORS (always if changed)
	// remap from by output-cable  Di0,Di1,Do0,Do1,Hi0,Hi1,Ho0,Ho1
	// to by cable-output:         Di0,Do0, Di1,Do1,  Hi0,Ho0, Hi1,Ho1

	uint32_t now = millis();
	for (int cable_pair=0; cable_pair<NUM_MIDI_PORTS/2; cable_pair++)
	{
		for (int out=0; out<2; out++)
		{
			#define INDEX_MASK_HOST     0x04
			#define INDEX_MASK_OUTPUT   0x02
			#define INDEX_MASK_CABLE    0x01

			int i = ((cable_pair<<1)&INDEX_MASK_HOST) + (out*INDEX_MASK_OUTPUT) + (cable_pair&1);

			bool midi_on =
				now > m_midi_activity[i] &&
				now < m_midi_activity[i] + MIDI_ACTIVITY_TIMEOUT;

			if (draw_title_frame ||  midi_on != m_last_midi_activity[i])
			{
				m_last_midi_activity[i] = midi_on;
				int color = midi_on ?
					out ? TFT_RED : TFT_GREEN :
					0;

				mylcd.setDrawColor(color);
				mylcd.fillCircle(
					INDICATOR_X + cable_pair*INDICATOR_PAIR_SPACING + out*INDICATOR_SPACING,
					INDICATOR_Y,
					INDICATOR_RADIUS);
			}
		}
	}


	rig_machine.updateUI();

}	// theSystem::loop()
