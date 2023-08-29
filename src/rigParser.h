//-------------------------------------------------------
// rigParser.h
//-------------------------------------------------------

#pragma once

#include "defines.h"
#include "rigToken.h"	// for NUM_SUBSECTIONS

// Special constants

#define BUTTON_INHERIT_FLAG		0xFFFF

#define	DEFAULT_RIG_NAME		"rpiLooper"
#define DEFAULT_MODAL_NAME		"clipVolumes"

#define RIG_TYPE_MODAL		0x0001		// rig is a modal rig
#define RIG_TYPE_SYSTEM		0x0002		// rig was loaded from constants


// These are fixed by architecture

#define MAX_RIG_VALUE			255		// stored in uint8's
#define MAX_DEFINE_VALUE		255		// stored in uint8's
#define MAX_STATEMENTS 			(1 + NUM_BUTTONS * NUM_SUBSECTIONS)

// These can be increased upto 255 (uint18_t)
// as needed to accomodate larger programs

#define RIG_NUM_AREAS			16
#define RIG_NUM_DEFINES			128
#define RIG_NUM_VALUES			128
#define RIG_NUM_STRINGS			128
#define RIG_NUM_LISTENS			128		// same as RIG_NUM_VALUES

// Pool Sizes

#define MAX_DEFINE_POOL			4096
#define MAX_STRING_POOL 		4096
#define MAX_STATEMENT_POOL 		4096
	// These can be increased to upto 64K

#define MAX_EXPRESSION_POOL		4096
	// Can only be increased to 32K - 1 as it is both one based
	// and the high order bit of offsets are used for inline expressions

#define MAX_RIG_SIZE (	\
	sizeof(rig_t) + \
	MAX_DEFINE_POOL + \
	MAX_STRING_POOL + \
	MAX_STATEMENT_POOL + \
	MAX_EXPRESSION_POOL )


typedef struct
{
	const int id;
	const int *args;
} statement_param_t;


typedef struct
{
	uint16_t rig_type;			// 0=baseRig, 1=modalRig
	uint16_t num_statements;

	uint16_t define_pool_len;
	uint16_t string_pool_len;
	uint16_t statement_pool_len;
	uint16_t expression_pool_len;

	uint16_t define_ids[RIG_NUM_DEFINES+1];
	uint8_t  define_values[RIG_NUM_DEFINES+1];
	uint16_t statements[MAX_STATEMENTS + 1];

	uint16_t button_refs[NUM_BUTTONS][NUM_SUBSECTIONS];
		// button_refs are 1 based indexes into statement lists
		// and 1 based offsets into the expression pool, so that
		// we can identify used buttons.
	uint16_t strings[RIG_NUM_STRINGS];

	const char 	*define_pool;
	const char 	*string_pool;
	const uint8_t *statement_pool;
	const uint8_t *expression_pool;

} rig_t;


//--------------------------------------------------------
// API available to the rigMachine et al:
//--------------------------------------------------------

extern const statement_param_t *findParams(int tt);
	// returns the parameter list for a given statement token
extern const char *argTypeToString(int i);
	// return a string for displaying the above paremeter types

extern const rig_t *parseRig(const char *rig_name);
	// The main entry point.
	// Allocates a temporary rig on the heap, and parses the rig.
	// if that goes well, the rig is relocated and packed into the
	// rig pool, and a pointer the rig is returned.


//----------------------------------------
// param constants
//----------------------------------------
// init_header only params

#define PARAM_DEFINE_NUM	 1
#define PARAM_USER_IDENT	 2
#define PARAM_DEFINE_VALUE	 3

#define PARAM_STRING_NUM     5
#define PARAM_TEXT    		 6		// limited to 79 in length

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
#define PARAM_PEDAL_NAME     21		// lmited to 7 in length
#define PARAM_ROTARY_NUM	 22

#define PARAM_VALUE_NUM      30		// LISTEN, setValue, and endModal
#define PARAM_VALUE			 31		// setValue value

#define PARAM_MIDI_PORT      40
#define PARAM_MIDI_CHANNEL   41
#define PARAM_LISTEN_CHANNEL 42
#define PARAM_MIDI_CC        43
#define PARAM_MIDI_VALUE	 44

#define PARAM_RIG_NAME		 50		// limited to 31 in length and valid filename characters

#define PARAM_STRING_EXPRESSION			101
#define PARAM_LED_COLOR_EXPRESSION		102
#define PARAM_DISPLAY_COLOR_EXPRESSION	103


//-----------------------------------------
// Notes
//-----------------------------------------
// There will space for at least TWO of these largest rigs,
// 		in the statically allocated rig_pool.  Additionally,
// 		one will be allocated on the heap during parsing.
// Because of the use of uint16_t offsets, a Rig is inherently limited to 64K.
// Only BaseRigs contain Listens and Values
//     	So, BY CONVENTION, The BaseRig and any child ModalRigs
//     	MUST make sure that variable slots are used correctly
//     	between parents and children.  Interestingly, children
//     	will retain value states of any private variables they use
// Loading a BaseRig resets the entire state of the machine.
// Returning from a ModalRig resets all the buttons, with inheritance, correctly,
//     	BUT Areas are not cleared (alhtough they are re-evaluated).
//     	Therefore, Areas are not properly inherited and so, therefore
//        	BY CONVENTION AREA SLOTS SHOULD NOT BE RE-USED and changed IN VARIOUS MODAL RIGS
//     	in the underlying rig.
// Overlays are MODAL and can be stacked upto 16 deep, limited by the
// 		space in the static rig_pool.
// Strings, StatementLists, Statements and Expressions are stored in pools.
// Rig Fonts can be of the following sizes and/or bold:
// 	12, 14, 16, 18, 20, 24, 28, 32, 40, 48
// Rigs are parsed onto the heap, and then relocated into the rig_pool
//      releasing the heap memory, so the parsing guarentees that the
//      rig can be as big as the below defines, but when it is relocated
//      (and packed) into the rig pool, it may not fit, depending on the
//      other rigs that have been previously loaded.
// The rig_pool is large enough to hold two max'd out rigs.

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

// end of rigParser.h
