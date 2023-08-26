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

#define TEENSY_EXPRESSION2_PREF_VERSION   238
	// EEPROM location 0 is magic
	// it is 237 for teensyExpression1
	// and 238 for teensyExpression2
	// so that we can recognize the difference
	// and automatically do a factory reset when switching between them


// Pref Constants

#define OUTPUT_DEVICE_OFF			0
#define OUTPUT_DEVICE_USB			1
#define OUTPUT_DEVICE_SERIAL		2

// Pref Names

#define PREF_BRIGHTNESS				"BRIGHTNESS"
#define PREF_DEBUG_DEVICE			"DEBUG_DEVICE"
#define PREF_FILE_SYSTEM_PORT		"FILE_SYSTEM_PORT"
#define PREF_FTP_ENABLE				"FTP_PORT"


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
	char			NAME[MAX_PEDAL_NAME+1];		// 7 character max name
	uint8_t			PORT;						// 0..NUM_MIDI_PORTS-1
	uint8_t			CHANNEL;					// the midi channel to send on
	uint8_t			CC;							// the CC number to use

	uint16_t		CALIB_MIN;				 	// 0..1023 - default(0)
	uint16_t		CALIB_MAX;               	// 0..1023 - default(1023)
	uint8_t     	CURVE_TYPE;              	// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
	pref_curve_t 	CURVE[NUM_PEDAL_CURVES];
}	pref_pedal_t;

typedef struct
{
	uint8_t			PORT;						// 0..NUM_MIDI_PORTS-1
	uint8_t			CHANNEL;					// default(0),
	uint8_t			CC;							// the CC number to use
}	pref_rotary_t;


//====================================
// the main prefs_t data structure
//====================================

typedef struct
{
	uint8_t 		BRIGHTNESS;					// LED brightness, 1..100 - default=30
	uint8_t			DEBUG_DEVICE;         		// off, USB, Serial - default(2=Serial)
	uint8_t			FILE_SYS_DEVICE;   			// off, USB, Serial - default(0=off)
	uint8_t			SPOOF_FTP;         			// off, on - on implies FTP_PORT == HOST
	uint8_t			FTP_PORT;         			// off, USB, HOST

	pref_pedal_t 	PEDAL[NUM_PEDALS];
	pref_rotary_t	ROTARY[NUM_ROTARY];

	// midi monitoring, general on/off

	uint8_t			MIDI_MONITOR;    			// off, USB, Serial - default(off)

	// whether to monitor specific ports

	uint8_t			MONITOR_PORT[NUM_MIDI_PORTS + 2];		// off, on, for 4 midi and 2 serial ports; only INPUT0 is off at this time

	// whether to monitor specific channels

	uint8_t 		MONITOR_CHANNEL[MIDI_MAX_CHANNEL];		// off, on - default(on) for 16 channels

	// what to monitor

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

	// FTP specific monitoring

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
// methods
//================================================

extern prefs_t prefs;
extern const prefs_t prefs_min;
extern const prefs_t prefs_max;

// initialize and reset methods

extern void reset_prefs();
	// does not re-read them

extern bool read_prefs();
	// returns true if prefs were automatically reset

extern void save_prefs();

// offset based accessors
// used by config system

#define pcast(m)			((uint32_t) &(m))
#define poff(PREF)		((uint16_t) {pcast(prefs.PREF) - pcast(prefs)})
// #define STRINGIFY(x) 		#x
// #define TOSTRING(x) 		STRINGIFY(x)


#if 0

	// written but not used so far

	extern void 	writePref8(uint16_t off, uint8_t value, const char *name = 0);
	extern uint8_t 	readPref8(uint16_t off, const char *name = 0);

	extern void 	writePref16(uint16_t off, uint16_t value, const char *name = 0);
	extern uint16_t readPref16(uint16_t off, const char *name = 0);

	extern uint8_t 	readPref8Min(uint16_t off);
	extern uint8_t 	readPref8Max(uint16_t off);
	extern uint16_t readPref16Min(uint16_t off);
	extern uint16_t readPref16Max(uint16_t off);


	// dirty change detection and restore
	// note that to restore all prefs, you merely call read_prefs()

	extern bool 	prefs_dirty();
	extern bool 	pref8_dirty(uint16_t off);
	extern bool 	pref16_dirty(uint16_t off);
	extern uint8_t 	pref8_restore(uint16_t off, const char *name);
	extern uint16_t pref16_restore(uint16_t off, const char *name);

#endif	// 0


// end of prefs.h
