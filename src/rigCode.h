//-------------------------------------------------------------------
// rigCode.h
//-------------------------------------------------------------------
// Contains the definition of the binary image of a Rig in memory
// A Rig is limited to 32 or maybe 64K.
// Currently nothing is packed.
// All Rigs are Overlays
// One Rig is the Base Rig
// Only Base Rigs contain Listens and Values
// Loading a Base Rig resets the entire state of the machine.
// Overlays are MODAL and can be stacked upto 16 deep, limited by available RAM.
// Strings, StatementLists, Statements and Expressions are stored in pools.

#pragma once

#include "defines.h"
#include "rigToken.h"	// for NUM_SUBSECTIONS

#define RIG_NUM_AREAS			16
#define RIG_NUM_LISTENS			128
#define RIG_NUM_VALUES			128
#define RIG_NUM_STRINGS			128
	// Listens and Values are limited to this number of items

#define MAX_STATEMENTS			1024
#define MAX_STATEMENT_LISTS		(NUM_BUTTONS * 6)
#define MAX_EXPRESSIONS     	1024
	// An areas is either completely filled with a single text value,
	// or may be a vertical or horizontal progress bar

// Rig Fonts can be of the following sizes and/or bold:
// 	12, 14, 16, 18, 20, 24, 28, 32, 40, 48

#define RIG_FONT_TYPE_NORMAL	0
#define RIG_FONT_TYPE_BOLD		1

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


#define RIG_MIDI_PORT_MIDI		0
#define RIG_MIDI_PORT_SERIAL	1

typedef struct
{
	char name[8];
	uint8_t port;
	uint8_t channel;
	uint8_t cc;
} rigPedal_t;


typedef struct
{
	uint8_t     active;		// 1 if this is an active listen
	uint8_t 	port;		// MIDI, SERIAL
	uint8_t		channel;	// 1..15
	uint8_t		cc;			// 0.127
} rigListen_t;	// 4 bytes



#define OVERLAY_TYPE_RIG			0
#define OVERLAY_TYPE_OVERLAY		1


typedef struct
{
	uint16_t	overlay_type;			// OVERLAY_TYPE_DEFINES
	uint16_t	length;					// size of the entire memory footprit, including pools
	uint16_t	string_pool;			// offset from the beginning of this struct to the strings
	uint16_t	statement_pool;			// offset from the beginning of this struct to statement list pool
	uint16_t	expression_pool;		// offset from the beginning of this struct to the expression pool

	rigPedal_t  pedals[NUM_PEDALS];		// 32 bytes
	rigArea_t	areas[RIG_NUM_AREAS];	// 160 bytes
	uint16_t	button_refs[NUM_BUTTONS][NUM_SUBSECTIONS];	// 320 bytes
	rigListen_t	listens[RIG_NUM_LISTENS];	// 512 bytes
	uint16_t    strings[RIG_NUM_STRINGS];

	// about 1K bytes upto here
	// values only exist in base rigs

	uint8_t			values[RIG_NUM_VALUES];	// 128 bytes

} rigHeader_t;	// less than 1.5K bytes


// button_refs are 1 based indexes into statement lists
// and 1 based offsets into the expression pool, so that
// we can identify used buttons.



//----------------------------------------
// memory map
//----------------------------------------
//
//  Overlay or Base Rig Header
//
// String Pool
//      NULL terminated string[0]
//      NULL terminated string[1]
//      ....
//
//  Statement (List) Pool
//      Statements only occur in lists
//      But are individually accessible for editing
//		Statement list 0 is always present, but may be empty
//      All indexes and offset are 16 bits
//
//		See rigOps.h for a rigorous definition of statement structures
//
//		[ statement_pool_length]
//		[ num_statements ]
//      [ num_statement_lists ]
//
//      [ statement_list_0 ]		// 16 bit indexes into the statements
//      [ num_statements_0 ]		// number of statements in list
//		....
//      [ statement_N ]
//      [ num_statements_N]
//
//      [ statment_0 ]				// offset to statement 0
//		...
//		[ statement_N ]				// ofset to statement n
//
//
//  Expression Pool
//
//      expression references in code and button color/blink sections
//      are an offset into the expression pool if the high order bit
//      is set. Otherwise, the high order byte is one of the EXP types
//		defined in rigExpression.h, and information is stored in
//      the low order byte.
//
//
//      an expression in the expression pool has
//
//		[ expression pool length ]
//      [num_expressions]
//      [expresion_offset_0]		// offset to expression 0
//		...
//      [expresion_offset_N]		// offset to expression N
//
//
