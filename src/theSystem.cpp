
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


#include "rigParser.h"


#if 0

#include "ftp_defs.h"


#include "configSystem.h"
#include "rigLooper.h"
#include "rigTest.h"
#include "rigMidiHost.h"
#endif // 0


#define dbg_sys   0

#define MIDI_ACTIVITY_TIMEOUT 			90

#define BATTERY_CHECK_TIME  			30000

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

#define EXP_TIMER_INTERVAL 5000
    // 5000 us = 5 ms == 200 times per second
#define EXP_TIMER_PRIORITY  240                     // lowest priority
    // compared to default priority of 128


//--------------------------------
// global variables
//--------------------------------

aSystem theSystem;

int_rect tft_rect(0,0,479,319);				// full screen
int_rect title_rect(0,0,479,35);			// not including line
int_rect full_client_rect(0,37,479,319);	// under line to bottom of screen
int_rect pedal_rect(0,235,479,319);			// 89 high, starting at 230
int_rect client_rect(0,37,479,235);			// under line to above pedals

#define SYNTH_RECT_HEIGHT 70
#define SONG_STATE_WIDTH  100
#define SONG_MSG1_WIDTH   120

int_rect synth_rect(
	client_rect.xs,
	client_rect.ys,
	client_rect.xe,
	client_rect.ys + SYNTH_RECT_HEIGHT-1);

int_rect song_title_rect(
	client_rect.xs,
	synth_rect.ye,
	client_rect.xe - SONG_STATE_WIDTH,
	synth_rect.ye + 35);

int_rect song_state_rect(
	song_title_rect.xe,
	synth_rect.ye,
	client_rect.xe,
	song_title_rect.ye);

int_rect song_msg_rect[2];
	// assigned in begin()





#if 0

const char *rig_names[MAX_EXP_RIGS];

//----------------------------------------
// aWindow (base class)
//----------------------------------------


// virtual
void aWindow::endModal(uint32_t param)
	// currently
{
    // called by modal windows when they end themselves
	theSystem.endModal(this,param);
}



//----------------------------------------
// aSystem
//----------------------------------------

#endif // 0


aSystem::aSystem()
{
	last_battery_level = 0;

	draw_pedals = 1;
	draw_title = 1;
	m_title = 0;

	song_msg_rect[0].assign(
		client_rect.xs,
		song_title_rect.ye + 10,
		client_rect.xs + SONG_MSG1_WIDTH - 1,
		client_rect.ye);

	song_msg_rect[1].assign(
		song_msg_rect[0].xe,
		song_title_rect.ye + 10,
		client_rect.xe,
		client_rect.ye);

#if 0

    m_num_rigs = 0;
    m_cur_rig_num = -1;
    m_prev_rig_num = 0;

	m_num_modals = 0;

    for (int i=0; i<MAX_EXP_RIGS; i++)
        m_rigs[i] = 0;

#endif // 0

}





void aSystem::setTitle(const char *title)
{
	m_title = title;
	draw_title = 1;
}




void aSystem::begin()
{
	display(dbg_sys,"aSystem::begin()",0);

	for (int i=0; i<NUM_MIDI_PORTS; i++)
	{
		m_midi_activity[i] = millis();
		m_last_midi_activity[i] = 0;
	}

    theButtons.init();
	thePedals.init();
	initRotary();

    m_timer.priority(EXP_TIMER_PRIORITY);
    m_timer.begin(timer_handler,EXP_TIMER_INTERVAL);

	if (!fileSystem::init())
	{
        mylcd.Set_Text_colour(TFT_YELLOW);
        mylcd.println("");
        mylcd.println("aSystem: COULD NOT START FILE SYSTEM!!");
		delay(10000);
	}

    theButtons.setButtonType(4, BUTTON_EVENT_CLICK, LED_BLACK, LED_BLACK, LED_BLACK, LED_PURPLE );
        // int num,
        // int mask,
        // int default_color=-1,
        // int selected_color=-1,
        // int touch_color=-1,
        // int pressed_color=-1);

#if 0
    addRig(new configSystem());
    addRig(new rigLooper());
    // addRig(new rigNew());
    // addRig(new rigOld());
	addRig(new rigTest());
    addRig(new rigMidiHost());

	song_msg_rect[0].assign(
		client_rect.xs,
		song_title_rect.ye + 10,
		client_rect.xs + SONG_MSG1_WIDTH - 1,
		client_rect.ye);

	song_msg_rect[1].assign(
		song_msg_rect[0].xe,
		song_title_rect.ye + 10,
		client_rect.xe,
		client_rect.ye);

	for (int i=0; i<m_num_rigs; i++)
		rig_names[i] = m_rigs[i]->short_name();

	setPrefMax(PREF_RIG_NUM,m_num_rigs-1);
	setPrefStrings(PREF_RIG_NUM,rig_names);

    // get rig_num from prefs and activate it

    int rig_num = getPref8(PREF_RIG_NUM);
    if (rig_num >= m_num_rigs)
        rig_num = m_num_rigs - 1;

    // rig_num = 0;
        // override prefs setting
        // for working on a particular rig

    activateRig(rig_num);
        // show the first windw


#endif // 0

	display(dbg_sys,"returning from aSystem::begin()",0);
}


//-------------------------------------------------
// Rig management
//-------------------------------------------------

#if 0

void aSystem::addRig(aWindow *pRig)
{
    if (m_num_rigs >= MAX_EXP_RIGS + 1)
    {
        my_error("TOO MANY RIGS! %d",m_num_rigs+1);
        return;
    }
    m_rigs[m_num_rigs++] = pRig;
}


void aSystem::activateRig(int new_rig_num)
{
	display(dbg_sys,"activateRig(%d)",new_rig_num);

    if (new_rig_num >= m_num_rigs)
    {
        my_error("attempt to activate illegal rig number %d",new_rig_num);
        return;
    }

    // deactivate previous rig

    if (m_cur_rig_num >= 0)
    {
        getCurRig()->end();
        m_prev_rig_num = m_cur_rig_num;
    }

	// start the new rig

    m_cur_rig_num = new_rig_num;
	startWindow(getCurRig(),false);

    // add the system long click handler

	theButtons.getButton(0,THE_SYSTEM_BUTTON)->addLongClickHandler();
}


//----------------------------------------
// window management
//----------------------------------------

void aSystem::startWindow(aWindow *win, bool warm)
{
	display(dbg_sys,"startWindow(%d,%s)",warm,win->name());

	theButtons.clear();
	mylcd.Fill_Screen(0);
	if (!(win->m_flags & WIN_FLAG_OWNER_TITLE))
		setTitle(win->name());
	else
		draw_title = 1;
	if (win->m_flags & WIN_FLAG_SHOW_PEDALS)
		draw_pedals = 1;
	win->begin(warm);
}


void aSystem::startModal(aWindow *win)
{
	display(dbg_sys,"startModal(%s)",win->name());

	if (m_num_modals >= MAX_MODAL_STACK)
	{
		my_error("NUMBER OF MODAL WINDOWS EXCEEDED",m_num_modals);
		return;
	}

	if (!m_num_modals)
		getCurRig()->end();

	m_modal_stack[m_num_modals++] = win;

	startWindow(win,false);
}



void aSystem::swapModal(aWindow *win, uint32_t param)
{
	display(dbg_sys,"swapModal(%s,0x%08x)",win->name(),param);

	if (!m_num_modals)
	{
		startModal(win);
		return;
	}

	aWindow *old = getTopModalWindow();
	old->end();

	m_modal_stack[m_num_modals-1] = win;
	startWindow(win,true);

	// old->onEndModal(old,param);
	if (old->m_flags & WIN_FLAG_DELETE_ON_END)
		delete old;

}




aWindow *aSystem::getTopModalWindow()
{
	if (m_num_modals)
		return m_modal_stack[m_num_modals-1];
	return 0;
}


void aSystem::endModal(aWindow *win, uint32_t param)
	// currently always acts on top of stack,
	// api allows for closing a window in the middle,
	// though it will not work at this time.
{
	display(dbg_sys,"aSystem::endModal(%s,0x%08x)",win->name(),param);

	m_num_modals--;
	aWindow *new_win = m_num_modals ?
		getTopModalWindow() :
		getCurRig();

	startWindow(new_win,true);
	if (!m_num_modals && m_cur_rig_num)
	{
		// reset the system button handler
		theButtons.getButton(0,THE_SYSTEM_BUTTON)->addLongClickHandler();
		draw_pedals = 1;
	}

	new_win->onEndModal(win,param);
	if (win->m_flags & WIN_FLAG_DELETE_ON_END)
		delete win;
}

#endif // 0

//-----------------------------------------
// events
//-----------------------------------------

void aSystem::buttonEvent(int row, int col, int event)
{
	int num = row * NUM_BUTTON_COLS + col;
	if (num == 4)
		parseRig();

#if 0
	int num = row * NUM_BUTTON_COLS + col;

	// modal windows get the event directly

	if (m_num_modals)
	{
		getTopModalWindow()->onButtonEvent(row,col,event);
	}

    // intercept long click on THE_SYSTEM_BUTTON
	// from rigs to go to the configSystem ...

	else if (num == THE_SYSTEM_BUTTON &&
 			 m_cur_rig_num &&
			 event == BUTTON_EVENT_LONG_CLICK)
	{
		setLED(THE_SYSTEM_BUTTON,LED_PURPLE);
		activateRig(0);
	}

	// else let the current rig have it

	else
	{
		getCurRig()->onButtonEvent(row,col,event);
	}
#endif // 0

}



//-----------------------------------------
// timer handlers
//-----------------------------------------


// static
void aSystem::timer_handler()
{
	// basics

    theButtons.task();
	thePedals.task();
	pollRotary();
	theSystem.handleSerialData();

	// get and enque midi messages
	// moved from old critical_timer_handler()

	uint32_t msg = usb_midi_read_message();  // read from device

    if (msg)
    {
		int pindex = (msg >> 4) & PORT_INDEX_MASK;
		theSystem.midiActivity(pindex);
			// the message comes in on port index 0 or 1
			// PORT_INDEX_DUINO_INPUT0 or PORT_INDEX_DUINO_INPUT1

        // enqueue it for processing (display from device)
		// if we are monitoring the input port, or it is the remote FTP

		if (prefs.MONITOR_PORT[pindex] || (  			// if monitoring the port, OR
			prefs.FTP_ENABLE && INDEX_CABLE(pindex)))	// FTP enabled and cabler == 1
		{
	        enqueueProcess(msg);
		}
    }

    // process incoming and outgoing midi events

    dequeueProcess();

#if 0

	// call window handler
	if (theSystem.m_num_modals)
		theSystem.getTopModalWindow()->timer_handler();
	else
	    theSystem.getCurRig()->timer_handler();
#endif // 0
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

#define MAX_BASE64_BUF  10240
	// agreed upon in console.pm
#define MAX_SERIAL_TEXT_LINE (MAX_BASE64_BUF+32)
	// allow for "file_command:BASE64 " (13 + 6 + 1)


#define SERIAL_TIMEOUT  200		   // ms
char static_serial_buffer[MAX_SERIAL_TEXT_LINE+1];

volatile int fu = 0;


void aSystem::handleSerialData()
{
	// The main USB Serial is only expected to contain lines of text
	// Serial3 may contain either text or serial midi data

	int buf_ptr = 0;
	bool is_midi = false;
	bool started = false;
	bool finished = false;
	elapsedMillis line_timeout = 0;
	bool from_serial3 = 0;

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
		if (c == 0x0B)
		{
			is_midi = 1;
			static_serial_buffer[buf_ptr++] = c;
			for (int i=0; i<3; i++)
			{
				while (!Serial3.available()) {fu++;}
				c = Serial3.read();
				static_serial_buffer[buf_ptr++] = c;
			}
			finished = true;
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
	else if (finished && is_midi)
	{
		display_bytes(dbg_sys-1,"aSystem recv serial midi: ",(uint8_t*)static_serial_buffer,4);
		// theSystem.getCurRig()->onSerialMidiEvent(static_serial_buffer[2],static_serial_buffer[3]);
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
			my_error("aSystem got unexpected serial data from_serial3(%d) is_midi(%d) buf_ptr(%d) %s",
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
// battery indicator

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


void aSystem::loop()
	//  called from Arduino loop()
{
	// initQueryFTP();
		// query the FTP battery level on a timer

#if 0

	aWindow *win = m_num_modals ?
		getTopModalWindow() :
		getCurRig();

	//----------------------------------
	// PEDALS
	//----------------------------------
	// draw the pedal frame and titles if needed

	if (win->m_flags & WIN_FLAG_SHOW_PEDALS)
#endif	// 0

	{
		bool draw_pedal_values = false;
		int pedal_width = pedal_rect.width() / NUM_PEDALS;

		if (draw_pedals)
		{
			draw_pedals = false;
			draw_pedal_values = true;

			mylcd.Fill_Rect(
				pedal_rect.xs,
				pedal_rect.ys,
				pedal_rect.width(),
				PEDAL_TEXT_AREA_HEIGHT,
				TFT_YELLOW);

			mylcd.setFont(Arial_18_Bold);   // Arial_16);
			mylcd.Set_Text_colour(0);
			mylcd.Set_Draw_color(TFT_YELLOW);

			for (int i=0; i<NUM_PEDALS; i++)
			{
				mylcd.printf_justified(
					i*pedal_width,
					pedal_rect.ys + 5,
					pedal_width,
					PEDAL_TEXT_AREA_HEIGHT,
					LCD_JUST_CENTER,
					TFT_BLACK,
					TFT_YELLOW,
					false,
					"%s",
					prefs.PEDAL[i].NAME);

				if (i && i<NUM_PEDALS)
					mylcd.Draw_Line(
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

				mylcd.setFont(Arial_40_Bold);   // Arial_40);
				mylcd.Set_Text_colour(TFT_WHITE);

				mylcd.printf_justified(
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

	bool draw_title_frame = draw_title;

	if (draw_title)
	{
		draw_title = false;
		fillRect(title_rect,TFT_BLACK);

		// title text

		draw_title = false;
        mylcd.setFont(Arial_16_Bold);
        mylcd.Set_Text_Cursor(5,10);
        mylcd.Set_Text_colour(TFT_YELLOW);
        mylcd.print(m_title);

		// horizontal line

        mylcd.Set_Draw_color(TFT_YELLOW);
	    mylcd.Draw_Line(0,36,mylcd.Get_Display_Width()-1,36);

		// midi indicator frames

        mylcd.Set_Draw_color(TFT_WHITE);
		for (int i=0; i<NUM_MIDI_PORTS; i++)
		{
			mylcd.Fill_Circle(
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

		mylcd.Fill_Rect(
			BATTERY_X,
			BATTERY_Y,
			BATTERY_WIDTH,
			BATTERY_HEIGHT,
			battery_frame_color);
		mylcd.Fill_Rect(
			BATTERY_X + BATTERY_WIDTH -1,
			BATTERY_Y + (BATTERY_HEIGHT/4),
			4,
			(BATTERY_HEIGHT/2),
			battery_frame_color);
		mylcd.Fill_Rect(
			BATTERY_X + BATTERY_FRAME,
			BATTERY_Y + BATTERY_FRAME,
			BATTERY_WIDTH - 2*BATTERY_FRAME,
			BATTERY_HEIGHT - 2*BATTERY_FRAME,
			TFT_BLACK);

		float pct = ftp_battery_level == -1 ? 1.0 : (((float)ftp_battery_level)-0x40) / 0x24;
		int color = ftp_battery_level == -1 ? TFT_LIGHTGREY : (pct <= .15 ? TFT_RED : TFT_DARKGREEN);
		if (pct > 1) pct = 1.0;

		// display(0,"battery_level=%d   pct=%0.2f",ftp_battery_level,pct);

		float left_width = pct * ((float) BATTERY_WIDTH - 2*BATTERY_FRAME);
		float right_width = (1-pct) * ((float) BATTERY_WIDTH - 2*BATTERY_FRAME);
		int left_int = left_width;
		int right_int = right_width;

		mylcd.Fill_Rect(
			BATTERY_X + BATTERY_FRAME,
			BATTERY_Y + BATTERY_FRAME,
			left_int,
			BATTERY_HEIGHT - 2*BATTERY_FRAME,
			color);

		if (right_int > 0)
			mylcd.Fill_Rect(
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

	unsigned now = millis();
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

				mylcd.Set_Draw_color(color);
				mylcd.Fill_Circle(
					INDICATOR_X + cable_pair*INDICATOR_PAIR_SPACING + out*INDICATOR_SPACING,
					INDICATOR_Y,
					INDICATOR_RADIUS);
			}
		}
	}


	// call the current window's updateUI method
#if 0
	win->updateUI();
#endif // 0

}	// aSystem::loop()



#if !MIDI_ACTIVITY_INLINE
	void aSystem::midiActivity(int port_num)
	{
		m_midi_activity[port_num] = millis();
		display(dbg_sys,"midiActivity(%d)",port_num);
	}
#endif
