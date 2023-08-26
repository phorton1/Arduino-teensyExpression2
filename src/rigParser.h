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

// These are fixed by architecture

#define MAX_RIG_VALUE			255		// stored in uint8's
#define MAX_DEFINE_VALUE		255		// stored in uint8's
#define MAX_STATEMENTS 			(1 + NUM_BUTTONS * NUM_SUBSECTIONS)

// These can be increased upto 255 (uint18_t)
// as needed to accomodate larger programs

#define RIG_NUM_DEFINES			128
#define RIG_NUM_VALUES			128
#define RIG_NUM_STRINGS			128

#define RIG_NUM_AREAS			16
#define RIG_NUM_LISTENS			128		// same as RIG_NUM_VALUES


// Pool Sizes

#define MAX_DEFINE_POOL			4096
#define MAX_STRING_POOL 		4096
#define MAX_STATEMENT_POOL 		4096
	// These can be increased to upto 64K

#define MAX_EXPRESSION_POOL		4096
	// Can only be increased to 32K - 1 as it is both one based
	// and the high order bit of offsets are used for inline expressions



typedef struct
{
	uint16_t overlay_type;			// BASERIG,OVERLAY
	uint16_t num_statements;

	uint16_t define_pool_len;
	uint16_t string_pool_len;
	uint16_t statement_pool_len;
	uint16_t expression_pool_len;

	uint16_t define_ids[RIG_NUM_DEFINES+1];
	uint8_t  define_values[RIG_NUM_DEFINES+1];
	uint16_t statements[MAX_STATEMENTS + 1];

	uint16_t	button_refs[NUM_BUTTONS][NUM_SUBSECTIONS];
		// button_refs are 1 based indexes into statement lists
		// and 1 based offsets into the expression pool, so that
		// we can identify used buttons.
	uint16_t    strings[RIG_NUM_STRINGS];


} rigHeader_t;


typedef struct
{
	char 	define_pool[MAX_DEFINE_POOL];
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


// init_header only params

#define PARAM_DEFINE_NUM	 1
#define PARAM_USER_IDENT	 2
#define PARAM_DEFINE_VALUE	 3

#define PARAM_STRING_NUM     5
#define PARAM_TEXT    		 6		// also used for pedal name

// area statement

#define PARAM_AREA_NUM       10		// also used for display area
#define PARAM_FONT_SIZE      11
#define PARAM_FONT_TYPE      12
#define PARAM_FONT_JUST		 13
#define PARAM_START_X        14
#define PARAM_START_Y        15
#define PARAM_END_X          16
#define PARAM_END_Y          17

// general statements

#define PARAM_PEDAL_NUM      20
#define PARAM_PEDAL_NAME     21
#define PARAM_ROTARY_NUM	 22

#define PARAM_VALUE_NUM      30		// LISTEN and setValue
#define PARAM_VALUE			 31		// setValue value

#define PARAM_MIDI_PORT      40
#define PARAM_MIDI_CHANNEL   41
#define PARAM_LISTEN_CHANNEL 42
#define PARAM_MIDI_CC        43
#define PARAM_MIDI_VALUE	 44

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


extern bool parseRig(const char *rig_name);
extern const statement_param_t *findParams(int tt);
extern const char *argTypeToString(int i);

// parse time (for expressions)

extern rigHeader_t rig_header;
extern rigCode_t rig_code;


// runtime (for dumping & execution)

extern const rigHeader_t	*cur_rig_header;
extern const rigCode_t		*cur_rig_code;

// end of rigParser.h
