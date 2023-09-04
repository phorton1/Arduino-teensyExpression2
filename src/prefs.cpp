//--------------------------------------
// prefs.cpp
//--------------------------------------

#include <myDebug.h>
#include "prefs.h"
#include <EEPROM.h>
#include "iPadDefines.h"
#include "commonDefines.h"

#define dbg_prefs	0
	// 0 = show file operations
	// -1 = show restore operations and setPrefs
	// -2 = show readPrefs
#define dbg_changes 1
	// 0 = show warnings on change detection


#define PREF_VERSION_OFFSET		1
#define NUM_EEPROM_USED	(sizeof(prefs_t) + PREF_VERSION_OFFSET)


prefs_t prefs;
	// in memory prefs
prefs_t last_prefs;
	// a copy of the prefs for change detectioni

//-----------------------------------------------------
// prefTypes
//-----------------------------------------------------
// non-uint8_t prefs MUST be registered with this method

#define PREF_TYPE_NONE			0		// -1
#define PREF_TYPE_8				1		// uint8_t
#define PREF_TYPE_16			2		// uint16_t
#define PREF_TYPE_RIG_NAME		3		// char[MAX_RIG_NAME + 1]


uint8_t prefType(int off)
{
	if (off == poff(RIG_NAME))
	{
		return PREF_TYPE_RIG_NAME;
	}
	else if (
		off == poff(PEDAL[0].CALIB_MIN) ||
		off == poff(PEDAL[1].CALIB_MIN) ||
		off == poff(PEDAL[2].CALIB_MIN) ||
		off == poff(PEDAL[3].CALIB_MIN) ||
		off == poff(PEDAL[0].CALIB_MAX) ||
		off == poff(PEDAL[1].CALIB_MAX) ||
		off == poff(PEDAL[2].CALIB_MAX) ||
		off == poff(PEDAL[3].CALIB_MAX) )
	{
		return PREF_TYPE_16;
	}
	return PREF_TYPE_8;
}


//----------------------------------------------------
// DEFAULT PREFS
//----------------------------------------------------
// are statically initialized with the default values

#define PP_MAX			127
#define PP_HALF			(PP_MAX / 2)
#define PP_ONE			(PP_MAX / 3)
#define PP_TWO			(2 * PP_MAX / 3)


static const prefs_t default_prefs =
{
	.BRIGHTNESS			= 30,				// LED brightness, 1..100 - default=30
	.DEBUG_DEVICE		= DEBUG_DEVICE_SERIAL,		// off, USB, Serial - default(2=Serial)
	.FILE_SYS_DEVICE	= OUTPUT_DEVICE_DEBUG,		// off, Debug, USB, Serial - default(Debug)
	.SPOOF_FTP          = 0,						// off, on
	.FTP_PORT			= 1,						// off, USB, HOST

	.RIG_NAME		    = {'S','y','s','t','e','m','_','D','e','f','a','u','l','t', 0},

	.PEDAL = {
		{											// pedal 0 - Synth
			.CALIB_MIN	= 0,						// 0..1023 - default(0)
			.CALIB_MAX	= 1023,						// 0..1023 - default(1023)
			.CURVE_TYPE	= 0,						// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
			.CURVE = {                      		// setup simple curves for the three different types
			{ .POINTS = { {0,0}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_HALF,PP_HALF}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_ONE,PP_ONE}, {PP_TWO,PP_TWO}, {PP_MAX,PP_MAX}  }, }, },
		},
		{											// pedal 1 = Looper
			.CALIB_MIN	= 0,						// 0..1023 - default(0)
			.CALIB_MAX	= 1023,						// 0..1023 - default(1023)
			.CURVE_TYPE	= 0,						// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
			.CURVE = {                      		// setup simple curves for the three different types
			{ .POINTS = { {0,0}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_HALF,PP_HALF}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_ONE,PP_ONE}, {PP_TWO,PP_TWO}, {PP_MAX,PP_MAX}  }, }, },
		},
		{											// pedal 2 = WAH
			.CALIB_MIN	= 0,						// 0..1023 - default(0)
			.CALIB_MAX	= 1023,						// 0..1023 - default(1023)
			.CURVE_TYPE	= 0,						// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
			.CURVE = {                      		// setup simple curves for the three different types
			{ .POINTS = { {0,0}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_HALF,PP_HALF}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_ONE,PP_ONE}, {PP_TWO,PP_TWO}, {PP_MAX,PP_MAX}  }, }, },
		},
		{											// pedal 3 = Guitar Volume
			.CALIB_MIN	= 0,						// 0..1023 - default(0)
			.CALIB_MAX	= 1023,						// 0..1023 - default(1023)
			.CURVE_TYPE	= 0,						// 0=linear, 1=asymptotic, 2=scurve - default(0=;inear)
			.CURVE = {                      		// setup simple curves for the three different types
			{ .POINTS = { {0,0}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_HALF,PP_HALF}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {0,0}, {PP_ONE,PP_ONE}, {PP_TWO,PP_TWO}, {PP_MAX,PP_MAX}  }, }, },
        }, },

	.MIDI_MONITOR					= OUTPUT_DEVICE_DEBUG,		// off, Debug, USB, Serial - default(0=off) 			DEBUG in testing
	.MONITOR_INPUT  				= { 1, 1, 1, 1, 1, 1, 1, },							 	// 7 ports     off, on		all on in testing
	.MONITOR_OUTPUT  				= { 1, 1, 1, 1, 1, 1, 1, },							 	// 7 ports     off, on		all on in testing
	.MONITOR_CHANNEL 				= { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },	// 16 channels off, on		all on in testing

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

};	// const pref defaults


//----------------------------------------------------
// PREF MIN & MAX VALUES
//----------------------------------------------------
// only one value has a non-zero minimum

static const prefs_t prefs_min = {
	.BRIGHTNESS			= 5,
};


static const prefs_t prefs_max =
{
	.BRIGHTNESS			= 100,					// LED brightness, 1..100
	.DEBUG_DEVICE		= DEBUG_DEVICE_SERIAL,  // off, USB, Serial
	.FILE_SYS_DEVICE	= OUTPUT_DEVICE_SERIAL, // off, Debug, USB, Serial
	.SPOOF_FTP			= 1,					// off, on
	.FTP_PORT			= 2,					// off, USB, HOST

	.PEDAL = {
		{									// pedal 0
			.CALIB_MIN	= 1023,				// 0..1023
			.CALIB_MAX	= 1023,				// 0..1023
			.CURVE_TYPE	= 2,				// 0=linear, 1=asymptotic, 2=scurve
			.CURVE = {
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}  }, }, },
		},
		{									// pedal 1
			.CALIB_MIN	= 1023,				// 0..1023
			.CALIB_MAX	= 1023,				// 0..1023
			.CURVE_TYPE	= 2,				// 0=linear, 1=asymptotic, 2=scurve
			.CURVE = {
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}  }, }, },
		},
		{									// pedal 2
			.CALIB_MIN	= 1023,				// 0..1023
			.CALIB_MAX	= 1023,				// 0..1023
			.CURVE_TYPE	= 2,				// 0=linear, 1=asymptotic, 2=scurve
			.CURVE = {
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}  }, }, },
		},
		{									// pedal 3
			.CALIB_MIN	= 1023,				// 0..1023
			.CALIB_MAX	= 1023,				// 0..1023
			.CURVE_TYPE	= 2,				// 0=linear, 1=asymptotic, 2=scurve
			.CURVE = {
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {0,0} }, },
			{ .POINTS = { {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}, {PP_MAX,PP_MAX}  }, }, },
        }, },

	.MIDI_MONITOR					= DEBUG_DEVICE_SERIAL,									// off, USB, Serial
	.MONITOR_INPUT  				= { 1, 1, 1, 1, 1, 1, 1, },							 	// 7 ports     off, on
	.MONITOR_OUTPUT  				= { 1, 1, 1, 1, 1, 1, 1, },							 	// 7 ports     off, on
	.MONITOR_CHANNEL 				= { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },	// 16 channels off, on

	.MONITOR_SYSEX					= 2,	// off, on, Detail
	.MONITOR_ACTIVE_SENSE			= 1,	// off, on
	.MONITOR_NOTE_ON				= 1,	// off, on
	.MONITOR_NOTE_OFF				= 1,	// off, on
	.MONITOR_VELOCITY				= 1,	// off, on, note: "after touch poly"
	.MONITOR_PROGRAM_CHG			= 1,	// off, on
	.MONITOR_AFTER_TOUCH			= 1,	// off, on
	.MONITOR_PITCH_BEND				= 1,	// off, on
	.MONITOR_CCS					= 1,	// off, on
	.MONITOR_EVERYTHING_ELSE		= 1,	// off, on
	.MONITOR_FTP					= 1,	// off, on
	.MONITOR_FTP_PATCHES			= 1,	// off, on
	.MONITOR_FTP_NOTE_INFO			= 1,	// off, on
	.MONITOR_FTP_TUNING_MSGS		= 1,	// off, on
	.MONITOR_FTP_COMMANDS			= 1,	// off, on
	.MONITOR_FTP_VALUES				= 1,	// off, on
	.MONITOR_FTP_POLY_MODE			= 1,	// off, on
	.MONITOR_FTP_BEND_MODE			= 1,	// off, on
	.MONITOR_FTP_VOLUME				= 1,	// off, on
	.MONITOR_FTP_BATTERY			= 1,	// off, on
	.MONITOR_FTP_SENSITIVITY		= 1,	// off, on
	.MONITOR_FTP_KNOWN_COMMANDS		= 1,	// off, on
	.MONITOR_FTP_UNKNOWN_COMMANDS	= 1,	// off, on

};	// const prefs maximums




//----------------------------------------------
// methods that work across ALL prefs
//----------------------------------------------

void reset_prefs()
	// set EEPROM(0) to TEENSY_EXPRESSION2_PREF_VERSION
	// and EEPROM(1..NUM_EEPROM_USED) to 255
{
    display(dbg_prefs,"reset_prefs()",0);
	EEPROM.write(0,TEENSY_EXPRESSION2_PREF_VERSION);
    for (uint16_t i=PREF_VERSION_OFFSET; i<NUM_EEPROM_USED; i++)
    {
        EEPROM.write(i,255);
    }
}



// extern
bool read_prefs()
	// set any non default (!=255) values into in-memory prefs
{
	display(dbg_prefs,"read_prefs()",0);

	bool retval = 0;
	if (EEPROM.read(0) != TEENSY_EXPRESSION2_PREF_VERSION)
	{
		retval = 1;
		reset_prefs();
	}

	memcpy(&prefs,&default_prefs,sizeof(prefs_t));

	// read the prefs by type in a stream like manner

	uint16_t offset = 0;
	uint8_t *ptr = (uint8_t *) &prefs;
	uint8_t *last_ptr = (uint8_t *) &last_prefs;

	while (offset < sizeof(prefs))
	{
		uint8_t type = prefType(offset);
		if (type == PREF_TYPE_8)
		{
			uint8_t byte = EEPROM.read(PREF_VERSION_OFFSET + offset);
			if (byte != 255)
			{
				*ptr = byte;
				warning(dbg_prefs,"read_prefs non-default PREF_8(0x%02x) at offset %d",byte,offset);
			}
			*last_ptr = *ptr;
			ptr++;
			last_ptr++;
			offset++;
		}
		else if (type == PREF_TYPE_16)
		{
			uint8_t byte0 = EEPROM.read(PREF_VERSION_OFFSET + offset);
			uint8_t byte1 = EEPROM.read(PREF_VERSION_OFFSET + offset+1);
			if (byte0 != 255 || byte1 != 255)
			{
				ptr[0] = byte0;
				ptr[1] = byte1;
				warning(dbg_prefs,"read_prefs non-default PREF_16(0x%02x%02x) at offset %d",byte0,byte1,offset);
			}
			last_ptr[0] = ptr[0];
			last_ptr[1] = ptr[1];
			ptr += 2;
			last_ptr += 2;
			offset += 2;
		}
		else if (type == PREF_TYPE_RIG_NAME)
		{
			uint8_t bytes[MAX_RIG_NAME+1];
			bool is_default = 1;
			for (int i=0; i<=MAX_RIG_NAME; i++)
			{
				bytes[i] = EEPROM.read(PREF_VERSION_OFFSET + offset + i);
				if (bytes[i] != 255)
					is_default = 0;
			}
			if (!is_default)
			{
				warning(dbg_prefs,"read_prefs non-default PREF_RIG_NAME(%s) at offset %d=",(const char *)bytes,offset);
				for (int i=0; i<=MAX_RIG_NAME; i++)
				{
					ptr[i] = bytes[i];
				}
			}
			for (int i=0; i<=MAX_RIG_NAME; i++)
			{
				last_ptr[i] = ptr[i];
			}
			ptr += MAX_RIG_NAME+1;
			last_ptr += MAX_RIG_NAME+1;
			offset += MAX_RIG_NAME+1;
		}
		else
		{
			my_error("illegal prefType(%d) in read_prefs() at offset(%d)",type, offset);
			return false;
		}
	}

	return retval;
}



// extern
void save_prefs()
	// write to EEPROM and update last_pref members
	// writes 255 for prefs that match the default
	// PRH THERE ARE CERTAIN VALUES FOR WHICH THIS WILL NOT WORK!
	// i.e. if the low order of a uint16_t happens to be 255
{
    display(dbg_prefs,"save_prefs()",0);

	uint16_t offset = 0;
	uint8_t *last_ptr = (uint8_t*) &last_prefs;
	const uint8_t *ptr = (const uint8_t*) &prefs;
	const uint8_t *def_ptr = (const uint8_t *) &default_prefs;

	while (offset < sizeof(prefs))
	{
		uint8_t type = prefType(offset);
		if (type == PREF_TYPE_8)
		{
			uint8_t byte = *ptr;
			*last_ptr = *ptr;
			if (byte == *def_ptr)
			{
				byte = 255;
			}
			else
			{
				warning(dbg_prefs,"save_prefs non-default PREF_8(0x%02x) at offset %d",byte,offset);
			}
			EEPROM.write(PREF_VERSION_OFFSET + offset, byte);
			ptr++;
			def_ptr++;
			last_ptr++;
			offset++;
		}
		else if (type == PREF_TYPE_16)
		{
			uint8_t byte0 = ptr[0];
			uint8_t byte1 = ptr[1];
			last_ptr[0] = byte0;
			last_ptr[1] = byte1;
			if (byte0 == def_ptr[0] && byte1 == def_ptr[1])
			{
				byte0 = 255;
				byte1 = 255;
			}
			else
			{
				warning(dbg_prefs,"save_prefs non-default PREF_16(0x%02x%02x) at offset %d",byte0,byte1,offset);
			}
			EEPROM.write(PREF_VERSION_OFFSET + offset, 	   byte0);
			EEPROM.write(PREF_VERSION_OFFSET + offset + 1, byte1);
			ptr += 2;
			def_ptr += 2;
			last_ptr += 2;
			offset += 2;
		}
		else if (type == PREF_TYPE_RIG_NAME)
		{
			uint8_t bytes[MAX_RIG_NAME+1];
			for (int i=0; i<=MAX_RIG_NAME; i++)
			{
				bytes[i] = ptr[i];
				last_ptr[i] = ptr[i];
			}

			bool is_default = 1;
			for (int i=0; i<=MAX_RIG_NAME; i++)
			{
				if (bytes[i] != def_ptr[i])
				{
					is_default = 0;
				}
			}

			if (is_default)
			{
				for (int i=0; i<=MAX_RIG_NAME; i++)
				{
					bytes[i] = 255;
				}
			}
			else
			{
				warning(dbg_prefs,"save_prefs non-default PREF_RIG_NAME(%s) at offset %d=",(const char *)bytes,offset);
			}
			for (int i=0; i<=MAX_RIG_NAME; i++)
			{
				EEPROM.write(PREF_VERSION_OFFSET + offset + i, bytes[i]);
			}

			ptr += MAX_RIG_NAME+1;
			def_ptr += MAX_RIG_NAME+1;
			last_ptr += MAX_RIG_NAME+1;
			offset += MAX_RIG_NAME+1;
		}
		else
		{
			my_error("illegal prefType(%d) in save_prefs() at offset(%d)",type,offset);
			return;
		}
	}
}



//-------------------------------------------------
// read and write individual prefs
//-------------------------------------------------

void setPref(uint16_t off, uint32_t value, const char *name)
{
	uint8_t type = prefType(off);
	display(dbg_prefs,"setPref(%d==%s,0x%08x) type(%d)",off,name?name:"",value,type);
	if (type == PREF_TYPE_8)
	{
		uint8_t val = value;
		uint8_t *ptr = (uint8_t*) &prefs;
		ptr[off] = val;
	}
	else if (type == PREF_TYPE_16)
	{
		uint16_t val = value;
		uint8_t *ptr = (uint8_t*) &prefs;
		uint16_t *ptr16 = (uint16_t *) &ptr[off];
		*ptr16 = val;
	}
	else if (type == PREF_TYPE_RIG_NAME)
	{
		char *ptr = (char*) &prefs;
		memcpy(&ptr[off],(const char *) value, MAX_RIG_NAME+1);
		ptr[MAX_RIG_NAME+1] = 0;
	}
	else
	{
		my_error("attempt to setPref(%d==%s,0x%08x) type(%d)",off,name?name:"",value,type);
	}
}


uint32_t getPref(uint16_t off, const char *name)
{
	uint8_t type = prefType(off);
	display(dbg_prefs + 1,"getPref(%d==%s) type(%d)",off,name?name:"",type);
	if (type == PREF_TYPE_8)
	{
		const uint8_t *ptr = (const uint8_t*) &prefs;
		return ptr[off];
	}
	else if (type == PREF_TYPE_16)
	{
		const uint8_t *ptr = (const uint8_t*) &prefs;
		const uint16_t *ptr16 = (const uint16_t *) &ptr[off];
		return *ptr16;
	}
	else if (type == PREF_TYPE_RIG_NAME)
	{
		const uint8_t *ptr = (const uint8_t *) &prefs;
		return (uint32_t) &ptr[off];
	}
	my_error("attempt to getPref(%d==%s) type(%d)",off,name?name:"",type);
	return 0;
}


uint16_t getPrefMin(uint16_t off)
{
	uint8_t type = prefType(off);
	if (type == PREF_TYPE_8)
	{
		const uint8_t *ptr = (const uint8_t*) &prefs_min;
		return ptr[off];
	}
	else if (type == PREF_TYPE_16)
	{
		const uint8_t *ptr = (const uint8_t*) &prefs_min;
		const uint16_t *ptr16 = (const uint16_t *) &ptr[off];
		return *ptr16;
	}
	my_error("attempt to getPrefMin(%d) type(%d)",off,type);
	return 0;
}


uint16_t getPrefMax(uint16_t off)
{
	uint8_t type = prefType(off);
	if (type == PREF_TYPE_8)
	{
		const uint8_t *ptr = (const uint8_t*) &prefs_max;
		return ptr[off];
	}
	else if (type == PREF_TYPE_16)
	{
		const uint8_t *ptr = (const uint8_t*) &prefs_max;
		const uint16_t *ptr16 = (const uint16_t *) &ptr[off];
		return *ptr16;
	}
	my_error("attempt to getPrefMin(%d) type(%d)",off,type);
	return 0;
}


// extern
bool prefs_dirty()
{
	uint8_t *ptr = (uint8_t*) &prefs;
	uint8_t *last_ptr = (uint8_t*) &last_prefs;
	for (uint16_t i=0; i<sizeof(prefs_t); i++)
		if (last_ptr[i] != ptr[i])
		{
			warning(dbg_changes,"prefs_dirty returning 1",0);
			return true;
		}
	warning(dbg_changes,"prefs_dirty returning 0",0);
	return false;
}
