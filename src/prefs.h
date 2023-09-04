//------------------------------------------
// prefs.h
//------------------------------------------
// structure that describes all prefs that
// are stored in EEPROM.
//
// Note that "setting" or "writing" a pref does
// NOT write it to EEPROM. It will be written
// to the in-memory cache and the prefs will
// then appear as dirty.
//
// Upon a call to save_prefs() the prefs will
// be written to EEPROM and marked as clean.
//
// Calling read_prefs() on a dirty set of prefs
// will cause them to be re-read from EEPROM and
// be made clean again.
//
// Finally, there are methods to check individual
// prefs for dirty, and to restore them, for use
// in things like cancelling a pedalConfig window.

#pragma once

#include "defines.h"

// PRH - should implement debug port following (off, DEBUG, USB, SERIAL)
//	   for FILE_SYS_DEVICE and MIDI_MONITOR

#define TEENSY_EXPRESSION2_PREF_VERSION   239
	// EEPROM location 0 is magic
	// it is 237 for teensyExpression1
	// and 238 for teensyExpression2
	// so that we can recognize the difference
	// and automatically do a factory reset when switching between them


// Enumeration of DEBUG_DEVICEs

#define DEBUG_DEVICE_OFF		0
#define DEBUG_DEVICE_USB		1
#define DEBUG_DEVICE_SERIAL		2

// Enumeration of OUTPUT_DEVICES that can follow the DEBUG_DEVICE

#define OUTPUT_DEVICE_OFF	    0
#define OUTPUT_DEVICE_DEBUG		1		// follows the DEBUG_DEVICE
#define OUTPUT_DEVICE_USB		2
#define OUTPUT_DEVICE_SERIAL	3

// Encapsulation of FILE_SYS_DEVICE and MONITOR_DEVICE

#define ACTIVE_FILE_SYS_DEVICE		( \
	prefs.FILE_SYS_DEVICE == OUTPUT_DEVICE_DEBUG  ? ((Stream *) dbgSerial) : \
	prefs.FILE_SYS_DEVICE == OUTPUT_DEVICE_USB    ? ((Stream *) &Serial) : \
	prefs.FILE_SYS_DEVICE == OUTPUT_DEVICE_SERIAL ? ((Stream *) &SERIAL_DEVICE) : NULL )

#define ACTIVE_MONTITOR_DEVICE		( \
	prefs.MONOTOR_DEVICE == OUTPUT_DEVICE_DEBUG   ? ((Stream *) dbgSerial) : \
	prefs.MONOTOR_DEVICE == OUTPUT_DEVICE_USB     ? ((Stream *) &Serial) : \
	prefs.MONOTOR_DEVICE == OUTPUT_DEVICE_SERIAL  ? ((Stream *) &SERIAL_DEVICE) : NULL )


//---------------------------------------
// Prefs Declaration
//---------------------------------------

#define NUM_PEDAL_CURVES		3		// number of curves per pedal
#define NUM_CURVE_POINTS		4		// number of points per curve


typedef struct
{
	uint8_t X;								// defaults depend on particular curve
	uint8_t Y;
}	pref_curve_point_t;

typedef struct
{
	pref_curve_point_t POINTS[NUM_CURVE_POINTS];
}	pref_curve_t;

typedef struct
{
	uint16_t		CALIB_MIN;				 	// 0..1023 - default(0)
	uint16_t		CALIB_MAX;               	// 0..1023 - default(1023)
	uint8_t     	CURVE_TYPE;              	// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
	pref_curve_t 	CURVE[NUM_PEDAL_CURVES];
}	pref_pedal_t;


//====================================
// the main prefs_t data structure
//====================================

typedef struct
{
	uint8_t 		BRIGHTNESS;					// LED brightness, 1..100 - default=30
	uint8_t			DEBUG_DEVICE;         		// off, USB, Serial - default(2=Serial)
	uint8_t			FILE_SYS_DEVICE;   			// off, Debug, USB, Serial - default(1=Debug)
	uint8_t			SPOOF_FTP;         			// off, on - on implies FTP_PORT == HOST
	uint8_t			FTP_PORT;         			// off, USB, HOST

	char			RIG_NAME[MAX_RIG_NAME + 1];

	pref_pedal_t 	PEDAL[NUM_PEDALS];

	// midi monitoring, general on/off

	uint8_t			MIDI_MONITOR;    			// off, Debug, USB, Serial - default(off)

	// whether to monitor specific ports
	// these are turned into 7 bit masks for matching the ports in enqueMidi

	uint8_t			MONITOR_INPUT[NUM_MIDI_PORTS];
	uint8_t			MONITOR_OUTPUT[NUM_MIDI_PORTS];

	// what channels to monitor on any monitored ports

	uint16_t 		MONITOR_CHANNEL[MIDI_MAX_CHANNEL];		// off, on - default(on) for 16 channels

	// what to monitor on any monitored ports and channels

	uint8_t			MONITOR_SYSEX;          	// off, on, Detail - default(on)
	uint8_t			MONITOR_ACTIVE_SENSE;    	// off, on - default(off)
	uint8_t			MONITOR_NOTE_ON;        	// off, on - default(off)
	uint8_t			MONITOR_NOTE_OFF;       	// off, on - default(off)
	uint8_t			MONITOR_VELOCITY;       	// off, on - default(off)   note: "after touch poly"
	uint8_t			MONITOR_PROGRAM_CHG;    	// off, on - default(on)
	uint8_t			MONITOR_AFTER_TOUCH;     	// off, on - default(off)
	uint8_t			MONITOR_PITCH_BEND;      	// off, on - default(off)
	uint8_t 		MONITOR_CCS;            	// off, on - default(off)
	uint8_t			MONITOR_EVERYTHING_ELSE;	// off, on - default(off)

	// FTP specific monitoring only occurs on the designated FTP input and output port
	// REGARDLESS of whether port/channel is specifically allowed.  Note that
	// identified FTP patches are in Addition to the SYSEX that might
	// be monitoried on that port/channel.

	uint8_t			MONITOR_FTP;                	// off, on - default(off)
	uint8_t			MONITOR_FTP_PATCHES;  			// off, on - default(off)
	uint8_t			MONITOR_FTP_NOTE_INFO;     	    // off, on - default(off)
	uint8_t			MONITOR_FTP_TUNING_MSGS;    	// off, on - default(off)
	uint8_t			MONITOR_FTP_COMMANDS;       	// off, on - default(off)
	uint8_t			MONITOR_FTP_VALUES;         	// off, on - default(off)
	uint8_t			MONITOR_FTP_POLY_MODE;     	    // off, on - default(off)
	uint8_t			MONITOR_FTP_BEND_MODE;      	// off, on - default(off)
	uint8_t			MONITOR_FTP_VOLUME;         	// off, on - default(off)
	uint8_t			MONITOR_FTP_BATTERY;        	// off, on - default(off)
	uint8_t			MONITOR_FTP_SENSITIVITY;    	// off, on - default(off)
	uint8_t			MONITOR_FTP_KNOWN_COMMANDS;     // off, on - default(off)
	uint8_t			MONITOR_FTP_UNKNOWN_COMMANDS;   // off, on - default(off)



}	prefs_t;




//================================================
// API
//================================================

extern prefs_t prefs;
	// global public direct read/write access

// initialize and reset methods

extern void reset_prefs();
	// clears the EEPROM but does not re-read prefs
	// you will typically need to call read_prefs() or reboot
	// after this.
extern bool read_prefs();
	// returns true if prefs were automatically reset
extern void save_prefs();


// Offset based accessors used by config system.
// These methods know the type of the pref and take, or return,
// a uint8_t, uint16_t, or const char * as the uint32_t parameter
// or uint32_t return type. Min and Maxes are returned as uint16_t's.
// There is no range checking in these routines!

#define pcast(m)			((uint32_t) &(m))
#define poff(PREF)			((uint16_t) {pcast(prefs.PREF) - pcast(prefs)})

extern void 	setPref(uint16_t off, uint32_t value, const char *name = 0);
extern uint32_t getPref(uint16_t off, const char *name = 0);
extern uint16_t getPrefMin(uint16_t off);
extern uint16_t getPrefMax(uint16_t off);
extern bool 	prefs_dirty();



// end of prefs.h
