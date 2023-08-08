# Analysis of existing Program

## A. Preferences

The current prefernces and configuation system is Complicated.


```
uint8_t prefs[NUM_EEPROM_USED];
uint8_t pref_cache[NUM_EEPROM_USED];
uint8_t last_prefs[NUM_EEPROM_USED];

int16_t pref_min[NUM_EEPROM_USED];
int16_t pref_max[NUM_EEPROM_USED];
const char **pref_strings[NUM_EEPROM_USED];
```

There are **three** arrays of uint8_t's for the prefs in memory.

All preferences are currently numbers, either uint8_t's with an
implict range of 0..127, or uint16_t's with an implicit range of
0..32767.  In either case one filled fields (255's) indicate
that the default value should be used.


This is because, in the current system, you can modify the prefs
in memory and NOT save them, but try them out, and then, at a later
time save them.    AND we need to keep track of instantaneous
changes WHILE editing them.

```
inline uint8_t  getPref8(int pref)      { return prefs[pref]; }
inline bool     getPrefBool(int pref)   { return (bool) prefs[pref]; }
inline uint16_t getPref16(int pref)     { uint16_t *p=(uint16_t *) &prefs[pref];  return *p; }
```

The **getPref** methods are very quick and return the current in-memory
value to be used by the system.


### Initialization

```
extern void init_global_prefs()		// prh note:  extern on function definition?!?
{
    for (int i=0; i<NUM_EEPROM_USED; i++)
    {
        prefs[i] = last_prefs[i] = pref_cache[i] = EEPROM.read(i);
    }
    setDefaultPrefs();
}
```

The init_global_prefs() method initializes all three arrays from the EE_PROM,
and then calls setDefaultPrefs(), which in turn explicitly calls
_setDefaultPref() methods for every preference.

```
static void _setDefaultPref8(int i, int min, int max, uint8_t def_val, const char *strings[]=0)
{
    if (pref_cache[i] == 255)
        prefs[i] = def_val;
    pref_min[i] = min;
    pref_max[i] = max;
    pref_strings[i] = strings;
}

static void _setDefaultPref16(int i, int min, int max, uint16_t def_val)
{
    if (pref_cache[i]==255 && pref_cache[i+1]==255)
    {
        uint16_t *p = (uint16_t *)&prefs[i];
        *p = def_val;
        pref_min[i] = min;		// prh note: this looks like a bug. These should be outside of the brackets.
        pref_max[i] = max;
    }
}
```

Preferences have an *ID* which is its *offset in EEPROM*, yech, so 16 bit prefs
take up two numbers.  Later, in the configuration system those *offsets* are
bound to individual "options" that go in the configSystem tree.

They each have a hardwired min and max.  Presumably the default is within these.

If the **cache** value is 255, the **in-memory** value is set to the default_value.

uint8_t prefs *may* have an enumerated array of strings associated with them and,
if so, the max value is passed in as the number of strings in the array minus one.


### Saving

```
// extern
void save_global_prefs()
{
    display(dbg_prefs,"save_global_prefs",0);
    for (int i=0; i<NUM_EEPROM_USED; i++)
    {
        EEPROM.write(i,pref_cache[i]);
        last_prefs[i] = pref_cache[i];
    }
}
```

Just writes the **cache** to the **EEPROM**.


### Factory Reset

Note that the Factory Reset functionality is in the configSystem (not in prefs.cpp).
It occurs when the modal confirm dialog for a factory reset terminates.


```
// virtual
void configSystem::onEndModal(expWindow *win, uint32_t param)
	// called when modal dialogs are ended
{
	if (param && win->getId() == OPTION_TYPE_FACTORY_RESET)
	{
		for (int i=0; i<NUM_EEPROM_USED; i++)
			setPref8(i,255);
		save_global_prefs();
		reboot(THE_SYSTEM_BUTTON);
	}
}
```

First it calls setPref8(i,255) for all EEPROM locations:

```
// extern
void setPref8(int pref, uint8_t val)
{
    pref_cache[pref] = prefs[pref] = val;
}
```

And then it calls save_global_prefs() to write them to the EEPROM,
and finally it calls reboot(THE_SYSTEM_BUTTON) to flash the system
button for a few seconds before rebooting.



### Modification

As we saw above, calling setPref() modifies the **pref** AND
**pref_cache** values in memory.     This will effectively
mark the prefs as *dirty* by the prefs_changed() methods:


```
// extern
bool prefs_changed()
{
    for (int i=0; i<NUM_EEPROM_USED; i++)
        if (last_prefs[i] != pref_cache[i])
            return true;
    return false;
}


// extern
bool pref_changed8(int pref)
{
    return last_prefs[pref] != pref_cache[pref];
}
```


Then, finally, there are *restore* methods to
*undo* changes made during editing:

```
// extern
void restore_prefs()
{
    display(dbg_prefs,"restore_prefs()",0);
    for (int i=0; i<NUM_EEPROM_USED; i++)
        pref_cache[i] = last_prefs[i];
    setDefaultPrefs();
}

// extern
void restore_pref8(int i)
{
    pref_cache[i] = last_prefs[i];
}
```


All in all this system is really complicated.
It is used by the configSystem, once again,
to allow preferences to be modified, used in memory,
and asynchronously saved.



## B. Configuration System

When leaving *configuration mode* a short click to the THE_SYSTEM_BUTTON
will exit, but use the existing prefs, a long press will save them, and
short press on another button, to the left of the system button, which
is amber, will abort the changes and restore the preferences.

The system button being PURPLE was supposed to be special so that you
know what mode you are in.  The reboot button is amber, even if the
prefs have not changed.   The brighntness is apparently not restored
on an 'abort'.  Abort does not work well.

It gets even more complicated when you get to the Pedal Preferences,
which may have ANOTHER two layer dirty/save function.


## C. FTP Sensitivity DOES NOT appear to be saved in Prefs

**Despite what the comments say**


The ftp.h comments says this.  Note that NUM_STRINGS should be renamed to NUM_GUITAR_STRINGS

```
extern int  ftp_sensitivity[NUM_STRINGS];            // all must be !- -1 or we are not initialized

// stored in EEPROM (separate from preferences)
// In this code I treat these as global settings.
//
// They have default values based on the default settings in the controller
// factory patch 0-0 (Polymode Patch 1).
//
// If the values in the EEPROM are different than those defaults, AND
// an FTP controller is located remotely or in the host port (as per prefs),
// the initial EEPROM settings will be sent to the controller.
//
// The public values here are READONLY and ONLY gotten from readbacks
// (replies from the controller). They are "gotten" by midiQueue.cpp,
// and stored here (in ftp.cpp) so that ftp.cpp is the central repository
// of FTP state.
```

I can find no code that writes, or reads these from EEPROM.



## D. SongMachine memory usage

Generally speaking the program uses the following amount of pre-allocated memory:

Sketch uses 184940 bytes (17%) of program storage space. Maximum is 1048576 bytes.
Global variables use 137280 bytes (52%) of dynamic memory, leaving 124864 bytes for local variables. Maximum is 262144 bytes.

However, the c++ new() function is extensively used:

- the songMachine() is newed up by rigLooper
- the various windows like winFtpTuner, winFtpSensitivity, winConfigPedal(i), are newed
- the whole configOption tree is newed up
- dialog windows like yesNoDialog are newed when needed
- the rigs are newed up as needed

Nowhere do I see any memory usage displays turned on.








### Song Names in Memory

While selecting songs, an array of pointers to names is used,
along with the teensy **new** and **free* heap memory alocators.
I assume that the heap grows from the top of some memory downwards
until it runs into the stack growing up from some location.
Note that there is no check on the result from new() below!!!

```
#define MAX_SONG_NAMES  255
char *songParser::song_names[MAX_SONG_NAMES];

void songParser::releaseSongNames()
{
    for (int i=0; i<num_song_names; i++)
    {
        free(song_names[i]);
    }
    num_song_names = 0;
    memset(song_names,0,MAX_SONG_NAMES * sizeof(char *));
}

// and for each directory entry during a directory read


	char *fn = new char[len+1];
	strncpy(fn,filename,len);
	fn[len] = 0;
	song_names[num_song_names++] = fn;
                }
```

They are sorted after reading the directory entries so that they show
up in order in the songSelection dialog.  Apart from the lack of
error checking on new(), since this is the only use of new() and free()
currently in the program, there should currently at least be no problem with
heap fragmentation.



### Song Text in memory

The text file is read entirely into memory and remains there after parsing
for displaying to the user:

```
#define MAX_SONG_TEXT   16384
char songParser::song_text[MAX_SONG_TEXT];
```


### Compiled song code in memory

The compiled song code which lives in memory is 4k

```
#define MAX_SONG_CODE   4096
uint8_t songParser::song_code[MAX_SONG_CODE];
```

### Token in memory

Of course, the current token itself is in memory.
This must be long enough for any displayed string,
reserverd word, or label.

```
#define MAX_SONG_TOKEN  80
char songParser::token[MAX_SONG_TOKEN+1] = {0};
```


### Labels and offset in memory

The songMachine uses the labels array to resolve
labels to jump to in the code.  This is a limited
use feature, there is only one usage in BigRiver.song,
and it is used for METHODS, which no song currently uses,
although every song currently uses a GOTO start statement at the end.


```
#define MAX_ID_LEN      12
#define MAX_LABELS      32
#define MAX_SONG_NAMES  255

typedef struct
{
    char name[MAX_ID_LEN+1];
    int  code_offset;
}   label_t;

label_t songParser::labels[MAX_LABELS];

```

### Relocations in memory


The relocation table is only used during parsing and *could*
probably be moved to the heap.

```
#define MAX_RELOCATIONS  255

typedef struct
{
    int offset;
    int line_num;
    int char_num;
    label_t *label;
}   relocation_t;

relocation_t songParser::relocs[MAX_RELOCATIONS];
```


