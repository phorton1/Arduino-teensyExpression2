//--------------------------------------
// configOptions.cpp
//--------------------------------------

#include <myDebug.h>
#include "configOption.h"
#include "prefs.h"
#include "myLeds.h"

// remove when implemented

void startFtpTuner(int i) {}
void startFtpSensitivity(int i)	{}
void configPedal(int i) {}
void factoryReset(int i) {}

// enum strings

const char *off_on[]                  = {"Off","On"};
const char *off_usb_serial[]          = {"Off","USB","Serial"};
const char *off_usb_host[]            = {"Off","USB","Host"};
const char *off_on_detail[]           = {"Off","On","Detail"};
const char *off_in_out_both[]		  = {"Off", "IN", "OUT", "Both" };

const char *curve_types[]             = {"linear","asympt","scurve"};
	// not currently used here, will be needed in configPedal() dialog


//-----------------------------------------------------
// macros for constructiong various types of options
//-----------------------------------------------------
// for neatness

#define  D_ROOT( name, title, parent ) \
	const opt_desc_t name = { title, -1, 0, parent, NULL, NULL, }

#define  D_SUB_MENU( name, title, parent ) \
	static const opt_desc_t name = { title, -1, 0, parent, NULL, NULL, }

#define  D_PREF8( name, title, parent, pref_id,  setter ) \
	static const opt_desc_t name = { title, poff(pref_id), 0, parent, NULL, setter, }

#define  D_ENUM( name, title, parent, pref_id, strings ) \
	static const opt_desc_t name = { title, poff(pref_id), 0, parent, strings, NULL, }

#define  D_ETYPED( name, title, parent, pref_id, strings,  opt ) \
	static const opt_desc_t name = { title, poff(pref_id), opt, parent, strings, NULL, }

#define  D_DIALOG( name, title, parent, fxn ) \
	static const opt_desc_t name = { title, -1, 0, parent, NULL, fxn, }

#define  D_DLG_OPT( name, title, parent, fxn, opt ) \
	static const opt_desc_t name = { title, -1, opt, parent, NULL, fxn, }


//-----------------------------------------------------
// actual declaration of options
//-----------------------------------------------------


D_ROOT( 	d_root, 			"root", 		NULL );
D_PREF8(	d_brightness,		"Brightness",	&d_root,  		BRIGHTNESS,				setLEDBrightness );

D_SUB_MENU( d_ftp, 				"FTP", 			&d_root );
D_ENUM(		d_spoof_ftp,		"Spoof FTP",	&d_ftp,			SPOOF_FTP,			    off_on );
D_ENUM(		d_ftp_port,			"FTP Port",		&d_ftp,			FTP_PORT,				off_usb_host );
D_DIALOG(	d_ftp_tuner,		"Tuner",		&d_ftp,			startFtpTuner );
D_DIALOG(	d_ftp_sense,		"Sensitivty",	&d_ftp,			startFtpSensitivity );

D_SUB_MENU(	d_pedals,			"Pedals",		&d_root );
D_DIALOG(	d_pedal1,			"Pedal1",		&d_pedals,		configPedal );
D_DIALOG(	d_pedal2,			"Pedal3",		&d_pedals,		configPedal );
D_DIALOG(	d_pedal3,			"Pedal3",		&d_pedals,		configPedal );
D_DIALOG(	d_pedal4,			"Pedal4",		&d_pedals,		configPedal );

D_SUB_MENU(	d_midi_opts,		"MIDI Monitor",	&d_root);
D_ENUM(		d_midi_monitor,		"Monitor",		&d_midi_opts,	MIDI_MONITOR,			off_usb_serial );

D_SUB_MENU(	d_midi_ports,		"Ports",		&d_midi_opts);
D_ENUM(		d_mon_usb1,			"USB1",			&d_midi_ports,	MONITOR_PORT[0],	off_in_out_both );
D_ENUM(		d_mon_usb2,			"USB2",			&d_midi_ports,	MONITOR_PORT[1],	off_in_out_both );
D_ENUM(		d_mon_usb3,			"USB3",			&d_midi_ports,	MONITOR_PORT[2],	off_in_out_both );
D_ENUM(		d_mon_usb4,			"USB4",			&d_midi_ports,	MONITOR_PORT[3],	off_in_out_both );
D_ENUM(		d_mon_host1,		"HOST1",		&d_midi_ports,	MONITOR_PORT[4],	off_in_out_both );
D_ENUM(		d_mon_host,			"HOST2",		&d_midi_ports,	MONITOR_PORT[5],	off_in_out_both );
D_ENUM(		d_mon_serial,		"SERIAL",		&d_midi_ports,	MONITOR_PORT[5],	off_in_out_both );

D_SUB_MENU(	d_midi_chans,		"Channels",		&d_midi_opts);
D_ENUM(		d_mon_ch1,			"Channel 1",	&d_midi_chans,	MONITOR_CHANNEL[0],		off_on );
D_ENUM(		d_mon_ch2,			"Channel 2",	&d_midi_chans,	MONITOR_CHANNEL[1],		off_on );
D_ENUM(		d_mon_ch3,			"Channel 3",	&d_midi_chans,	MONITOR_CHANNEL[2],		off_on );
D_ENUM(		d_mon_ch4,			"Channel 4",	&d_midi_chans,	MONITOR_CHANNEL[3],		off_on );
D_ENUM(		d_mon_ch5,			"Channel 5",	&d_midi_chans,	MONITOR_CHANNEL[4],		off_on );
D_ENUM(		d_mon_ch6,			"Channel 6",	&d_midi_chans,	MONITOR_CHANNEL[5],		off_on );
D_ENUM(		d_mon_ch7,			"Channel 7",	&d_midi_chans,	MONITOR_CHANNEL[6],		off_on );
D_ENUM(		d_mon_ch8,			"Channel 8",	&d_midi_chans,	MONITOR_CHANNEL[7],		off_on );
D_ENUM(		d_mon_ch9,			"Channel 9",	&d_midi_chans,	MONITOR_CHANNEL[8],		off_on );
D_ENUM(		d_mon_ch10,			"Channel 10",	&d_midi_chans,	MONITOR_CHANNEL[9],		off_on );
D_ENUM(		d_mon_ch11,			"Channel 11",	&d_midi_chans,	MONITOR_CHANNEL[10],	off_on );
D_ENUM(		d_mon_ch12,			"Channel 12",	&d_midi_chans,	MONITOR_CHANNEL[11],	off_on );
D_ENUM(		d_mon_ch13,			"Channel 13",	&d_midi_chans,	MONITOR_CHANNEL[12],	off_on );
D_ENUM(		d_mon_ch14,			"Channel 14",	&d_midi_chans,	MONITOR_CHANNEL[13],	off_on );
D_ENUM(		d_mon_ch15,			"Channel 15",	&d_midi_chans,	MONITOR_CHANNEL[14],	off_on );
D_ENUM(		d_mon_ch16,			"Channel 16",	&d_midi_chans,	MONITOR_CHANNEL[15],	off_on );

D_SUB_MENU(	d_midi_evts,		"Events",		&d_midi_opts);
D_ENUM(		d_mon_sysex,		"Sysex",		&d_midi_evts,	MONITOR_SYSEX,			off_on_detail );
D_ENUM(		d_mon_active_sense,	"Active Sense",	&d_midi_evts,	MONITOR_ACTIVE_SENSE,	off_on );
D_ENUM(		d_mon_note_on,		"Note On",		&d_midi_evts,	MONITOR_NOTE_ON,		off_on );
D_ENUM(		d_mon_note_off,		"Note Off",		&d_midi_evts,	MONITOR_NOTE_OFF,		off_on );
D_ENUM(		d_mon_velocity,		"Velocity ",	&d_midi_evts,	MONITOR_VELOCITY,		off_on );
D_ENUM(		d_mon_program_chg,	"Program Chg",	&d_midi_evts,	MONITOR_PROGRAM_CHG,	off_on );
D_ENUM(		d_mon_after_touch,	"Aftertouch",	&d_midi_evts,	MONITOR_AFTER_TOUCH,	off_on );
D_ENUM(		d_mon_pitch_bend,	"Pitch Bend",	&d_midi_evts,	MONITOR_PITCH_BEND,		off_on );
D_ENUM(		d_mon_other_ccs,	"Other CCs", 	&d_midi_evts,	MONITOR_CCS,			off_on );
D_ENUM(		d_mon_everything,	"All Others",	&d_midi_evts,	MONITOR_EVERYTHING_ELSE,off_on );

D_SUB_MENU(	d_ftp_evts,			"FTP",				&d_midi_opts);
D_ENUM(		d_ftp_ftp,			"Monitor FTP",		&d_ftp_evts,	MONITOR_FTP,                	off_on );
D_ENUM(		d_ftp_patches,		"Patches",  		&d_ftp_evts,	MONITOR_FTP_PATCHES,  			off_on );
D_ENUM(		d_ftp_notes,		"Note Info",     	&d_ftp_evts,	MONITOR_FTP_NOTE_INFO,     		off_on );
D_ENUM(		d_ftp_tuning,		"Tuning Msgs",    	&d_ftp_evts,	MONITOR_FTP_TUNING_MSGS,    	off_on );
D_ENUM(		d_ftp_commands,		"Commands",       	&d_ftp_evts,	MONITOR_FTP_COMMANDS,       	off_on );
D_ENUM(		d_ftp_values,		"Values",         	&d_ftp_evts,	MONITOR_FTP_VALUES,         	off_on );
D_ENUM(		d_ftp_poly,			"Poly_mode",     	&d_ftp_evts,	MONITOR_FTP_POLY_MODE,     		off_on );
D_ENUM(		d_ftp_bend,			"Bend_mode",     	&d_ftp_evts,	MONITOR_FTP_BEND_MODE,      	off_on );
D_ENUM(		d_ftp_volume,		"Volume",         	&d_ftp_evts,	MONITOR_FTP_VOLUME,         	off_on );
D_ENUM(		d_ftp_battery,		"Battery",        	&d_ftp_evts,	MONITOR_FTP_BATTERY,        	off_on );
D_ENUM(		d_ftp_sensitivity,	"Sensitivity",    	&d_ftp_evts,	MONITOR_FTP_SENSITIVITY,    	off_on );
D_ENUM(		d_ftp_known,		"Known commands",  	&d_ftp_evts,	MONITOR_FTP_KNOWN_COMMANDS, 	off_on );
D_ENUM(		d_ftp_unknown,		"Unknown commands",	&d_ftp_evts,	MONITOR_FTP_UNKNOWN_COMMANDS,	off_on );

D_SUB_MENU(	d_system,			"System" TEENSY_EXPRESSION_VERSION,	&d_root);
D_ETYPED( 	d_debug_device,		"Debug Device",		&d_system,		DEBUG_DEVICE,		off_usb_serial,	OPTION_NEEDS_REBOOT );
D_ETYPED( 	d_file_sys_device,	"File Sys Device",	&d_system,		FILE_SYS_DEVICE,	off_usb_serial,	OPTION_NEEDS_REBOOT );
D_DLG_OPT(	d_factory_reset,	"Factory Reset",	&d_system,		factoryReset,		OPTION_FACTORY_RESET );


//------------------------------------------------------------
// list of all options in correct order
//------------------------------------------------------------

// extern
const opt_desc_t all_opts[] =
{
	d_root,
	d_brightness,
	d_ftp,
	d_spoof_ftp,
	d_ftp_port,
	d_ftp_tuner,
	d_ftp_sense,
	d_pedals,
	d_pedal1,
	d_pedal2,
	d_pedal3,
	d_pedal4,
	d_midi_opts,
	d_midi_monitor,
	d_midi_ports,
	d_mon_usb1,
    d_mon_usb2,
    d_mon_usb3,
    d_mon_usb4,
    d_mon_host1,
    d_mon_host,
    d_mon_serial,
	d_midi_chans,
	d_mon_ch1,
	d_mon_ch2,
	d_mon_ch3,
	d_mon_ch4,
	d_mon_ch5,
	d_mon_ch6,
	d_mon_ch7,
	d_mon_ch8,
	d_mon_ch9,
	d_mon_ch10,
	d_mon_ch11,
	d_mon_ch12,
	d_mon_ch13,
	d_mon_ch14,
	d_mon_ch15,
	d_mon_ch16,
	d_midi_evts,
	d_mon_sysex,
	d_mon_active_sense,
	d_mon_note_on,
	d_mon_note_off,
	d_mon_velocity,
	d_mon_program_chg,
	d_mon_after_touch,
	d_mon_pitch_bend,
	d_mon_other_ccs,
	d_mon_everything,
	d_ftp_evts,
	d_ftp_ftp,
	d_ftp_patches,
	d_ftp_notes,
	d_ftp_tuning,
	d_ftp_commands,
	d_ftp_values,
	d_ftp_poly,
	d_ftp_bend,
	d_ftp_volume,
	d_ftp_battery,
	d_ftp_sensitivity,
	d_ftp_known,
	d_ftp_unknown,
	d_system,
	d_debug_device,
	d_file_sys_device,
	d_factory_reset,
};


// extern
uint16_t NUM_OPTS()
{
	return sizeof(all_opts) / sizeof(opt_desc_t);
}