//-------------------------------------------------------
// rigParser.h
//-------------------------------------------------------
// Contains the definition of the binary image of a Rig in memory
// A Rig is limited to 32 or maybe 64K.
// Currently nothing is packed.
// All Rigs are Overlays
// One Rig is the Base Rig
// Only Base Rigs contain Listens and Values
// Loading a Base Rig resets the entire state of the machine.
// Overlays are MODAL and can be stacked upto 16 deep, limited by available RAM.
// Strings, StatementLists, Statements and Expressions are stored in pools.
// An areas is either completely filled with a single text value,
// or may be a vertical or horizontal progress bar
// Rig Fonts can be of the following sizes and/or bold:
// 	12, 14, 16, 18, 20, 24, 28, 32, 40, 48

#pragma once

#include "defines.h"
#include "rigToken.h"	// for NUM_SUBSECTIONS

#define MIDI_MAX_CHANNEL		15
#define MIDI_MAX_VALUE			127

// This is fixed by architecture

#define MAX_STATEMENTS 			(1 + NUM_BUTTONS * NUM_SUBSECTIONS)

// These can be increased upto 255 (uint18_t)

#define RIG_NUM_AREAS			16
#define RIG_NUM_LISTENS			128
#define RIG_NUM_VALUES			128
#define RIG_NUM_STRINGS			128

// These can be increased to upto 64K (uint16_t)

#define MAX_EXPRESSIONS     	1024
#define MAX_STRING_POOL 		4096
#define MAX_STATEMENT_POOL 		4096
#define MAX_EXPRESSION_POOL		4096


typedef struct
{
	uint8_t		font_size;
	uint8_t     font_type;		// NORMAL, BOLD
	uint8_t		font_just;		// LEFT, CENTER, RIGHT
	int16_t		xs;
	int16_t		ys;
	int16_t		xe;
	int16_t		ye;
} rigArea_t;	// 10 bytes

typedef struct
{
	char name[8];
	uint8_t port;			// 0..NUM_MIDI_PORTS
	uint8_t channel;		// 0..MIDI_MAX_CHANNEL
	uint8_t cc;				// 0..MIDI_MAX_VALUE
} rigPedal_t;


typedef struct
{
	uint8_t     active;		// 1 if this is an active listen
	uint8_t 	port;		// MIDI, SERIAL, etc
	uint8_t		channel;	// 0..MIDI_MAX_CHANNEL
	uint8_t		cc;			// 0..MIDI_MAX_VALUE
} rigListen_t;


typedef struct
{
	uint16_t overlay_type;			// BASERIG,OVERLAY
	uint16_t num_statements;
	uint16_t statement_pool_len;
	uint16_t expression_pool_len;
	uint16_t string_pool_len;

	uint16_t statements[MAX_STATEMENTS + 1];

	// following can be inherited by overlays

	rigArea_t	areas[RIG_NUM_AREAS];
	uint16_t	button_refs[NUM_BUTTONS][NUM_SUBSECTIONS];
		// button_refs are 1 based indexes into statement lists
		// and 1 based offsets into the expression pool, so that
		// we can identify used buttons.
	uint16_t    strings[RIG_NUM_STRINGS];

	// Following only exist in BASE_RIGS

	rigPedal_t  pedals[NUM_PEDALS];
	uint8_t		values[RIG_NUM_VALUES];
	rigListen_t	listens[RIG_NUM_LISTENS];

} rigHeader_t;


typedef struct
{
	char 	string_pool[MAX_STRING_POOL];
	uint8_t statement_pool[MAX_STATEMENT_POOL];
	uint8_t expression_pool[MAX_EXPRESSION_POOL];
} rigCode_t;


//----------------------------------------
// params
//----------------------------------------

typedef struct
{
	const int id;
	const int *args;
} statement_param_t;


// init_only params

#define PARAM_VALUE_NUM      1

#define PARAM_PEDAL_NUM      10
#define PARAM_PEDAL_NAME     11

#define PARAM_AREA_NUM       20
#define PARAM_FONT_SIZE      21
#define PARAM_FONT_TYPE      22
#define PARAM_FONT_JUST		 23
#define PARAM_START_X        24
#define PARAM_START_Y        25
#define PARAM_END_X          26
#define PARAM_END_Y          27

#define PARAM_STRING_NUM     30
#define PARAM_TEXT    		 31

// monadic params in either

#define PARAM_MIDI_PORT      40
#define PARAM_MIDI_CHANNEL   41
#define PARAM_MIDI_CC        42

// expression params

#define PARAM_NUM_EXPRESSION			100
#define PARAM_STRING_EXPRESSION			101
#define PARAM_LED_COLOR_EXPRESSION		102
#define PARAM_DISPLAY_COLOR_EXPRESSION	103


//----------------------------------------
// memory map
//----------------------------------------
// String Pool
//      NULL terminated string[0]
//      NULL terminated string[1]
//      ....
//
//  Statement (List) Pool
//      Statements only occur in lists, so these are actually statement lists
//		Statement sizes are rigourously known by their parameter list definitions.
//
//		StatementList[0]
//		StatementList[1]
//		,,,
//
//  Expression Pool
//      expression references in code and button color/blink sections
//      are an offset into the expression pool if the high order bit
//      is not set. Otherwise, the high order byte is one of the EXP types
//		defined in rigExpression.h, and information is stored in
//      the low order byte.
//
//		Expression[0]
//		Expression[1]
//		...


extern bool parseRig();
extern const statement_param_t *findParams(int tt);

extern const rigHeader_t  *cur_rig_header;
extern const rigCode_t	*cur_rig_code;

// end of rigParser.h


