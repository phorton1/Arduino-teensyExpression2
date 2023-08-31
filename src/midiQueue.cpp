//----------------------------------------------
// midiQueue.cpp
//----------------------------------------------
#include "myDebug.h"
#include "midiQueue.h"
#include "prefs.h"
#include "ftp.h"
#include "ftp_defs.h"
#include "midiHost.h"
#include "rigMachine.h"
#include "theSystem.h"


#define dbg_midi_send  0
#define dbg_queue	   1
#define dbg_ftp		   0
	// 0 = show set values
	// =1 = show sendFTPCommandAndValue()
#define dbg_ftp_notes  1
	// a separate define for just showing FTP note stuff

#define MAX_QUEUE       	8192
#define MAX_OUTGOING_QUEUE  1024
#define FTP_RETRY_TIME		100
#define FTP_RETRY_COUNT		10


#define MAX_SYSEX       1024


//---------------------------------------
// vars
//---------------------------------------
// the midi queue

static int queue_head;
static int queue_tail;
static uint32_t midi_queue[MAX_QUEUE];

// For generally handling two part COMMAND and REPLY sequences

static uint8_t last_command[NUM_MIDI_PORTS * 2];

// For handling asynchrounous COMMAND/REPLY conversation with the FTP


static uint8_t  pending_command;
static uint8_t  pending_command_value;
static int 		command_retry_count;
static uint32_t command_time;

static int 		outgoing_head;
static int 		outgoing_tail;
static uint16_t outgoing_queue[MAX_OUTGOING_QUEUE];

// For handling FTP notes

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
	// use_port >>= 4;
	msgUnion msg(use_port, type, channel, p1, p2);

    if (port <= MIDI_PORT_USB4)
	{
	    display_level(dbg_midi_send,1,"sendMidiMessageUSB(%s, 0x%02x,  0x%02x,0x%02x,0x%02x,0x%02x) = 0x%08x use_port=0x%02x",what,port,type,channel,p1,p2,msg.i,use_port);
		usb_midi_write_packed(msg.i);
		usb_midi_flush_output();
 	}
	else if (port <= MIDI_PORT_HOST2)
	{
	    display_level(dbg_midi_send,1,"sendMidiMessageHost(%s, 0x%02x,  0x%02x,0x%02x,0x%02x,0x%02x) = 0x%08x use_port=0x%02x",what,port,type,channel,p1,p2,msg.i,use_port);
		midi_host.write_packed(msg.i);
	}
	else // port == MIDI_PORT_SERIAL
    {
	    display_level(dbg_midi_send,1,"sendMidiMessageSerial(%s, 0x%02x,  0x%02x,0x%02x,0x%02x,0x%02x) = 0x%08x use_port=0x%02x",what,port,type,channel,p1,p2,msg.i,use_port);
	    SERIAL_DEVICE.write(msg.b,4);
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




//-------------------------------------
// outgoing Message Processing
//-------------------------------------

void sendFTPCommandAndValue(uint8_t ftp_port, uint8_t cmd, uint8_t val)
	// merely enquees the 16 bit command and value
{
    display_level(dbg_ftp+1,0,"sendFTPCommandAndValue(0x%02x, 0x%02x,0x%02x)",ftp_port,cmd,val);
	uint16_t cmd_and_val = (cmd << 8) | val;

    // __disable_irq();
    outgoing_queue[outgoing_head++] = cmd_and_val;
    if (outgoing_head == MAX_OUTGOING_QUEUE)
        outgoing_head = 0;
    if (outgoing_head == outgoing_tail)
        my_error("outgoing_queue overflow at %d",outgoing_head);
    // __enable_irq();
}


static uint16_t _dequeueOutgoing()
{
    int cmd_and_val = 0;
    if (outgoing_tail != outgoing_head)
    {
        cmd_and_val = outgoing_queue[outgoing_tail++];
        if (outgoing_tail == MAX_OUTGOING_QUEUE)
            outgoing_tail = 0;
    }
    return cmd_and_val;
}


static void _sendPendingCommand()
	// only called if the global pending_command is set
{
	// shouldn't really get here without an active
	// FTP port, but they *might* change the pref
	// after the fire-and-forget

    int ftp_port = FTP_ACTIVE_PORT;
    if (ftp_port)
    {
		sendMidiControlChange(ftp_port,FTP_CONTROL_CHANNEL, FTP_COMMAND_OR_REPLY, pending_command);
		sendMidiControlChange(ftp_port,FTP_CONTROL_CHANNEL, FTP_COMMAND_VALUE,    pending_command_value);
	}
    command_time = millis();
}


void _processOutgoing()
{
    // see if there's a command to dequue and send
    // dequeue them even if we don't send them

    if (!pending_command)
    {
		uint16_t cmd_and_val = _dequeueOutgoing();
        if (cmd_and_val)
        {
			pending_command = cmd_and_val >> 8;
            pending_command_value = cmd_and_val & 0xff;
            display_level(dbg_ftp+1,0,"FTP send cmd(0x%02x) val(0x%02x)",pending_command,pending_command_value);
            command_retry_count = 0;
            _sendPendingCommand();
        }
    }
    else if (command_retry_count > FTP_RETRY_COUNT)
    {
        my_error("FTP timed out sending cmd(0x%02x) val(0x%02x)",pending_command,pending_command_value);
        command_retry_count = 0;
        pending_command = 0;
    }
    else if (millis() - command_time > FTP_RETRY_TIME)
    {
        command_retry_count++;
        display_level(dbg_ftp+1,0,"FTP resend(%d) cmd(0x%02x) val(0x%02x)",command_retry_count,pending_command,pending_command_value);
        _sendPendingCommand();
    }
}



//----------------------------------------------------------------
// Enqueuing
//----------------------------------------------------------------

void enqueueMidi(msgUnion &msg)
{
	display_level(dbg_queue+1,3,"_enqueueMidi(0x%08x)",msg.i);

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
		if (dbg_queue <= 0)
			display_level(dbg_queue,4,"_enqueueMidi(0x%08x) out(%d) port(0x%02x) channel(%-2d) type(0x%02x) param1(0x%02x) param2(0x%02x)",
				msg.i,
				msg.isOutput(),
				msg.portEnum(),
				msg.channel(),
				msg.type(),
				msg.param1(),
				msg.param2());

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


static void _handleFTP(msgUnion &msg)
{
	uint8_t type = msg.type();
	uint8_t pindex = msg.portEnum();
	uint8_t p1 = msg.param1();
	uint8_t p2 = msg.param2();

	if (dbg_ftp <= 0)
		display_level(dbg_queue,4,"_handleFTP  (0x%08x) out(%d) port(0x%02x) channel(%-2d) type(0x%02x) param1(0x%02x) param2(0x%02x)",
			msg.i,
			msg.isOutput(),
			pindex,
			msg.channel(),
			type,
			p1,
			p2);

	if (type == MIDI_TYPE_NOTE_OFF || type == MIDI_TYPE_NOTE_ON)	// 0x08 or 0x09
	{
		display_level(dbg_ftp_notes,2,"FTP(%d) note val=%-3d vel=%d",pindex,msg.b[2],msg.b[3]);
		most_recent_note_val = msg.b[2];
		most_recent_note_vel = msg.b[3];
	}
	else if (type == MIDI_TYPE_CC)				// 0x0b = Control Change
    {
		if (p1 == FTP_NOTE_INFO)    // 0x1e
		{
			// note that we keep calling addNote and deleteNote
			// even when we are not in tuner mode, which theoretically
			// might slow host mode down a bit ... we *could* make the
			// add a midiQueue::setFTPTunerMode(bool) to only add and
			// delete notes when we are actually in the tuner.

			uint8_t string = p2>>4;
			uint8_t vel = p2 & 0x0f;
			if (most_recent_note_vel)
			{
				display_level(dbg_ftp_notes,2,"FTP(%d) addNote(%d,%d,%d,%d)",pindex,most_recent_note_val,most_recent_note_vel,string,vel);
				addNote(most_recent_note_val,most_recent_note_vel,string,vel);
			}
			else
			{
				display_level(dbg_ftp_notes,2,"FTP(%d) deleteNote(%d)",pindex,string);
				deleteNote(string);
			}
			most_recent_note_vel = 0;
			most_recent_note_val = 0;
		}
		else if (p1 == FTP_SET_TUNING || p1 == FTP_TUNING)  // 0x1d || 0x3d
		{
			if (p1 == FTP_SET_TUNING)   // 0x1D
			{
				display_level(dbg_ftp_notes,2,"FTP(%d) tuning_note1 = ftp_cur_note",pindex);
				ftp_tuning_note = ftp_cur_note;
			}
			else if (!ftp_tuning_note)
			{
				display_level(dbg_ftp_notes,2,"FTP(%d) tuning_note2 = ftp_cur_note",pindex);
				ftp_tuning_note = ftp_cur_note;
			}

			// 0x00 = -40,  0x40 == 0, 0x80 == +40
			int tuning = ((int) p2) - 0x40;      // 40 == 0,  0==-
			if (ftp_tuning_note)
			{
				display_level(dbg_ftp_notes,2,"FTP(%d) ftp_tuning_note->tuning = %d",pindex,tuning);
				ftp_tuning_note->tuning = tuning;
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
				display_level(dbg_ftp,2,"FTP(%d) poly_mode = 0x%02x",pindex,p2);
				ftp_poly_mode = p2;
			}
			else if (command == FTP_CMD_PITCHBEND_MODE)
			{
				display_level(dbg_ftp,2,"FTP(%d) bend_mode = 0x%02x",pindex,p2);
				ftp_bend_mode = p2;
			}
			else if (command == FTP_CMD_BATTERY_LEVEL) // we can parse this one because it doesn't require extra knowledge
			{
				display_level(dbg_ftp,2,"FTP(%d) battery_level = 0x%02x",pindex,p2);
				ftp_battery_level = p2;
			}
			else if (command == FTP_CMD_GET_SENSITIVITY)
			{
				// we double check that this is a response to our
				// specific pending command !?!
				if (pending_command == FTP_CMD_GET_SENSITIVITY)
				{
					display_level(dbg_ftp,2,"FTP(%d) sensitivity[%d] = 0x%02x",pindex,pending_command_value,p2);
					ftp_sensitivity[pending_command_value] = p2;
				}
			}
			else if (command == FTP_CMD_SET_SENSITIVITY)  // we can parse this one because it doesn't require extra knowledge
			{
				int string = p2 >> 4;
				int level  = p2 & 0xf;
				display_level(dbg_ftp,2,"FTP(%d) sensitivity2[%d] = 0x%02x",pindex,string,level);
				ftp_sensitivity[string] = level;
			}
			else if (command == FTP_CMD_DYNAMICS_SENSITIVITY)
			{
				display_level(dbg_ftp,2,"FTP(%d) dynamic_range = 0x%02x",pindex,p2);
				ftp_dynamic_range = p2;
			}
			else if (command == FTP_CMD_DYNAMICS_OFFSET)
			{
				display_level(dbg_ftp,2,"FTP(%d) dynamic_offset = 0x%02x",pindex,p2);
				ftp_dynamic_offset = p2;
			}
			else if (command == FTP_CMD_TOUCH_SENSITIVITY)
			{
				display_level(dbg_ftp,2,"FTP(%d) touch_sensitivity = 0x%02x",pindex,p2);
				ftp_touch_sensitivity = p2;
			}

			// now that we have the 2nd 3F message, if we matched the 1F message,
			// clear the pending outgoing command

			if (command == pending_command)
			{
				display_level(dbg_ftp+1,1,"FTP(%d) clearing pending_command(0x%02x)",pindex,pending_command);
				pending_command = 0;
			}

		}	// p1 == FTP_COMMAND_VALUE
	}	// type == 0x0b   Control Change
}	// handleFTP()



static void _processMsg(uint32_t msg32)
{
	msgUnion msg(msg32);

	if (dbg_queue <= 0)
		display_level(dbg_queue,4,"_processMsg (0x%08x) out(%d) port(0x%02x) channel(%-2d) type(0x%02x) param1(0x%02x) param2(0x%02x)",
			msg.i,
			msg.isOutput(),
			msg.portEnum(),
			msg.channel(),
			msg.type(),
			msg.param1(),
			msg.param2());

	// I believe I cannot limit the checks to channel 8
	// due to the fact that I want notes ...

	uint8_t ftp_port = FTP_ACTIVE_PORT;
	if (ftp_port &&
		msg.port() == ftp_port &&
		!msg.isOutput())
	{
		_handleFTP(msg);
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
			_processMsg(msg32);
	}
	_processOutgoing();
}


