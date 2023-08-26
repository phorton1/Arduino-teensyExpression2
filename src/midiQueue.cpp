#include "myDebug.h"
#include "midiQueue.h"
#include "prefs.h"
#include "ftp.h"
#include "ftp_defs.h"
#include "midiHost.h"
// #include "theSystem.h"


#define dbg_midi_send  1
#define dbg_ftp		   -1
	// 0 = show set values
	// =1 = show sendFTPCommandAndValue()
#define dbg_queue	   1



#define MAX_QUEUE       8192
#define MAX_SYSEX       1024


#define PORT_HOST_MASK  0x40

#define NUM_REAL_PORTS  6
    // only real input ports can process sysex messages into buffers
    // or be monitored for FTP stuff

#define GET_COMMAND_VALUE(w)    ((w)>>24)
    // outgoing pending commands are full 32bit midi messages
    // so we use this to compare them to the incomming_command,
    // which is just a uint8_t

uint8_t last_command[NUM_REAL_PORTS]  = {0,0,0,0,0,0};
    // as we are processing messages, we keep track of the most recent
    // B7 1F "command or reply" (i.e. 07==FTP_CMD_BATTERY_LEVEL),
    // to be able to hook it up to the following "command_or_reply" value
    // message (B7 1F "value") for processing and display purposes.

uint8_t pending_command        = 0;
uint8_t pending_command_value  = 0;


//int command_retry_count         = 0;
// elapsedMillis command_time      = 0;
    // These four variables are used to implement an asynychronous
    // command and reply conversation with the FTP.  When we send
    // a command (and value), we save them here, and in processing
    // (if and) when the FTP replies with the correct values, we
    // clear them, which allows for the next command to be sent.


uint8_t most_recent_note_val = 0;
uint8_t most_recent_note_vel = 0;
    // these values are cached from the most recent NoteOn/NoteOff
    // messages and used to create (or delete) my note_t's upon 1E
    // NoteInfo messages.





class msgUnion
{
    public:

        // msgUnion()              { i = 0; }      // default constructor

        msgUnion(uint32_t msg)  { i = msg; }        // from the queue

        msgUnion(uint8_t port, uint32_t msg)
        {
            i = (msg & ~MIDI_PORT_NUM_MASK) | port;
        }

        msgUnion(uint8_t port, uint8_t type, uint8_t channel, uint8_t p1, uint8_t p2)
        {
            b[0] = type | port;
            b[1] = (type<<4) | (channel - 1);
            b[2] = p1;
            b[3] = p2;
        }

		void setPort(uint8_t port)
		{
            i = (i & ~MIDI_PORT_NUM_MASK) | port;
		}

        inline uint8_t port()              { return i & MIDI_PORT_NUM_MASK; }
		inline bool    isUSB()			   { return port() <= MIDI_PORT_USB4; }
        inline bool    isHost()            { return port() >= MIDI_PORT_HOST1 && port() <= MIDI_PORT_HOST2; }
        inline bool    isSerial()          { return port() == MIDI_PORT_SERIAL; }

        inline uint8_t realIndex()     	   { return (i & MIDI_PORT_NUM_MASK) >> 4; }
        inline uint8_t type()              { return i & 0x0f; }
        inline uint8_t channel()           { return (b[1] & 0xf) + 1; }

        inline uint8_t param1()            { return b[2]; }
        inline uint8_t param2()            { return b[3]; }

        bool isActiveSense()        { return (i & 0xff0f) == 0xfe0f; }

    union {
        uint32_t i;
        uint8_t b[4];
    };
};




static void monitor_msg(bool output, msgUnion &msg)
{
}


static void handleFTP(msgUnion &msg)
{
	uint8_t type = msg.type();
	uint8_t pindex = msg.realIndex();
	uint8_t p1 = msg.param1();
	uint8_t p2 = msg.param2();

	if (type == 0x08 || type == 0x09)	// Note OFf or ON
	{
		display(dbg_ftp,"FTP(%d) note val=%-3d vel=%d",pindex,msg.b[2],msg.b[3]);
		most_recent_note_val = msg.b[2];
		most_recent_note_vel = msg.b[3];
	}
	else if (type == 0x0b)				// Control Change
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




static void enqueueMidi(msgUnion &msg)
{
	display(dbg_queue,"enqueueMidi(0x%08x)",msg.i);
	if (prefs.FTP_ENABLE && (
		msg.port() == MIDI_PORT_USB1 ||
		msg.port() == MIDI_PORT_HOST1 ))
	{
		handleFTP(msg);
	}
	// rig_machine.onMidiCC(
	// 		EXP_MIDI_PORT_SERIAL,
	// 		(static_serial_buffer[1] & 0x0f) + 1,
	// 		static_serial_buffer[2],
	// 		static_serial_buffer[3]);
}



// extern
void enqueueMidi(uint8_t port, uint32_t msg32)
	// from theSystem::timer_handler() for usb or
	// midiHost::rx_data() for host
{
	msgUnion msg(port,msg32);
	enqueueMidi(msg);
}

// extern
void enqueueMidi(uint8_t port, const uint8_t *bytes)
	// from theSystem::handleSerialData()
{
	msgUnion msg(port,
		(bytes[0] & 0x0f),
		(bytes[1] & 0x0f) + 1,
		bytes[2],
		bytes[3]);
	enqueueMidi(msg);
}



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

	msg.setPort(port);
	monitor_msg(1, msg);
}



void sendMidiProgramChange(uint8_t port, uint8_t channel, uint8_t prog_num)
{
    sendMidiMessage("programChange", port, 0x0C, channel, prog_num, 0);
}

void sendMidiControlChange(uint8_t port, uint8_t channel, uint8_t cc_num, uint8_t value)
{
    sendMidiMessage("controlChange", port, 0x0B, channel, cc_num, value);
}


bool sendFTPCommandAndValue(uint8_t cmd, uint8_t val, bool wait)
{
    display(dbg_ftp+1,"sendFTPCommandAndValue(%02x,%02x) wait=%d",cmd,val,wait);
    if (!prefs.FTP_ENABLE)
    {
        warning(0,"prefs.FTP_ENABLE is Off in sendFTPCommandAndValue(%02x,%02x)",cmd,val);
        return 0;
    }

	// I send a single command pair to the HOST1 port
	// and get replies from both HOST1 and HOST2.
	// Since HOST1 typically finishes first, that's the
	// one that gets used to set FTP values.

	#define FTP_TIMEOUT    6000

	proc_entry();
	pending_command = cmd;
	pending_command_value = val;
	sendMidiControlChange(MIDI_PORT_HOST1, 8, FTP_COMMAND_OR_REPLY, 	cmd);
	sendMidiControlChange(MIDI_PORT_HOST1, 8, FTP_COMMAND_VALUE, 	val);

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



