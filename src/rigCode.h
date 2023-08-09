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
// Areas, Strings, StatementLists, and Expressions are stored in pools.
// There can be upto 16 Screen Areas;
// There can be upto 32K Strings, Statements, or Expression, limited by program size;
// The high order bit of offsets are reserved to indicate a constant
// There can be upto (NUM_BUTTONS * 4) + 1 StatementLists;
// Statements are in order in Statement Lists.
// Areas that change are re-drawn from the base-rig UP through any modal overlays
//    unless the Overlay defines the NO_PARENT_DRAW bit

#pragma once

#include "defines.h"

#define RIG_NUM_SLOTS			128
	// Listens and Values are limited to this number of items
#define RIG_MAX_VALUE			127
	// The maximum Value that can be stored
#define RIG_MAX_AREAS			16
	// An areas is either completely filled with a single text value,
	// or may be a vertical or horizontal progress bar

// Rig Fonts can be of the following sizes and/or bold:
// 	12, 14, 16, 18, 20, 24, 28, 32, 40, 48

#define AREA_TYPE_UNUSED		0
#define AREA_TYPE_TEXT			1
#define AREA_TYPE_PROG_VERT		2
#define AREA_TYPE_PROG_HORZ		3

typedef struct
{
	uint8_t		area_type;		// AREA_TYPE enumerated defines
	int16_t		xs;
	int16_t		ys;
	int16_t		xe;
	int16_t		ye;
} rigArea_t;	// 10 bytes


#define LISTEN_PORT_NONE 		0x00
#define LISTEN_PORT_MIDI		0x01
#define LISTEN_PORT_SERIAL		0x02


typedef struct
{
	uint8_t 	listen_port;		// LISTEN_PORT bitwise defines
	uint8_t		in_channel;			// 15 channels, bitwise
	uint8_t		in_cc;				// 0.127
} rigListen_t;	// 4 bytes

typedef struct
{
	uint8_t		is_defined;			// currently 0 or 1 if defined by this overlay
	uint16_t 	color_expression;	// index into expression pool
	uint16_t	blink_expression;	// index into expression pool
} rigLed_t;		// 6 bytes


#define BUTTON_BIT_DEFINED    0x0001
#define BUTTON_PRESS_REPEATS  0x0002

typedef struct
{
	uint16_t	button_bits;		// BUTTON_BIT bitwise defines
	uint16_t	press_statements;	// statement list index
	uint16_t	click_statements;	// statement list index
	uint16_t	long_statements;	// statement list index
	uint16_t	release_statements;	// statement list index
} rigButton_t;	// 10 bytes


// Overlay bits are reserved in case there are different types
// of overlays, return values, etc.

#define OVERLAY_TYPE_RESERVED		0x0000
#define OVERLAY_TYPE_OVERLAY		0x0001
#define OVERLAY_TYPE_RIG			0x0002
#define OVERLAY_TYPE_NO_DRAW_PARENT 0x0004


typedef struct
{
	uint16_t	overlay_type;			// bitwise OVERLAY_TYPE_DEFINES
	uint16_t	length;					// size of the entire memory footprit, including pools
	uint16_t	string_pool;			// offset from the beginning of this struct to the strings
	uint16_t	statement_pool;			// offset from the beginning of this struct to statement list pool
	uint16_t	expression_pool;		// offset from the beginning of this struct to the expression pool

	rigArea_t	areas[RIG_MAX_AREAS];	// 160 bytes
	rigLed_t	leds[NUM_BUTTONS];		// 150 bytes
	rigButton_t	buttons[NUM_BUTTONS];	// 250 bytes

	// about 512 bytes upto here
	// the following only exist in base rigs

	rigListen_t		listens[RIG_NUM_SLOTS];	// 512 bytes
	uint8_t			values[RIG_NUM_SLOTS];	// 128 bytes

} rigHeader_t;	// less than 1.5K bytes





//----------------------------------------
// memory map
//----------------------------------------
//
//  Overlay or Base Rig Header
//
// String Pool
//		[ string pool length]	16 bits
//		[ num_strings ]			16 bits
//      [ string offset 0]		16 bits
//      ...
//      [ string offset N]		16 bits
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
//		See rigOps.h for a rigorous definition of expression structures
//
//		[ expression pool length ]
//      [num_expressions]
//      [expresion_offset_0]		// offset to expression 0
//		...
//      [expresion_offset_N]		// offset to expression N
//
//
