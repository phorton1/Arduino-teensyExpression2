//-------------------------------------------
// configOptions.h
//-------------------------------------------
// contains const description of config options tree
// for use by configsystem

#pragma once

#define NUM_CONFIG_OPTIONS		77
	// prh 2025-03-08 changed from 78 to 77

// values which must agree with table setup

#define BRIGHTNESS_OPT_NUM		1
#define PEDAL0_OPT_NUM			10

// OPTION FLAGS - these are set as the ID of the dialog.

#define OPTION_LOAD_RIG			0x0100
	// Brings up the winFileDialog dialog and returns
	// zero or OPTION_LOAD_RIG. if OPTION_LOAD_RIG the
	// new rig name is gotten from the winFileDlg and parses it.
	// Note that only base_rigs can be loaded directly by the user.
#define OPTION_DUMP_H_FILE		0x0200
	// Brings up the winFileDialog dialog and returns
	// zero or OPTION_DUMP_H_FILES. if OPTION_LOAD_RIG the
	// new rig name is gotten from the winFileDlg and it is dumped

#if 0	// removed from compilation 2025-03-08 while normalizing
		// fileSystem to TE3
	#define OPTION_FORMAT_SD		0x2000
		// brings up the formatSDDialog, which either returns
		// control to the configSystem, or reformats the SD
		// card and reboots.
#endif

#define OPTION_SPOOF_FTP		0x4000
	// The spoof_ftp value is or'd into the bottom nibble,
	// if if this bit is set when the yesNoDialog returns,
	// the pref is set, they all are saved, and we reboot
#define OPTION_FACTORY_RESET	0x8000
	// Is a yesNoDialog, so it returns 0 or OPTION_FACTORY_RESET.
	// If OPTION_FACTORY_RESET is returned, the preferences are reset
	// and we reboot

#define DEBUG_DEVICE_NAME	"Debug Device"
	// for matching option titles in winConfig

typedef void (*setter_fxn)(int i);

typedef struct opt_desc
{
    uint16_t        num;
	const char		*title;
	int				pref_offset;		// offset in preferences for calling read/writePref; -1 is no pref
	uint16_t		type;				// bitwise type from above
	const opt_desc	*p_parent;			// pointer to parent
	const char		**strings;			// the allowable strings, may be null
	setter_fxn		setter;				// setter function called onChange()

} opt_desc_t;


class winConfig;;
    // forward


//-------------------------------------------
// configOption
//-------------------------------------------

class configOption
{
    public:

        configOption() 	{}

        void init(const opt_desc_t *desc);

        int          getNum() 		const	{ return m_opt_desc->num; }
        const char   *getTitle() 	const	{ return m_opt_desc->title; }
        uint16_t     getType() 		const	{ return m_opt_desc->type; }
        const char   **getStrings() const	{ return m_opt_desc->strings; }
        setter_fxn   getSetterFxn() const	{ return m_opt_desc->setter; }

		uint16_t	getMin() 		const;
		uint16_t	getMax()		const;

        configOption *getParent()	const;
        configOption *getNext()		const;
        configOption *getPrev()		const;
        configOption *getFirstChild() const;
		int getNumChildren() const;

        bool  hasValue()			const	{ return m_opt_desc->pref_offset >= 0; }
		bool  needsValueDisplay()	const	{ return m_display_value != getValue(); }
        void  clearDisplayValue()     		{ m_display_value = getValue(); }

		void enable(int enabled)	  		{ m_enabled = enabled; }
		int  getEnabled()			const	{ return m_enabled; }
		bool enabledChanged() 		  		{ bool retval = (m_display_enabled != m_enabled); m_display_enabled = m_enabled; return retval; }
		int  getSelected()			const	{ return m_selected; }
		void select(int selected)	  		{ m_selected = selected; }
		bool selectedChanged()		  		{ bool retval = (m_display_selected != m_selected); m_display_selected = m_selected; return retval; }

        int   getValue() const;
		void  setValue(int i);        		// enforces min/max
        void  incValue();  					// inc by 1 with wrap
        const char *getValueString() const;


    private:

        const opt_desc_t *m_opt_desc;

        int m_value;

        int m_selected;
		int m_enabled;
		int m_display_value;
        int m_display_selected;
        int m_display_enabled;



};


//-------------------------------------------
// configOption
//-------------------------------------------

class configOptions
{
	public:

		configOptions();

		void init();

		configOption *getOption(int num);
};


extern configOptions the_options;




// end of configOptions.h
