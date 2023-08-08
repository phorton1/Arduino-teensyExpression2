//-------------------------------------------
// configOptions.h
//-------------------------------------------
// contains const description of config options tree
// for use by configsystem

#pragma once

// #define OPTION_16BIT			0x0001
// will be needed for pedal CALIB_MIN and CALIB_MAX

#define OPTION_FACTORY_RESET	0x8000
#define OPTION_NEEDS_REBOOT		0x4000

typedef void (*setter_fxn)(int i);
	// functions that are callable from options
	// i.e. onChange(BRIGHTNESS) or invoke_dialog(FTP_TUNER)

// Const Data Structure

typedef struct opt_desc
{
	const char		*title;
	int				pref_offset;		// offset in preferences for calling read/writePref
	int				type;				// bitwise type from above
	const opt_desc	*p_parent;			// pointer to parent
	const char		**strings;			// the allowable strings, may be null
	setter_fxn		setter;				// setter function called onChange()

} opt_desc_t;


///--------------------------------------
// public list of all options
//---------------------------------------
// in a particular order

extern const opt_desc_t all_opts[];
extern uint16_t NUM_OPTS();


// end of configOptions.h
