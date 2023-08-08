//--------------------------------------
// prefs.cpp
//--------------------------------------

#include <myDebug.h>
#include "prefs.h"
#include <EEPROM.h>
#include "iPadDefines.h"
#include "commonDefines.h"

#define dbg_prefs	0

#define NUM_EEPROM_USED	(sizeof(prefs_t))

prefs_t last_prefs;
	// a copy of the prefs for change detectioni


//----------------------------------------------------
// IN_MEMORY_PREFS
//----------------------------------------------------
// are statically initialized with the default values

#define PP_MAX			127
#define PP_HALF			(PP_MAX / 2)
#define PP_ONE			(PP_MAX / 3)
#define PP_TWO			(2 * PP_MAX / 3)

prefs_t prefs;


const prefs_t default_prefs =
{
	.BRIGHTNESS			= 30,				// LED brightness, 1..100 - default=30
	.DEBUG_DEVICE		= 2,				// off, USB, Serial - default(2=Serial)
	.FILE_SYS_DEVICE	= 0,				// off, USB, Serial - default(0=off)
	.FTP_ENABLE			= 1,				// off, on - default - default(on)
	.PEDAL = {
		{											// pedal 0 - Synth
			.NAME = {'S','y','n','t','h', 0},
			.IS_SERIAL = 0,							// off, on; default depends on the Pedal
			.MIDI_CHANNEL = SYNTH_VOLUME_CHANNEL,	// the midi channel to send on
			.MIDI_CC = SYNTH_VOLUME_CC,				// the CC number to use

			.CALIB_MIN	= 0,						// 0..1023 - default(0)
			.CALIB_MAX	= 1023,						// 0..1023 - default(1023)
			.CURVE_TYPE	= 0,						// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
			.CURVE = {                      		// setup simple curves for the three different types
			{ .POINTS = { {0,0}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_HALF,PP_HALF}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_ONE,PP_ONE}, {PP_TWO,PP_TWO}, {PP_MAX,PP_MAX}  }, }, },
		},
		{											// pedal 1 = Looper
			.NAME = {'L','o','o','p', 0},
			.IS_SERIAL = 1,							// off, on; default depends on the Pedal
			.MIDI_CHANNEL = 0,						// the midi channel to send on
			.MIDI_CC = LOOP_CONTROL_BASE_CC + RPI_CONTROL_LOOP_VOLUME,	// the CC number to use
			.CALIB_MIN	= 0,						// 0..1023 - default(0)
			.CALIB_MAX	= 1023,						// 0..1023 - default(1023)
			.CURVE_TYPE	= 0,						// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
			.CURVE = {                      		// setup simple curves for the three different types
			{ .POINTS = { {0,0}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_HALF,PP_HALF}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_ONE,PP_ONE}, {PP_TWO,PP_TWO}, {PP_MAX,PP_MAX}  }, }, },
		},
		{											// pedal 2 = WAH
			.NAME = {'W','a','h', 0},
			.IS_SERIAL = 0,							// off, on; default depends on the Pedal
			.MIDI_CHANNEL = GUITAR_EFFECTS_CHANNEL,	// the midi channel to send on
			.MIDI_CC = GUITAR_WAH_CONTROL_CC,		// the CC number to use
			.CALIB_MIN	= 0,						// 0..1023 - default(0)
			.CALIB_MAX	= 1023,						// 0..1023 - default(1023)
			.CURVE_TYPE	= 0,						// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
			.CURVE = {                      		// setup simple curves for the three different types
			{ .POINTS = { {0,0}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_HALF,PP_HALF}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_ONE,PP_ONE}, {PP_TWO,PP_TWO}, {PP_MAX,PP_MAX}  }, }, },
		},
		{											// pedal 3 = Guitar Volume
			.NAME = {'G','u','i','t','a','r', 0},
			.IS_SERIAL = 0,							// off, on; default depends on the Pedal
			.MIDI_CHANNEL = GUITAR_VOLUME_CHANNEL,	// the midi channel to send on
			.MIDI_CC = GUITAR_VOLUME_CC,			// the CC number to use
			.CALIB_MIN	= 0,						// 0..1023 - default(0)
			.CALIB_MAX	= 1023,						// 0..1023 - default(1023)
			.CURVE_TYPE	= 0,						// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
			.CURVE = {                      		// setup simple curves for the three different types
			{ .POINTS = { {0,0}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_HALF,PP_HALF}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_ONE,PP_ONE}, {PP_TWO,PP_TWO}, {PP_MAX,PP_MAX}  }, }, },
        }, },

	.ROTARY = {
			{
				.IS_SERIAL = 1,						// on/on, default(on)
				.MIDI_CHANNEL = 0,					// midi_channel default(0)
				.MIDI_CC = LOOP_CONTROL_BASE_CC + RPI_CONTROL_INPUT_GAIN,
			},
			{
				.IS_SERIAL = 1,						// on/on, default(on)
				.MIDI_CHANNEL = 0,					// midi_channel default(0)
				.MIDI_CC = LOOP_CONTROL_BASE_CC + RPI_CONTROL_THRU_VOLUME,
			},
			{
				.IS_SERIAL = 1,						// on/on, default(on)
				.MIDI_CHANNEL = 0,					// midi_channel default(0)
				.MIDI_CC = LOOP_CONTROL_BASE_CC + RPI_CONTROL_MIX_VOLUME,
			},
			{
				.IS_SERIAL = 1,						// on/on, default(on)
				.MIDI_CHANNEL = 0,					// midi_channel default(0)
				.MIDI_CC = LOOP_CONTROL_BASE_CC + RPI_CONTROL_OUTPUT_GAIN,
			},
		},

	.MIDI_MONITOR					= 0,	// off, USB, Serial - default(off)

	.MONITOR_PORT = 				{0, 1, 1, 1, 1, 1, },	 // off, on, for 4 midi and 2 serial ports; only INPUT0 is off at this time
	.MONITOR_CHANNEL =				{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, },	// 16 channels, default(on)

	.MONITOR_SYSEX					= 1,	// off, on, Detail - default(on)
	.MONITOR_ACTIVE_SENSE			= 0,	// off, on - default(off)
	.MONITOR_NOTE_ON				= 0,	// off, on - default(off)
	.MONITOR_NOTE_OFF				= 0,	// off, on - default(off)
	.MONITOR_VELOCITY				= 0,	// off, on - default(off)   note: "after touch poly"
	.MONITOR_PROGRAM_CHG			= 1,	// off, on - default(on)
	.MONITOR_AFTER_TOUCH			= 0,	// off, on - default(off)
	.MONITOR_PITCH_BEND				= 0,	// off, on - default(off)
	.MONITOR_CCS					= 0,	// off, on - default(off)
	.MONITOR_EVERYTHING_ELSE		= 0,	// off, on - default(off)

	.MONITOR_FTP					= 0,	// off, on - default(off)
	.MONITOR_FTP_PATCHES			= 0,	// off, on - default(off)
	.MONITOR_FTP_NOTE_INFO			= 0,	// off, on - default(off)
	.MONITOR_FTP_TUNING_MSGS		= 0,	// off, on - default(off)
	.MONITOR_FTP_COMMANDS			= 0,	// off, on - default(off)
	.MONITOR_FTP_VALUES				= 0,	// off, on - default(off)
	.MONITOR_FTP_POLY_MODE			= 0,	// off, on - default(off)
	.MONITOR_FTP_BEND_MODE			= 0,	// off, on - default(off)
	.MONITOR_FTP_VOLUME				= 0,	// off, on - default(off)
	.MONITOR_FTP_BATTERY			= 0,	// off, on - default(off)
	.MONITOR_FTP_SENSITIVITY		= 0,	// off, on - default(off)
	.MONITOR_FTP_KNOWN_COMMANDS		= 0,	// off, on - default(off)
	.MONITOR_FTP_UNKNOWN_COMMANDS	= 0,	// off, on - default(off)

};	// in_memory prefs with defaults


//----------------------------------------------------
// PREF MIN & MAX VALUES
//----------------------------------------------------
// only one value has a non-zero minimum

const prefs_t prefs_min = {
	.BRIGHTNESS			= 30,
};


const prefs_t prefs_max =
{
	.BRIGHTNESS			= 100,				// LED brightness, 1..100 - default=30
	.DEBUG_DEVICE		= 2,				// off, USB, Serial - default(2=Serial)
	.FILE_SYS_DEVICE	= 2,				// off, USB, Serial - default(2=Serial)
	.FTP_ENABLE			= 1,				// off, on - default - default(on)
	.PEDAL = {
		{									// pedal 0
			.NAME		= {0},
			.IS_SERIAL 	= 1,				// off, on; default depends on the Pedal
			.MIDI_CHANNEL = 127,			// the midi channel to send on
			.MIDI_CC 	= 127,				// the CC number to use
			.CALIB_MIN	= 1023,				// 0..1023 - default(0)
			.CALIB_MAX	= 1023,				// 0..1023 - default(1023)
			.CURVE_TYPE	= 2,				// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
			.CURVE = {                      // setup simple curves for the three different types
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}  }, }, },
		},
		{									// pedal 1
			.NAME		= {0},
			.IS_SERIAL 	= 1,				// off, on; default depends on the Pedal
			.MIDI_CHANNEL = 127,			// the midi channel to send on
			.MIDI_CC 	= 127,				// the CC number to use
			.CALIB_MIN	= 1023,				// 0..1023 - default(0)
			.CALIB_MAX	= 1023,				// 0..1023 - default(1023)
			.CURVE_TYPE	= 2,				// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
			.CURVE = {                      // setup simple curves for the three different types
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}  }, }, },
		},
		{									// pedal 2
			.NAME		= {0},
			.IS_SERIAL 	= 1,				// off, on; default depends on the Pedal
			.MIDI_CHANNEL = 127,			// the midi channel to send on
			.MIDI_CC 	= 127,				// the CC number to use
			.CALIB_MIN	= 1023,				// 0..1023 - default(0)
			.CALIB_MAX	= 1023,				// 0..1023 - default(1023)
			.CURVE_TYPE	= 2,				// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
			.CURVE = {                      // setup simple curves for the three different types
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}  }, }, },
		},
		{									// pedal 3
			.NAME		= {0},
			.IS_SERIAL 	= 1,				// off, on; default depends on the Pedal
			.MIDI_CHANNEL = 127,			// the midi channel to send on
			.MIDI_CC 	= 127,				// the CC number to use
			.CALIB_MIN	= 1023,				// 0..1023 - default(0)
			.CALIB_MAX	= 1023,				// 0..1023 - default(1023)
			.CURVE_TYPE	= 2,				// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
			.CURVE = {                      // setup simple curves for the three different types
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}  }, }, },
        }, },

	.ROTARY = {
			{
				.IS_SERIAL = 1,						// on/on, default(on)
				.MIDI_CHANNEL = 0,					// midi_channel default(0)
				.MIDI_CC = 1,
			},
			{
				.IS_SERIAL = 1,						// on/on, default(on)
				.MIDI_CHANNEL = 0,					// midi_channel default(0)
				.MIDI_CC = 1,
			},
			{
				.IS_SERIAL = 1,						// on/on, default(on)
				.MIDI_CHANNEL = 0,					// midi_channel default(0)
				.MIDI_CC = 1,
			},
			{
				.IS_SERIAL = 1,						// on/on, default(on)
				.MIDI_CHANNEL = 0,					// midi_channel default(0)
				.MIDI_CC = 1,
			},
		},

	.MIDI_MONITOR					= 2,	// off, USB, Serial - default(off)
	.MONITOR_PORT 					= { 1, 1, 1, 1, 1, 1, },	// 6 * off/on
	.MONITOR_CHANNEL 				= { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, },	// 16 * off/on

	.MONITOR_SYSEX					= 2,	// off, on, Detail - default(on)
	.MONITOR_ACTIVE_SENSE			= 1,	// off, on - default(off)
	.MONITOR_NOTE_ON				= 1,	// off, on - default(off)
	.MONITOR_NOTE_OFF				= 1,	// off, on - default(off)
	.MONITOR_VELOCITY				= 1,	// off, on - default(off)   note: "after touch poly"
	.MONITOR_PROGRAM_CHG			= 1,	// off, on - default(on)
	.MONITOR_AFTER_TOUCH			= 1,	// off, on - default(off)
	.MONITOR_PITCH_BEND				= 1,	// off, on - default(off)
	.MONITOR_CCS					= 1,	// off, on - default(off)
	.MONITOR_EVERYTHING_ELSE		= 1,	// off, on - default(off)
	.MONITOR_FTP					= 1,	// off, on - default(off)
	.MONITOR_FTP_PATCHES			= 1,	// off, on - default(off)
	.MONITOR_FTP_NOTE_INFO			= 1,	// off, on - default(off)
	.MONITOR_FTP_TUNING_MSGS		= 1,	// off, on - default(off)
	.MONITOR_FTP_COMMANDS			= 1,	// off, on - default(off)
	.MONITOR_FTP_VALUES				= 1,	// off, on - default(off)
	.MONITOR_FTP_POLY_MODE			= 1,	// off, on - default(off)
	.MONITOR_FTP_BEND_MODE			= 1,	// off, on - default(off)
	.MONITOR_FTP_VOLUME				= 1,	// off, on - default(off)
	.MONITOR_FTP_BATTERY			= 1,	// off, on - default(off)
	.MONITOR_FTP_SENSITIVITY		= 1,	// off, on - default(off)
	.MONITOR_FTP_KNOWN_COMMANDS		= 1,	// off, on - default(off)
	.MONITOR_FTP_UNKNOWN_COMMANDS	= 1,	// off, on - default(off)

};	// in_memory prefs with defaults




//----------------------------------------------
// methods that work across ALL prefs
//----------------------------------------------

void reset_prefs()
	// set all prefs to default (255) value
{
    display(dbg_prefs,"reset_prefs(%d)",NUM_EEPROM_USED);
    for (uint16_t i=0; i<NUM_EEPROM_USED; i++)
    {
        EEPROM.write(i,255);
    }
	read_prefs();
}



// extern
void read_prefs()
	// set any non default (!=255) values into in-memory prefs
{
    display(dbg_prefs,"read_prefs(%d)",NUM_EEPROM_USED);
	memcpy(&prefs,&default_prefs,NUM_EEPROM_USED);

	uint8_t *ptr = (uint8_t*) &prefs;
	uint8_t *last_ptr = (uint8_t*) &last_prefs;
    for (uint16_t i=0; i<NUM_EEPROM_USED; i++)
    {
        uint8_t byte = EEPROM.read(i);
		if (byte != 255)
		{
			*ptr = byte;
			// display(0,"NON DEFAULT PREF AT BYTE %d",i);
		}
		*last_ptr = *ptr;
		ptr++;
		last_ptr++;
    }
}



// extern
void save_prefs()
	// write to EEPROM and update last_pref members
	// after the first save, all the 255's will go away in EEPROM
	// so if you change the defaults, or the structure of the prefs,
	// you MUST do a factory reset.
{
    display(dbg_prefs,"save_prefs(%d)",NUM_EEPROM_USED);
	uint8_t *ptr = (uint8_t*) &prefs;
	uint8_t *last_ptr = (uint8_t*) &last_prefs;
	const uint8_t *def_ptr = (const uint8_t *) &default_prefs;

    for (uint16_t i=0; i<NUM_EEPROM_USED; i++)
    {
		uint8_t val = *ptr;
		*last_ptr = val;
		if (val == *def_ptr)
			val = 255;
        EEPROM.write(i,val);

		ptr++;
		last_ptr++;
		def_ptr++;
    }
}



#if 0

	// written but not used so far

	//-------------------------------------------------
	// read and write individual prefs
	//-------------------------------------------------

	// extern
	uint8_t readPref8(uint16_t off, const char *name)
		// get the in-memory value
		// the name is soley for debugging
	{
		uint8_t *ptr = (uint8_t*) &prefs;
		uint8_t value = ptr[off];
		display(dbg_prefs + 1,"readPref8(%s,%d)=%d", name?name:"", off, value);
		return value;
	}


	// extern
	void writePref8(uint16_t off, uint8_t value, const char *name)
		// sets the in-memory value only!
	{
		display(dbg_prefs + 1,"writePref8(%s, %d) off=%d", name?name:"", value, off);
		uint8_t *ptr = (uint8_t*) &prefs;
		ptr[off] = value;
	}


	// extern
	uint16_t readPref16(uint16_t off, const char *name)
		// get the in-memory value
		// the name is soley for debugging
	{
		uint8_t *ptr = (uint8_t*) &prefs;
		uint16_t *ptr16 = (uint16_t *) &ptr[off];
		uint16_t value = *ptr16;
		display(dbg_prefs + 1,"readPref16(%s,%d)=%d", name?name:"", off, value);
		return value;
	}


	// extern
	void writePref16(uint16_t off, uint16_t value, const char *name)
		// sets the in-memory value only!
		// the name is soley for debugging
	{
		display(dbg_prefs + 1,"writePref16(%s, %d) off=%d", name?name:"", value, off);
		uint8_t *ptr = (uint8_t *) &prefs;
		uint16_t *ptr16 = (uint16_t *) &ptr[off];
		*ptr16 = value;
	}



	// extern
	uint8_t readPref8Min(uint16_t off)
	{
		uint8_t *ptr = (uint8_t*) &prefs_min;
		return ptr[off];
	}
	// extern
	uint8_t readPref8Max(uint16_t off)
	{
		uint8_t *ptr = (uint8_t*) &prefs_max;
		return ptr[off];
	}
	// extern
	uint16_t readPref16Min(uint16_t off)
	{
		uint8_t *ptr = (uint8_t*) &prefs_min;
		uint16_t *ptr16 = (uint16_t *) &ptr[off];
		return *ptr16;
	}
	// extern
	uint16_t readPref16Max(uint16_t off)
	{
		uint8_t *ptr = (uint8_t*) &prefs_max;
		uint16_t *ptr16 = (uint16_t *) &ptr[off];
		return *ptr16;
	}

	//------------------------------------------------------------
	// change detection and restore
	//------------------------------------------------------------

	// extern
	bool prefs_dirty()
	{
		uint8_t *ptr = (uint8_t*) &prefs;
		uint8_t *last_ptr = (uint8_t*) &last_prefs;
		for (uint16_t i=0; i<NUM_EEPROM_USED; i++)
			if (last_ptr[i] != ptr[i])
				return true;
		return false;
	}

	// extern
	bool pref8_dirty(uint16_t off)
	{
		uint8_t *ptr = (uint8_t*) &prefs;
		uint8_t *last_ptr = (uint8_t*) &last_prefs;
		return last_ptr[off] != ptr[off];
	}

	// extern
	bool pref16_dirty(uint16_t off)
	{
		uint8_t *ptr = (uint8_t*) &prefs;
		uint8_t *last_ptr = (uint8_t*) &last_prefs;
		uint16_t *ptr16 = (uint16_t *) &ptr[off];
		uint16_t *last_ptr16 = (uint16_t *) &last_ptr[off];
		return *last_ptr16 != *ptr16;
	}

	// extern
	uint8_t pref8_restore(uint16_t off, const char *name)
	{
		uint8_t *ptr = (uint8_t*) &prefs;
		uint8_t *last_ptr = (uint8_t*) &last_prefs;
		last_ptr[off] = ptr[off] = EEPROM.read(off);
		display(dbg_prefs + 1,"pref8_restore(%s, %d) to %d", name?name:"", off, ptr[off]);
		return ptr[off];
	}

	// extern
	uint16_t pref16_restore(uint16_t off, const char *name)
	{
		uint8_t *ptr = (uint8_t*) &prefs;
		uint8_t *last_ptr = (uint8_t*) &last_prefs;
		last_ptr[off] = ptr[off] = EEPROM.read(off);
		last_ptr[off+1] = ptr[off+1] = EEPROM.read(off+1);
		uint16_t *ptr16 = (uint16_t *) &ptr[off];
		display(dbg_prefs + 1,"pref16_restore(%s, %d) to %d", name?name:"", off, *ptr16);
		return *ptr16;
	}

#endif // 0
