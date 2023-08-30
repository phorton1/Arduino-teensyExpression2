//----------------------------------------------
// midiQueue.cpp
//----------------------------------------------
#include "myDebug.h"
#include "midiQueue.h"
#include "prefs.h"
#include "ftp.h"
#include "ftp_defs.h"
#include "midiHost.h"
#include "midiTypes.h"
#include "rigMachine.h"
#include "theSystem.h"


#define dbg_midi_send  0
#define dbg_queue	   0
#define dbg_ftp		   -1
	// 0 = show set values
	// =1 = show sendFTPCommandAndValue()


#define MAX_QUEUE       8192
#define MAX_SYSEX       1024


//---------------------------------------
// vars
//---------------------------------------

static int queue_head = 0;
static int queue_tail = 0;
static uint32_t midi_queue[MAX_QUEUE];


// Stuff for handling FTP ...
// although the command and reply scheme *may* be general

static uint8_t last_command[NUM_MIDI_PORTS * 2]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    // as we are processing messages, we keep track of the most recent
    // B7 1F XX "FTP_COMMAND_OR_REPLY" (XX) that was processed, (i.e.
	// 07 == FTP_CMD_BATTERY_LEVEL), so that when we get the
	// B7 3F YY "FTP_COMMAND_VALUE" message we know what YY is.

static uint8_t pending_command        = 0;
static uint8_t pending_command_value  = 0;
// static int command_retry_count         = 0;
// static elapsedMillis command_time      = 0;
    // These four variables are used to implement an asynychronous command
    // and reply conversation with the FTP with retries and timeouts.
	// When we send a command and value, we save them here, and in processing,
    // (if and) when the FTP replies with the correct command reply and
	// subsequent value, we know the message was received, and so we can
	// clear the pending command and allow another to be sent.

static uint8_t most_recent_note_val = 0;
static uint8_t most_recent_note_vel = 0;
    // these values are cached from the most recent NoteOn/NoteOff
    // messages and used to create (or delete) my note_t's in ftp.cpp
	// upon subsequent NoteInfo messages.


//-------------------------------------
// immediate sends as device (cable0)
//-------------------------------------

static void sendMidiMessage(const char *what, uint8_t port, uint8_t type, uint8_t channel, uint8_t p1, uint8_t p2)
{
    if (channel < 1 || channel > 16)
    {
        my_error("sendMidiMessage(%s) channel(%d) must be between 1 and 16",what,channel);
        return;
    }

	// we first create a msgUnion to actually send to the correct subport of the 3 devices

	uint8_t use_port =
		(port <= MIDI_PORT_USB4) ? port :
		(port <= MIDI_PORT_HOST2) ? port - MIDI_PORT_HOST1 : 0;
	msgUnion msg(use_port, type, channel, p1, p2);

    if (port <= MIDI_PORT_USB4)
	{
	    display(dbg_midi_send,"sendMidiMessageUSB(%s, 0x%02x,  0x%02x,0x%02x,0x%02x,0x%02x) = 0x%08x",what,port,type,channel,p1,p2,msg.i);
		usb_midi_write_packed(msg.i);
		usb_midi_flush_output();
 	}
	else if (port <= MIDI_PORT_HOST2)
	{
	    display(dbg_midi_send,"sendMidiMessageHost(%s, 0x%02x,  0x%02x,0x%02x,0x%02x,0x%02x)",what,port,type,channel,p1,p2,msg.i);
		midi_host.write_packed(msg.i);
	}
	else // port == MIDI_PORT_SERIAL
    {
	    display(dbg_midi_send,"sendMidiMessageSerial(%s, 0x%02x,  0x%02x,0x%02x,0x%02x,0x%02x)",what,port,type,channel,p1,p2,msg.i);
	    Serial3.write(msg.b,4);
	}

	// then we set our port into the msg, and pass it to
	// enqueueMidi as output for monitoring display

	msg.setOutput();
	msg.setPort(port);
	enqueueMidi(msg);
}


void sendMidiProgramChange(uint8_t port, uint8_t channel, uint8_t prog_num)
{
    sendMidiMessage("programChange", port, MIDI_TYPE_PGM_CHG, channel, prog_num, 0);	// 0x0c
}

void sendMidiControlChange(uint8_t port, uint8_t channel, uint8_t cc_num, uint8_t value)
{
    sendMidiMessage("controlChange", port, MIDI_TYPE_CC, channel, cc_num, value);	// 0x0b
}


bool sendFTPCommandAndValue(uint8_t ftp_port, uint8_t cmd, uint8_t val, bool wait)
{
    display(dbg_ftp+1,"sendFTPCommandAndValue(0x%02x, %02x,%02x) wait=%d",ftp_port,cmd,val,wait);

	// I send a single command pair to the HOST1 port
	// and get replies from both HOST1 and HOST2.
	// Since HOST1 typically finishes first, that's the
	// one that gets used to set FTP values.

	#define FTP_TIMEOUT    6000

	proc_entry();
	pending_command = cmd;
	pending_command_value = val;
	sendMidiControlChange(ftp_port, 8, FTP_COMMAND_OR_REPLY, cmd);
	sendMidiControlChange(ftp_port, 8, FTP_COMMAND_VALUE, 	val);

	bool ok = 1;
	if (wait)
	{
		uint32_t start = millis();
		while (pending_command && millis() - start < FTP_TIMEOUT)
		{
			delay(1);
		}
		if (pending_command)
		{
			ok = 0;
			my_error("FTP TIMEOUT waiting for pending_command(0x%04x)",pending_command);
		}
	}
	proc_leave();
	return ok;
}


//----------------------------------------------------------------
// Enqueuing
//----------------------------------------------------------------

void enqueueMidi(msgUnion &msg)
{
	display(dbg_queue + 1,"_enqueueMidi(0x%08x)",msg.i);

	// Set the activity bit for the message

	the_system.midiActivity(msg.activityIndex());

	// Rig machine uses a port mask quick return if the port doesn't match

	if (msg.type() == MIDI_TYPE_CC)
	{
		rig_machine.onMidiCC(msg);
	}

	// determine whether to enqueue it for processing

	bool enqueue_it = false;

	uint8_t ftp_port = FTP_ACTIVE_PORT;
	if (ftp_port && msg.port() == ftp_port)
	{
		enqueue_it = 1;
	}
	else if (prefs.MIDI_MONITOR)
	{
		int p = msg.portEnum();
		enqueue_it = msg.isOutput() ?
			prefs.MONITOR_OUTPUT[p] :
			prefs.MONITOR_INPUT[p];
		enqueue_it = enqueue_it &&
			prefs.MONITOR_CHANNEL[msg.channel()-1];
	}

	if (enqueue_it)
	{
		display(dbg_queue,"_enqueueMidi(0x%08x)",msg.i);
		__disable_irq();
		midi_queue[queue_head++] = msg.i;
		if (queue_head == MAX_QUEUE)
			queue_head = 0;
		if (queue_head == queue_tail)
			my_error("enqueueProcess() overflow at %d",queue_head);
		__enable_irq();
	}
}


void enqueueMidi(bool output, uint8_t port, uint32_t msg32)
	// from theSystem::timer_handler() for usb or
	// midiHost::rx_data() for host
{
	msgUnion msg(port,msg32);
	if (output) msg.setOutput();
	enqueueMidi(msg);
}

// extern
void enqueueMidi(bool output, uint8_t port, const uint8_t *bytes)
	// from theSystem::handleSerialData()
{
	msgUnion msg(port,
		(bytes[0] & 0x0f),
		(bytes[1] & 0x0f) + 1,
		bytes[2],
		bytes[3]);
	if (output) msg.setOutput();
	enqueueMidi(msg);
}




//-------------------------------------------------------------
// deQueue messages and Handle FTP
//-------------------------------------------------------------


static void handleFTP(msgUnion &msg)
{
	uint8_t type = msg.type();
	uint8_t pindex = msg.portEnum();
	uint8_t p1 = msg.param1();
	uint8_t p2 = msg.param2();

	if (type == MIDI_TYPE_NOTE_OFF || type == MIDI_TYPE_NOTE_ON)	// 0x08 or 0x09
	{
		display(dbg_ftp,"FTP(%d) note val=%-3d vel=%d",pindex,msg.b[2],msg.b[3]);
		most_recent_note_val = msg.b[2];
		most_recent_note_vel = msg.b[3];
	}
	else if (type == MIDI_TYPE_CC)				// 0x0b = Control Change
    {
		if (p1 == FTP_NOTE_INFO)    // 0x1e
		{
			uint8_t string = p2>>4;
			uint8_t vel = p2 & 0x0f;
			if (most_recent_note_vel)
			{
				display(dbg_ftp,"FTP(%d) addNote(%d,%d,%d,%d)",pindex,most_recent_note_val,most_recent_note_vel,string,vel);
				addNote(most_recent_note_val,most_recent_note_vel,string,vel);
			}
			else
			{
				display(dbg_ftp,"FTP(%d) deleteNote(%d)",pindex,string);
				deleteNote(string);
			}
			most_recent_note_vel = 0;
			most_recent_note_val = 0;
		}
		else if (p1 == FTP_SET_TUNING || p1 == FTP_TUNING)  // 0x1d || 0x3d
		{
			if (p1 == FTP_SET_TUNING)   // 0x1D
			{
				display(dbg_ftp,"FTP(%d) tuning_note1 = most_recent_note",pindex);
				tuning_note = most_recent_note;
			}
			else if (!tuning_note)
			{
				display(dbg_ftp,"FTP(%d) tuning_note2 = most_recent_note",pindex);
				tuning_note = most_recent_note;
			}

			// 0x00 = -40,  0x40 == 0, 0x80 == +40
			int tuning = ((int) p2) - 0x40;      // 40 == 0,  0==-
			if (tuning_note)
			{
				display(dbg_ftp,"FTP(%d) tuning_note->tuning = %d",pindex,tuning);
				tuning_note->tuning = tuning;
			}
		}
		else if (p1 == FTP_COMMAND_OR_REPLY)
		{
			last_command[pindex] = p2;
		}
		else if  (p1 == FTP_COMMAND_VALUE)
		{
			uint8_t command = last_command[pindex];
			last_command[pindex] = 0;

			if (command == FTP_CMD_POLY_MODE)
			{
				display(dbg_ftp,"FTP(%d) poly_mode = 0x%02x",pindex,p2);
				ftp_poly_mode = p2;
			}
			else if (command == FTP_CMD_PITCHBEND_MODE)
			{
				display(dbg_ftp,"FTP(%d) bend_mode = 0x%02x",pindex,p2);
				ftp_bend_mode = p2;
			}
			else if (command == FTP_CMD_BATTERY_LEVEL) // we can parse this one because it doesn't require extra knowledge
			{
				display(dbg_ftp,"FTP(%d) battery_level = 0x%02x",pindex,p2);
				ftp_battery_level = p2;
			}
			else if (command == FTP_CMD_GET_SENSITIVITY)
			{
				if (pending_command == FTP_CMD_GET_SENSITIVITY)
				{
					display(dbg_ftp,"FTP(%d) sensitivity[%d] = 0x%02x",pindex,pending_command_value,p2);
					ftp_sensitivity[pending_command_value] = p2;
				}
			}
			else if (command == FTP_CMD_SET_SENSITIVITY)  // we can parse this one because it doesn't require extra knowledge
			{
				int string = p2 >> 4;
				int level  = p2 & 0xf;
				display(dbg_ftp,"FTP(%d) sensitivity2[%d] = 0x%02x",pindex,string,level);
				ftp_sensitivity[string] = level;
			}
			else if (command == FTP_CMD_DYNAMICS_SENSITIVITY)
			{
				display(dbg_ftp,"FTP(%d) dynamic_range = 0x%02x",pindex,p2);
				ftp_dynamic_range = p2;
			}
			else if (command == FTP_CMD_DYNAMICS_OFFSET)
			{
				display(dbg_ftp,"FTP(%d) dynamic_offset = 0x%02x",pindex,p2);
				ftp_dynamic_offset = p2;
			}
			else if (command == FTP_CMD_TOUCH_SENSITIVITY)
			{
				display(dbg_ftp,"FTP(%d) touch_sensitivity = 0x%02x",pindex,p2);
				ftp_touch_sensitivity = p2;
			}

			// now that we have the 2nd 3F message, if we matched the 1F message,
			// clear the pending outgoing command

			if (command == pending_command)
			{
				display(dbg_ftp + 1,"FTP(%d) pending_command(0x%02x)=0",pindex,pending_command);
				pending_command = 0;
			}

		}	// p1 == FTP_COMMAND_VALUE
	}	// type == 0x0b   Control Change
}	// handleFTP()



static void _processMessage(uint32_t msg32)
{
	msgUnion msg(msg32);

	display(dbg_queue,"processMessage(0x%08x)",msg32);

	uint8_t ftp_port = FTP_ACTIVE_PORT;
	if (ftp_port && msg.port() == ftp_port)
	{
		handleFTP(msg);
	}
}


void dequeueMidi()
{
    uint32_t msg32 = 0;
    while (queue_tail != queue_head)
    {
        msg32 = midi_queue[queue_tail++];
        if (queue_tail == MAX_QUEUE)
            queue_tail = 0;
		if (msg32)
			_processMessage(msg32);
	}
}


