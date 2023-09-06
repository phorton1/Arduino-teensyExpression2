//-------------------------------------------------------
// rigExpression.h
//-------------------------------------------------------

// need to add LEFT RIGHT CENTER tokens
// need to add MINUS DIVIDE NE GT GE LT LE tokens

#pragma once

#include "rigParser.h"


#define MAX_EXP_STACK   16

// NumericExpression =
//		NUMBER
//      NOT NumericExpression
//		( NumericExpression )
//      NumericExpresion BIN_OP NumericExpression
//      NumericExpresion ? NumericExpression : NumericExpression
// StringExpression
//		"text"
//		STRING[ NumericExpression ]
//      ( StringExpression )
//		NumericExpression ? StringExpression : StringExpression
// ColorExpression
//		COLOR
//		( ColorExpression )
//		NumericExpression ? ColorExpression : ColorExpression
// BIN_OP  + - * / == != > >= < <= & && | ||

// Some Useful Constants

#define EXP_RIG_TYPE_BASERIG	0
#define EXP_RIG_TYPE_OVERLAY	1


//-------------------------------------------------
// Inline Expressions
//-------------------------------------------------
// if the high order bit of an expression reference is set
// then the rest of the high order byte defines an inline
// compacted sub-expression as given below, and the low
// order bye contains the info

#define EXP_INLINE			0x80

// opcodes

#define EXP_NUMBER			0x01		// expression is a Number - inline only - (8 bit constant) 0..255 result
#define EXP_LED_COLOR		0x02		// expression is a LED color (8 bit color number 0..NUM_LED_COLORS-1
#define EXP_DISPLAY_COLOR	0x03		// expression is a DISPLAY color (8 bit display color 0..NUM_DISPLAY_COLORS
#define EXP_VALUE			0x04		// expression VALUE[value]
#define EXP_STRING			0x05		// expression STRING[value]

#define EXP_TEXT			0x08		// expression is an inline null terminated string
#define EXP_RIGHT_BRACKET   0x09

// these are in the same order as their token numbers

#define EXP_LEFT_PAREN		0x0E
#define EXP_RIGHT_PAREN		0x0F
#define EXP_QUESTION 		0x10
#define EXP_COLON			0x11

#define EXP_NOT				0x12
#define EXP_PLUS			0x13
#define EXP_MINUS			0x14
#define EXP_TIMES			0x15
#define EXP_DIVIDE			0x16
#define EXP_EQ				0x17
#define EXP_NE				0x18
#define EXP_GT				0x19
#define EXP_GE				0x1A
#define EXP_LT				0x1B
#define EXP_LE				0x1C
#define EXP_BITWISE_OR		0x1D
#define EXP_BITWISE_AND		0x1E
#define EXP_LOGICAL_OR		0x1F
#define EXP_LOGICAL_AND		0x20

#define EXP_END				0xFF

//-----------------------------------
// methods
//-----------------------------------

extern uint16_t rigNumericExpression(rig_t *rig);

extern uint16_t rigPedalNumExpression(rig_t *rig);
extern uint16_t rigRotaryNumExpression(rig_t *rig);
extern uint16_t rigListenNumExpression(rig_t *rig);
extern uint16_t rigAreaNumExpression(rig_t *rig);
extern uint16_t rigValueNumExpression(rig_t *rig);

extern uint16_t rigValueExpression(rig_t *rig);
extern uint16_t rigMidiChannelExpression(rig_t *rig);
extern uint16_t rigListenChannelExpression(rig_t *rig);
extern uint16_t rigMidiValueExpression(rig_t *rig);

extern uint16_t rigStringExpression(rig_t *rig);
extern uint16_t rigLedColorExpression(rig_t *rig);
extern uint16_t rigDisplayColorExpression(rig_t *rig);


// end of rigExpression.h
