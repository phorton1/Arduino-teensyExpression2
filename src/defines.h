//-----------------------------------------------
// defines.h
//-----------------------------------------------
// highest level H file in teensyExpression

#pragma once

#include <Arduino.h>

#define TEENSY_EXPRESSION_VERSION  	"v2.0"

#define MAX_RIG_NAME   		31

#define	DEFAULT_RIG_NAME	"rpiLooper"
#define DEFAULT_MODAL_NAME	"clipVolumes"
#define DEFAULT_RIG_TOKEN	"System_Default"
#define DEFAULT_MODAL_TOKEN	"default_modal"
	// the default rig names can be confusing, and their behavior is spread out:
	//
	// 1. in the rigMachine if the 'name' of the top of the stack
	//    is the same as the DEFAULT_TOKEN, we show the DEFAULT_NAME
	// 2. in parsing rigs, if the DEFAULT_TOKEN is passed in,
	//    or we have have a SYSTEM base_rig and the DEFAULT_MODAL_TOKEN
	//    is passed in, then we load the default for either from memory
	//    rather than parsing.
	// 3. The prefs default is "System_Default" cuz that
	//    looks nice in the winConfig system.
	// 4. This is where it could be confusing.
	//    We invariantly create H files during parse if the
	//    actual filename is "default" or "default_modal".
	//    It is intentional that "default_modal" is both
	//    the name of a loadable rig file AND the default
	//    modal token. This allows me to use it in the
	//    default.rig FILE for loadRig("default_modal"),
	//    and (if from a file), the default_modal.rig file
	//    is loaded, but if that same loadRig("default_modal") is
	//    called from the in-memory base rig, then
	//    the in memory default_modal_rig gets loaded.
	//
	// As it stands, if a user loads a file called "default.rig",
	// or "default_modal.rig", it will take longer ... as they will
	// also write over the H files on the SD card .. and that file
	// called "default_modal.rig" will show up as "clipVolumes" as
	// the title.  They can also have files called "rpiLooper.rig"
	// and "clipVolumes.rig", which are not really special names as
	// far as the system is concerned, but will further confuse them.
	// In all cases, the system will act correctly and consistently,
	// even it is confusing to the user!
	//
	// The recommendation is
	//
	//   Don't use the names default.rig, default_modal.rig,
	//   rpiLooper.rig, or clipVolumes.rig for custom rig files!
	//   And then I could have a system option to "dump H files",
	//   which would default to OFF, and there would be no extra
	//   penalty if they wanted to try to load the default.rig file.

#define SERIAL_DEVICE	   Serial3
	// the serial port to use for our 'Serial' device
	// will be Serial2 on Teensy 4.1


extern void mem_check(const char *where = 0);
	// in mem_check.cpp
extern bool legalFilename(const char *name);
	// in rigParser.cpp


// basics

#define NUM_BUTTON_COLS     5
#define NUM_BUTTON_ROWS     5
#define NUM_BUTTONS         (NUM_BUTTON_COLS * NUM_BUTTON_ROWS)

#define THE_SYSTEM_BUTTON   4

#define NUM_PEDALS          4
#define NUM_ROTARY          4

#define MAX_PEDAL_NAME		7		// also used for rotaries
#define MIDI_MAX_VALUE		127

// midi ports

#define MIDI_PORT_USB1      0x00
#define MIDI_PORT_USB2      0x10
#define MIDI_PORT_USB3      0x20
#define MIDI_PORT_USB4      0x30
#define MIDI_PORT_HOST1     0x40
#define MIDI_PORT_HOST2     0x50
#define MIDI_PORT_SERIAL    0x60

#define NUM_MIDI_PORTS      7
#define MAX_MIDI_PORT		(NUM_MIDI_PORTS-1)

#define MIDI_PORT_NUM_MASK    0xf0

#define MIDI_ENUM_TO_PORT(i)  (i) << 4
#define MIDI_PORT_TO_ENUM(p)  (p) >> 4

// one based definitions of MIDI_CHANNELS

#define MIDI_MAX_CHANNEL		16
#define MIDI_MIN_CHANNEL		1
#define MIDI_OMNI_CHANNEL		0
	// Midi messages MUST be sent on a valid midi channel 1-16, which
	// 		will be 0..15 or'd into the sent message.
	// The special value of 0 is only available for Listens, which means
	// to listen on any channel.

// Activity indicators

#define NUM_ACTIVITY_INDICATORS		6
	// these are paira for any USB, any HOST, and the SERIAL port.

#define ACTIVITY_INDICATOR_SERIAL_IN    0
#define ACTIVITY_INDICATOR_SERIAL_OUT   1
#define ACTIVITY_INDICATOR_HOST_IN		2
#define ACTIVITY_INDICATOR_HOST_OUT		3
#define ACTIVITY_INDICATOR_USB_IN		4
#define ACTIVITY_INDICATOR_USB_OUT		5


// Midi

#define MIDI_TYPE_NOTE_OFF		0x08
#define MIDI_TYPE_NOTE_ON		0x09
#define MIDI_TYPE_CC			0x0B
#define MIDI_TYPE_PGM_CHG		0x0C

// FTP

#define NUM_FTP_STRINGS   6


//----------------------------------------------------------------------
// PIN USAGE
//----------------------------------------------------------------------
// On Teensy 3.6 SERIAL is SERIAL3
// On Teensy 4.1 SERIAL is SERIAL2

// left/right
//   pin
//    GND
// L  0        unused1 RX1 used by LEDS
// L  1        unused2 TX1 used by LEDS
// L  2        ROTARY_1A
// L  3        ROTARY_1B
// L  4        ROTARY_2A
// L  5        LEDS_OUT (Serial1)
// L  6        ROTARY_2B
// L  7        SERIAL_RX
// L  8        SERIAL_TX
// L  9        ROTARY_3A
// L 10        ROTARY_3B
// L 11        ROTARY_4A
// L 12        ROTARY_4B
//   3.3V
// L 24        BUTTON_OUT0
// L 25        BUTTON_OUT1
// L 26        BUTTON_OUT2
// L 27        BUTTON_OUT3
// L 28        BUTTON_OUT4
// L 29        BUTTON_IN0
// L 30        BUTTON_IN1
// L 31 A12    BUTTON_IN2
// L 32 A13    BUTTON_IN3
//      5V
//      AGND
//      GND
// R 23 A9     EXPR1
// R 22 A8     EXPR2
// R 21 A7     EXPR3
// R 20 A6     EXPR4
// R 19 A5     TFT_DATA7
// R 18 A4     TFT_RESET
// R 17 A3     TFT_CS
// R 16 A2     TFT_CD(RS)
// R 15 A1     TFT_WR
// R 14 A0     TFT_RD
// R 13        TFT_DATA6
//      3,3V
// R    A22    x - unused analog only;
// R    A21    x - unused analog only;
// R 39 A20    TFT_DATA5
// R 38 A19    TFT_DATA4
// R 37 A18    TFT_DATA3
// R 36 A17    TFT_DATA2
// R 35 A16    TFT_DATA1
// R 34 A15    TFT_DATA0
// R 33 A14    BUTTON_IN4


//============================================================
// pin definitions and connectors
//============================================================
// The serial cable connector, 3 pin Dupon on front of PCB, pins are facing.
//      On Teensy 3.6 SERIAL is SERIAL3
//      On Teensy 4.1 SERIAL is SERIAL2
// In terms of the teensy's viewpoint of RX/TX,
// which map to Tip, Ring, and Sleeve the opposite
// of the Looper's.

// SERIAL_TX    pin 1		// Tip on TE
// SERIAL_RX	pin 2		// Ring on TE
// GND          pin 3		// Sleeve on both


//--------------------------------------------
// Button in and out pins
//--------------------------------------------
// 1st and 2nd 5 pin connectors on back in FACING order
// when seen from front of board.  Note that a single
// 15 pin connector on the board is used for 3 5 pin plugs.

#define PIN_BUTTON_IN0      29      // pin 5 on 1st connector facing
#define PIN_BUTTON_IN1      30      // pin 4
#define PIN_BUTTON_IN2      31      // pin 3
#define PIN_BUTTON_IN3      32      // pin 2
#define PIN_BUTTON_IN4      33      // pin 1

#define PIN_BUTTON_OUT4     28      // pin 1 on 2nd connector facing
#define PIN_BUTTON_OUT3     27      // pin 2
#define PIN_BUTTON_OUT2     26      // pin 3
#define PIN_BUTTON_OUT1     25      // pin 4
#define PIN_BUTTON_OUT0     24      // pin 5


//--------------------------------------------------------
// Expression pedal connector, 6 pin dupont on front, facing
//--------------------------------------------------------
// I added 10K resistors to the 1/4" pedal jacks to
// the insertion switch to ground them when not in use.
// New PCB also adds 10K pulldowns to each as well as a
// 1K inline power resistor to prevent reboots when inserting
// expression pedal plugs, resulting in an effective range of
// about 0..(0.75 * 1023)

#define PIN_EXPR4    20     // pin1 A6
#define PIN_EXPR3    21     // pin2 A7
#define PIN_EXPR2    22     // pin3 A8
#define PIN_EXPR1    23     // pin4 A9
                            // pin5 3.3V
                            // pin6 GND

//---------------------------------------
// Rotary Pins and connectors
//---------------------------------------
// 3rd and 4th 5 pin connectors on back in FACING order
// The kicad files show the NEW rotary board setup.

#if 0		// NEW ROTARY BOARD DESIGN

								// pin 1 3V on 3rd connector facing
	#define ROTARY_4B   12      // pin 2
	#define ROTARY_4A   11      // pin 3
	#define ROTARY_3B   10      // pin 4
	#define ROTARY_3A   9       // pin 5

								// pin 1 GND on 4th connector facing
	#define ROTARY_2B   6       // pin 2
	#define ROTARY_2A   4       // pin 3
	#define ROTARY_1B   3       // pin 4
	#define ROTARY_1A   2       // pin 5

#else		// old rotary perf board

	// these definitions work with the old rotary perf board

								// pin 1 3V on 3rd connector facing
	#define ROTARY_1B   12      // pin 2   // was 4
	#define ROTARY_1A   11      // pin 3
	#define ROTARY_2B   10      // pin 4
	#define ROTARY_2A   9       // pin 5

								// pin 1 GND on 4th connector facing
	#define ROTARY_3B   6       // pin 2	// was 2
	#define ROTARY_3A   4       // pin 3
	#define ROTARY_4A   3       // pin 4	// was 1 reversed
	#define ROTARY_4B   2       // pin 5

#endif


//-----------------------------------------------
// UNO style parallel ILI9486 320x480 display
//-----------------------------------------------
// The TFT DATA is on the 1st front 8 pin JST connector, facing

#define TFT_DATA0   34     // pin 1
#define TFT_DATA1   35     // pin 2
#define TFT_DATA2	36     // pin 3
#define TFT_DATA3   37     // pin 4
#define TFT_DATA4   38     // pin 5
#define TFT_DATA5   39     // pin 6
#define TFT_DATA6   13     // pin 7
#define TFT_DATA7   19     // pin 8


// The TFT controls and power are on the 2nd front 8 pin JST connector, facing
//
// GND              			// pin 1
#define TFT_RD     		14      // pin 2
#define TFT_WR          15 		// pin 3
#define TFT_CD_RS		16 		// pin 4
#define TFT_CS          17 		// pin 5
#define TFT_RESET       18 		// pin 6
// 5V               			// pin 7
// 3.3V             			// pin 8

// Touch Screen Redefinitions

#define YP  TFT_CD_RS   // 16=A2        // A2 maps to LCD_RS on arduino    // must be an analog pin, use "An" notation!
#define XM  TFT_CS      // 17=A3        // A3 maps to LCD_CS on arduino    // must be an analog pin, use "An" notation!
#define YM  TFT_DATA0   // 34=A15        / 8  maps to LCD_D0 on arduino    // can be a digital pin
#define XP  TFT_DATA1   // 35=A16       // 9  maps to LCD_D1 on arduino    // can be a digital pin


//===========================================================
// ansi colors
//===========================================================

#define ansi_color_black 	            30
#define ansi_color_red 	     	        31
#define ansi_color_green 	            32
#define ansi_color_brown 	 	        33
#define ansi_color_blue 	            34
#define ansi_color_magenta 	 	        35
#define ansi_color_cyan 	            36
#define ansi_color_light_grey 	        37

#define ansi_color_grey  	            90
#define ansi_color_light_red 	        91
#define ansi_color_light_green 	        92
#define ansi_color_yellow 		        93
#define ansi_color_light_blue  	        94
#define ansi_color_light_magenta        95
#define ansi_color_light_cyan 	        96
#define ansi_color_white  		        97

#define ansi_color_bg_black 	        40
#define ansi_color_bg_red 	     	    41
#define ansi_color_bg_green 	        42
#define ansi_color_bg_brown 	 	    43
#define ansi_color_bg_blue 	            44
#define ansi_color_bg_magenta 	 	    45
#define ansi_color_bg_cyan 	            46
#define ansi_color_bg_light_grey 	    47

#define ansi_color_bg_grey  	        100
#define ansi_color_bg_light_red 	    101
#define ansi_color_bg_light_green 	    102
#define ansi_color_bg_yellow 		    103
#define ansi_color_bg_light_blue  	    104
#define ansi_color_bg_light_magenta     105
#define ansi_color_bg_light_cyan 	    106
#define ansi_color_bg_white  		    107






//===========================================================
// int_rect
//===========================================================

class int_rect
{
public:

    int_rect()
    {
        xs = 0;
        ys = 0;
        xe = 0;
        ye = 0;
    }

    int_rect(int ixs, int iys, int ixe, int iye)
    {
        xs = ixs;
        ys = iys;
        xe = ixe;
        ye = iye;
    }

    void assign(int ixs, int iys, int ixe, int iye)
    {
        xs = ixs;
        ys = iys;
        xe = ixe;
        ye = iye;
    }


    int width() { return xe-xs+1; }
    int height()  { return ye-ys+1; }

    int xs;
    int ys;
    int xe;
    int ye;
};


#if 0	// obsolete stuff

    #define BUTTON_NUM(r,c)    ((r) * NUM_BUTTON_COLS + (c))
    #define BUTTON_ROW(i)      ((i) / NUM_BUTTON_COLS)
    #define BUTTON_COL(i)      ((i) % NUM_BUTTON_COLS)

    #define PEDAL_SYNTH     0
    #define PEDAL_LOOP      1
    #define PEDAL_WAH       2
    #define PEDAL_GUITAR    3

    #define LOOPER_NUM_TRACKS_TIMES_LAYERS    (LOOPER_NUM_TRACKS * LOOPER_NUM_LAYERS)

    typedef struct
        // structure common to New and Old rig patches
    {
        int prog_num;
        const char *short_name;         // SHOULD BE 6 CHARS OR LESS
        const char *long_name;          // NOT USED IN NEW RIG
        bool mono_mode;                 // NOT USED IN OLD RIG
    }   synthPatch_t;

    // common buttons

    #define BUTTON_MOVE_UP          12
    #define BUTTON_MOVE_LEFT        16
    #define BUTTON_MOVE_RIGHT       18
    #define BUTTON_MOVE_DOWN        22
    #define BUTTON_SELECT           17

#endif



// end of defines.h
