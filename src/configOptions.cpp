//--------------------------------------
// configOptions.cpp
//--------------------------------------

#include <myDebug.h>
#include "configOptions.h"
#include "prefs.h"
#include "myLeds.h"
#include "theSystem.h"
// #include "winDialogs.h"
#include "winFtpTuner.h"
#include "winFtpSensitivity.h"


#define dbg_opts	0
	// 0 = show setValue/incValue
	// -1 = show init()

configOptions the_options;
configOption options[NUM_CONFIG_OPTIONS];


// enum strings

static const char *off_on[]                  = {"Off","On"};
static const char *off_usb_serial[]          = {"Off","USB","Serial"};
static const char *off_usb_host[]            = {"Off","USB","Host"};
static const char *off_dbg_usb_host[]        = {"Off","Debug Device","USB","Host"};
static const char *off_on_detail[]           = {"Off","On","Detail"};
//static const char *curve_types[]             = {"linear","asympt","scurve"};

// setter functions

static void startFtpTuner(int i)
	{ the_system.startWindow(&win_ftp_tuner); }
static void startFtpSensitivity(int i)
	{ the_system.startWindow(&win_ftp_sensitivity); }
static void configPedal(int i) {}


//-----------------------------------------------------
// macros for constructiong various types of options
//-----------------------------------------------------
// for neatness

#define  D_ROOT( num, name, title, parent ) \
	const opt_desc_t name = { num, title, -1, 0, parent, NULL, NULL, }

#define  D_SUB_MENU( num, name, title, parent ) \
	static const opt_desc_t name = { num, title, -1, 0, parent, NULL, NULL, }

#define  D_PREF8( num, name, title, parent, pref_id,  setter ) \
	static const opt_desc_t name = { num, title, poff(pref_id), 0, parent, NULL, setter, }

#define  D_ENUM( num, name, title, parent, pref_id, strings ) \
	static const opt_desc_t name = { num, title, poff(pref_id), 0, parent, strings, NULL, }

#define  D_ENUM_OPT( num, name, title, parent, pref_id, strings, opt ) \
	static const opt_desc_t name = { num, title, poff(pref_id), opt, parent, strings, NULL, }

#define  D_DIALOG( num, name, title, parent, fxn ) \
	static const opt_desc_t name = { num, title, -1, 0, parent, NULL, fxn, }

#define  D_DLG_OPT( num, name, title, parent, fxn, opt ) \
	static const opt_desc_t name = { num, title, -1, opt, parent, NULL, fxn, }


//-----------------------------------------------------
// declaration of opt_desc_t's
//-----------------------------------------------------
// they are grouped in breadth-first menu level order

D_ROOT( 	0,	d_root, 			"root", 			NULL );
D_PREF8(	1,	d_brightness,		"Brightness",		&d_root,  		BRIGHTNESS,				setLEDBrightness );
D_SUB_MENU( 2,	d_ftp, 				"FTP", 				&d_root );
D_SUB_MENU(	3,	d_pedals,			"Pedals",			&d_root );
D_SUB_MENU(	4,	d_midi_opts,		"MIDI Monitor",		&d_root );
D_SUB_MENU(	5,	d_system,			"System " TEENSY_EXPRESSION_VERSION,&d_root);
D_ENUM_OPT(	6,	d_spoof_ftp,		"Spoof FTP",		&d_ftp,			SPOOF_FTP,			    off_on,			OPTION_SPOOF_FTP );
D_ENUM(		7,	d_ftp_port,			"FTP Port",			&d_ftp,			FTP_PORT,				off_usb_host );
D_DIALOG(	8,	d_ftp_tuner,		"Tuner",			&d_ftp,			startFtpTuner );
D_DIALOG(	9,	d_ftp_sense,		"Sensitivty",		&d_ftp,			startFtpSensitivity );
D_DIALOG(	10,	d_pedal1,			"Pedal1",			&d_pedals,		configPedal );
D_DIALOG(	11,	d_pedal2,			"Pedal3",			&d_pedals,		configPedal );
D_DIALOG(	12, d_pedal3,			"Pedal3",			&d_pedals,		configPedal );
D_DIALOG(	13, d_pedal4,			"Pedal4",			&d_pedals,		configPedal );
D_ENUM(		14,	d_midi_monitor,		"Monitor",			&d_midi_opts,	MIDI_MONITOR,			off_dbg_usb_host );
D_SUB_MENU(	15,	d_midi_ports,		"Ports",			&d_midi_opts);
D_SUB_MENU(	16,	d_midi_chans,		"Channels",			&d_midi_opts);
D_SUB_MENU(	17,	d_midi_evts,		"Events",			&d_midi_opts);
D_SUB_MENU(	18,	d_ftp_evts,			"FTP",				&d_midi_opts);
D_ENUM(		19,	d_mon_usb1_in,		"USB1 in",			&d_midi_ports,	MONITOR_INPUT [0],		off_on );
D_ENUM(		20,	d_mon_usb1_out,		"USB1 out",			&d_midi_ports,	MONITOR_OUTPUT[0],		off_on );
D_ENUM(		21,	d_mon_usb2_in,		"USB2 in",			&d_midi_ports,	MONITOR_INPUT [1],		off_on );
D_ENUM(		22,	d_mon_usb2_out,		"USB2 out",			&d_midi_ports,	MONITOR_OUTPUT[1],		off_on );
D_ENUM(		23,	d_mon_usb3_in,		"USB3 in",			&d_midi_ports,	MONITOR_INPUT [2],		off_on );
D_ENUM(		24,	d_mon_usb3_out,		"USB3 out",			&d_midi_ports,	MONITOR_OUTPUT[2],		off_on );
D_ENUM(		25,	d_mon_usb4_in,		"USB4 in",			&d_midi_ports,	MONITOR_INPUT [3],		off_on );
D_ENUM(		26,	d_mon_usb4_out,		"USB4 out",			&d_midi_ports,	MONITOR_OUTPUT[3],		off_on );
D_ENUM(		27,	d_mon_host1_in,		"HOST1 in",			&d_midi_ports,	MONITOR_INPUT [4],		off_on );
D_ENUM(		28,	d_mon_host1_out,	"HOST1 out",		&d_midi_ports,	MONITOR_OUTPUT[4],		off_on );
D_ENUM(		29,	d_mon_host2_in,		"HOST2 in",			&d_midi_ports,	MONITOR_INPUT [5],		off_on );
D_ENUM(		30,	d_mon_host2_out,	"HOST2 out",		&d_midi_ports,	MONITOR_OUTPUT[5],		off_on );
D_ENUM(		31,	d_mon_serial_in,	"SERIAL in",		&d_midi_ports,	MONITOR_INPUT [6],		off_on );
D_ENUM(		32,	d_mon_serial_out,	"SERIAL out",		&d_midi_ports,	MONITOR_OUTPUT[6],		off_on );
D_ENUM(		33,	d_mon_ch1,			"Channel 1",		&d_midi_chans,	MONITOR_CHANNEL[0],		off_on );
D_ENUM(		34,	d_mon_ch2,			"Channel 2",		&d_midi_chans,	MONITOR_CHANNEL[1],		off_on );
D_ENUM(		35,	d_mon_ch3,			"Channel 3",		&d_midi_chans,	MONITOR_CHANNEL[2],		off_on );
D_ENUM(		36,	d_mon_ch4,			"Channel 4",		&d_midi_chans,	MONITOR_CHANNEL[3],		off_on );
D_ENUM(		37,	d_mon_ch5,			"Channel 5",		&d_midi_chans,	MONITOR_CHANNEL[4],		off_on );
D_ENUM(		38,	d_mon_ch6,			"Channel 6",		&d_midi_chans,	MONITOR_CHANNEL[5],		off_on );
D_ENUM(		39,	d_mon_ch7,			"Channel 7",		&d_midi_chans,	MONITOR_CHANNEL[6],		off_on );
D_ENUM(		40,	d_mon_ch8,			"Channel 8",		&d_midi_chans,	MONITOR_CHANNEL[7],		off_on );
D_ENUM(		41,	d_mon_ch9,			"Channel 9",		&d_midi_chans,	MONITOR_CHANNEL[8],		off_on );
D_ENUM(		42,	d_mon_ch10,			"Channel 10",		&d_midi_chans,	MONITOR_CHANNEL[9],		off_on );
D_ENUM(		43,	d_mon_ch11,			"Channel 11",		&d_midi_chans,	MONITOR_CHANNEL[10],	off_on );
D_ENUM(		44,	d_mon_ch12,			"Channel 12",		&d_midi_chans,	MONITOR_CHANNEL[11],	off_on );
D_ENUM(		45,	d_mon_ch13,			"Channel 13",		&d_midi_chans,	MONITOR_CHANNEL[12],	off_on );
D_ENUM(		46,	d_mon_ch14,			"Channel 14",		&d_midi_chans,	MONITOR_CHANNEL[13],	off_on );
D_ENUM(		47,	d_mon_ch15,			"Channel 15",		&d_midi_chans,	MONITOR_CHANNEL[14],	off_on );
D_ENUM(		48,	d_mon_ch16,			"Channel 16",		&d_midi_chans,	MONITOR_CHANNEL[15],	off_on );
D_ENUM(		49,	d_mon_sysex,		"Sysex",			&d_midi_evts,	MONITOR_SYSEX,			off_on_detail );
D_ENUM(		50,	d_mon_active_sense,	"Active Sense",		&d_midi_evts,	MONITOR_ACTIVE_SENSE,	off_on );
D_ENUM(		51,	d_mon_note_on,		"Note On",			&d_midi_evts,	MONITOR_NOTE_ON,		off_on );
D_ENUM(		52,	d_mon_note_off,		"Note Off",			&d_midi_evts,	MONITOR_NOTE_OFF,		off_on );
D_ENUM(		53,	d_mon_velocity,		"Velocity ",		&d_midi_evts,	MONITOR_VELOCITY,		off_on );
D_ENUM(		54,	d_mon_program_chg,	"Program Chg",		&d_midi_evts,	MONITOR_PROGRAM_CHG,	off_on );
D_ENUM(		55,	d_mon_after_touch,	"Aftertouch",		&d_midi_evts,	MONITOR_AFTER_TOUCH,	off_on );
D_ENUM(		56,	d_mon_pitch_bend,	"Pitch Bend",		&d_midi_evts,	MONITOR_PITCH_BEND,		off_on );
D_ENUM(		57,	d_mon_other_ccs,	"Other CCs", 		&d_midi_evts,	MONITOR_CCS,			off_on );
D_ENUM(		58,	d_mon_everything,	"All Others",		&d_midi_evts,	MONITOR_EVERYTHING_ELSE,off_on );
D_ENUM(		59,	d_ftp_ftp,			"Monitor FTP",		&d_ftp_evts,	MONITOR_FTP,                	off_on );
D_ENUM(		60,	d_ftp_patches,		"Patches",  		&d_ftp_evts,	MONITOR_FTP_PATCHES,  			off_on );
D_ENUM(		61,	d_ftp_notes,		"Note Info",     	&d_ftp_evts,	MONITOR_FTP_NOTE_INFO,     		off_on );
D_ENUM(		62,	d_ftp_tuning,		"Tuning Msgs",    	&d_ftp_evts,	MONITOR_FTP_TUNING_MSGS,    	off_on );
D_ENUM(		63,	d_ftp_commands,		"Commands",       	&d_ftp_evts,	MONITOR_FTP_COMMANDS,       	off_on );
D_ENUM(		64,	d_ftp_values,		"Values",         	&d_ftp_evts,	MONITOR_FTP_VALUES,         	off_on );
D_ENUM(		65,	d_ftp_poly,			"Poly_mode",     	&d_ftp_evts,	MONITOR_FTP_POLY_MODE,     		off_on );
D_ENUM(		66,	d_ftp_bend,			"Bend_mode",     	&d_ftp_evts,	MONITOR_FTP_BEND_MODE,      	off_on );
D_ENUM(		67,	d_ftp_volume,		"Volume",         	&d_ftp_evts,	MONITOR_FTP_VOLUME,         	off_on );
D_ENUM(		68,	d_ftp_battery,		"Battery",        	&d_ftp_evts,	MONITOR_FTP_BATTERY,        	off_on );
D_ENUM(		69,	d_ftp_sensitivity,	"Sensitivity",    	&d_ftp_evts,	MONITOR_FTP_SENSITIVITY,    	off_on );
D_ENUM(		70,	d_ftp_known,		"Known commands",  	&d_ftp_evts,	MONITOR_FTP_KNOWN_COMMANDS, 	off_on );
D_ENUM(		71,	d_ftp_unknown,		"Unknown commands",	&d_ftp_evts,	MONITOR_FTP_UNKNOWN_COMMANDS,	off_on );
D_ENUM( 	72,	d_debug_device,		DEBUG_DEVICE_NAME,	&d_system,		DEBUG_DEVICE,		off_usb_serial );
D_ENUM( 	73,	d_file_sys_device,	"File Sys Device",	&d_system,		FILE_SYS_DEVICE,	off_dbg_usb_host );
D_DLG_OPT(	74,	d_factory_reset,	"Factory Reset",	&d_system,		0,					OPTION_FACTORY_RESET );


//------------------------------------------------------------
// list of all options in correct order
//------------------------------------------------------------

static const opt_desc_t *all_opts[] =
{
	&d_root,
    &d_brightness,
    &d_ftp,
    &d_pedals,
    &d_midi_opts,
    &d_system,
    &d_spoof_ftp,
    &d_ftp_port,
    &d_ftp_tuner,
    &d_ftp_sense,
    &d_pedal1,
    &d_pedal2,
    &d_pedal3,
    &d_pedal4,
    &d_midi_monitor,
    &d_midi_ports,
    &d_midi_chans,
    &d_midi_evts,
    &d_ftp_evts,
    &d_mon_usb1_in,
    &d_mon_usb1_out,
    &d_mon_usb2_in,
    &d_mon_usb2_out,
    &d_mon_usb3_in,
    &d_mon_usb3_out,
    &d_mon_usb4_in,
    &d_mon_usb4_out,
    &d_mon_host1_in,
    &d_mon_host1_out,
    &d_mon_host2_in,
    &d_mon_host2_out,
    &d_mon_serial_in,
    &d_mon_serial_out,
    &d_mon_ch1,
    &d_mon_ch2,
    &d_mon_ch3,
    &d_mon_ch4,
    &d_mon_ch5,
    &d_mon_ch6,
    &d_mon_ch7,
    &d_mon_ch8,
    &d_mon_ch9,
    &d_mon_ch10,
    &d_mon_ch11,
    &d_mon_ch12,
    &d_mon_ch13,
    &d_mon_ch14,
    &d_mon_ch15,
    &d_mon_ch16,
    &d_mon_sysex,
    &d_mon_active_sense,
    &d_mon_note_on,
    &d_mon_note_off,
    &d_mon_velocity,
    &d_mon_program_chg,
    &d_mon_after_touch,
    &d_mon_pitch_bend,
    &d_mon_other_ccs,
    &d_mon_everything,
    &d_ftp_ftp,
    &d_ftp_patches,
    &d_ftp_notes,
    &d_ftp_tuning,
    &d_ftp_commands,
    &d_ftp_values,
    &d_ftp_poly,
    &d_ftp_bend,
    &d_ftp_volume,
    &d_ftp_battery,
    &d_ftp_sensitivity,
    &d_ftp_known,
    &d_ftp_unknown,
    &d_debug_device,
    &d_file_sys_device,
    &d_factory_reset,
};



//--------------------------------------------
// configOption
//--------------------------------------------




void configOption::init(const opt_desc_t *desc)
{
	m_opt_desc			= desc;
	m_enabled			= 1;
    m_selected			= 0;
	m_display_value		= -2;
    m_display_selected	= -1;
	m_display_enabled	= -1;

    m_value = desc->pref_offset >= 0 ? desc->type & OPTION_16BIT ?
		readPref16(desc->pref_offset) :
		readPref8 (desc->pref_offset) : -1;

	display(dbg_opts+1,"init(%s) offset=%d  m_value=%d",getTitle(),desc->pref_offset,m_value);
}


uint16_t configOption::getMin() const
{
	return m_opt_desc->type & OPTION_16BIT ?
		readPref16Min(m_opt_desc->pref_offset) :
		readPref8Min (m_opt_desc->pref_offset);
}
uint16_t configOption::getMax() const
{
	return m_opt_desc->type & OPTION_16BIT ?
		readPref16Max(m_opt_desc->pref_offset) :
		readPref8Max (m_opt_desc->pref_offset);
}


configOption *configOption::getParent()	const
{
	if (m_opt_desc->p_parent)
	{
		for (int i=0; i<NUM_CONFIG_OPTIONS; i++)
		{
			if (options[i].m_opt_desc == m_opt_desc->p_parent)
				return &options[i];
		}
	}
	return 0;
}

configOption *configOption::getNext() const
{
	const opt_desc_t *parent = m_opt_desc->p_parent;
	int num =  m_opt_desc->num + 1;
	while (parent && num < NUM_CONFIG_OPTIONS)
	{
		if (options[num].m_opt_desc->p_parent == parent)
			return &options[num];
		num++;
	}
	return 0;
}


configOption *configOption::getPrev() const
{
	const opt_desc_t *parent = m_opt_desc->p_parent;
	int num =  m_opt_desc->num - 1;
	while (parent && num > 0)
	{
		if (options[num].m_opt_desc->p_parent == parent)
			return &options[num];
		num--;
	}
	return 0;
}

configOption *configOption::getFirstChild() const
{
	int num =  m_opt_desc->num + 1;
	while (num < NUM_CONFIG_OPTIONS)
	{
		if (options[num].m_opt_desc->p_parent == m_opt_desc)
			return &options[num];
		num++;
	}
	return 0;
}


int configOption::getNumChildren() const
{
	int count = 0;
	int num =  m_opt_desc->num + 1;
	while (num < NUM_CONFIG_OPTIONS)
	{
		if (options[num].m_opt_desc->p_parent == m_opt_desc)
			count++;
		num++;
	}
	return count;
}



void  configOption::setValue(int i)
    // enforces min/max
{
    if (m_opt_desc->pref_offset >= 0)
    {
		uint16_t min = getMin();
		uint16_t max = getMax();

		display(dbg_opts,"setValue(%s,%d) m_value(%d min(%d) max(%d)",
			getTitle(),i,m_value,min,max);

        if (i > max) i = max;
        if (i < min) i = min;

		m_value = i;

		if (m_opt_desc->type & OPTION_16BIT)
			writePref16(m_opt_desc->pref_offset,i,m_opt_desc->title);
		else
			writePref8(m_opt_desc->pref_offset,i,m_opt_desc->title);

        if (m_opt_desc->setter)
            (m_opt_desc->setter)(i);
        // if (m_opt_desc->type & OPTION_NEEDS_REBOOT)
        //    s_reboot_needed = 1;
    }
}


void configOption::incValue()
	// with wrapping (really only needed in forward direction)
{
	uint16_t min = getMin();
	uint16_t max = getMax();

	int new_value = m_value+1;
	if (new_value > max)
		new_value = min;
	if (new_value < min)
		new_value = max;

	display(dbg_opts,"incValue(%s) m_value(%d)  min(%d) max(%d) new_value(%d)",
		getTitle(),m_value,min,max,new_value);
	setValue(new_value);
}



int configOption::getValue() const
{
    if (m_opt_desc->pref_offset >= 0)
         return m_opt_desc->type & OPTION_16BIT ?
            readPref16(m_opt_desc->pref_offset,m_opt_desc->title) :
            readPref8 (m_opt_desc->pref_offset,m_opt_desc->title);
    return 0;
}


const char *configOption::getValueString() const
{
    if (m_opt_desc->pref_offset >= 0)
    {
        int val = getValue();
        const char **strings = m_opt_desc->strings;
        if (strings)
            return strings[val];
        else
        {
            static char buf[12];
            sprintf(buf,"%d",val);
            return buf;
        }
    }
    return "";
}


//--------------------------------------------
// configOptions
//--------------------------------------------


configOptions::configOptions()
{
}

void configOptions::init()
{
	display(0,"DEBUG_DEVICE=%d",prefs.DEBUG_DEVICE);
	display(0,"FILE_SYS_DEVICE=%d",prefs.FILE_SYS_DEVICE);
	for (int i=0; i<NUM_CONFIG_OPTIONS; i++)
	{
		options[i].init(all_opts[i]);
	}
}


configOption *configOptions::getOption(int num)
{
	if (num >= NUM_CONFIG_OPTIONS)
	{
		my_error("request for configOption(%d) NUM_CONFIG_OPTIONS=%d",num,NUM_CONFIG_OPTIONS);
		return 0;
	}
	return &options[num];
}