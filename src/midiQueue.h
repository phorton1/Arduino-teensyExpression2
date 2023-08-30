//----------------------------------------------
// midiQueue.h
//----------------------------------------------
#pragma once

#include "defines.h"

#define FTP_ACTIVE_PORT (   \
    prefs.SPOOF_FTP ? MIDI_PORT_HOST1 : \
    prefs.FTP_PORT == 1 ? MIDI_PORT_USB1 : \
    prefs.FTP_PORT == 2 ? MIDI_PORT_HOST1 : 0 )

#define MIDI_OUTPUT_B2_FLAG  0x80
    // set into the high order bit of the B2 byte

class msgUnion
    // USB raw input ports map to mine, but HOST0,1
    // are 4,5 on mine and SERIAL0 is 6,  The OUTPUT
    // state is weirdly stored in the high order bit
    // of the 3 byte (the first parameter).
    //
{
    public:

		msgUnion(uint32_t msg32)  { i = msg32; }

        msgUnion(uint8_t port, uint32_t msg32)
        {
            i = (msg32 & ~MIDI_PORT_NUM_MASK) | port;
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
        void setOutput()
        {
            b[2] |= MIDI_OUTPUT_B2_FLAG;
        }

        inline uint8_t port()		const { return i & MIDI_PORT_NUM_MASK; }
		inline bool    isUSB()		const { return port() <= MIDI_PORT_USB4; }
        inline bool    isHost()		const { return port() >= MIDI_PORT_HOST1 && port() <= MIDI_PORT_HOST2; }
        inline bool    isSerial()	const { return port() == MIDI_PORT_SERIAL; }
        inline bool    isOutput()	const { return b[2] & MIDI_OUTPUT_B2_FLAG; }

        inline uint8_t portEnum()	const { return (i & MIDI_PORT_NUM_MASK) >> 4; }

        inline uint8_t type()   	const { return i & 0x0f; }
        inline uint8_t channel()	const { return (b[1] & 0xf) + 1; }
        inline uint8_t param1() 	const { return b[2] & ~MIDI_OUTPUT_B2_FLAG; }
        inline uint8_t param2() 	const { return b[3]; }

        bool isActiveSense()		const { return (i & 0xff0f) == 0xfe0f; }

		inline uint8_t activityIndex() const
		{
			int p = port();
			bool o = isOutput();
			return p <= MIDI_PORT_USB3 ?
					o ? ACTIVITY_INDICATOR_USB_OUT :
						ACTIVITY_INDICATOR_USB_IN :
				p <= MIDI_PORT_HOST2 ?
					o ? ACTIVITY_INDICATOR_HOST_OUT :
						ACTIVITY_INDICATOR_HOST_IN :
				o ? ACTIVITY_INDICATOR_SERIAL_OUT :
					ACTIVITY_INDICATOR_SERIAL_IN;
		}

	// data

    union {
        uint32_t i;
        uint8_t b[4];
    };
};


extern void enqueueMidi(msgUnion &msg);
extern void enqueueMidi(bool output, uint8_t port, uint32_t msg32);
extern void enqueueMidi(bool output, uint8_t port, const uint8_t *bytes);
	// QUICKLY enqueue Messages.  If FTP is enabled, certain messags will always
	// be enqueued, even if they are not monitored.  Otherwise, only messages
	// on ports and channels being monitored will be enqueued. This is
	// intended to be called from time-critical code.

extern void dequeueMidi();
    // dequeue Messages on UI thread.  if FTP is enabled, certain messagss
	// will be checked for functional reasons and then possibly re-filtered
	// for port/channel before monitoring.  Then any messages left will be
	// filtered according to monitoring preferences for display, building
	// sysex buffers, etc.


extern void sendMidiSysex(uint8_t port, int length, const uint8_t *buf);
extern void sendMidiProgramChange(uint8_t port, uint8_t channel, uint8_t prog_num);
extern void sendMidiControlChange(uint8_t port, uint8_t channel, uint8_t cc_num, uint8_t value);

extern void sendFTPCommandAndValue(uint8_t ftp_port, uint8_t cmd, uint8_t val);


// end of midiQueue.h