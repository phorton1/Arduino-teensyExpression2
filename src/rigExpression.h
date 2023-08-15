//-------------------------------------------------------
// rigExpression.h
//-------------------------------------------------------

// need to add LEFT RIGHT CENTER tokens
// need to add MINUS DIVIDE NE GT GE LT LE tokens

#pragma once

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


//-------------------------------------------------
// Inline Expressions
//-------------------------------------------------
// if the high order bit of an expression reference is set
// then the rest of the high order byte defines an inline
// compacted sub-expression as given below, and the low
// order bye contains the info

#define EXP_INLINE			0x80
#define EXP_INLINE_ID		0x40

// opcodes

#define EXP_NUMBER			0x01		// expression is a Number - inline only - (8 bit constant) 0..255 result
#define EXP_DEFINE			0x02		// expression is an Identifier - inline_only - 0..MAX_DEFINES
#define EXP_LED_COLOR		0x03		// expression is a LED color (8 bit color number 0..NUM_LED_COLORS-1
#define EXP_DISPLAY_COLOR	0x04		// expression is a DISPLAY color (8 bit display color 0..NUM_DISPLAY_COLORS
#define EXP_VALUE			0x05		// expression VALUE[value]
#define EXP_STRING			0x06		// expression STRING[value]

#define EXP_TEXT			0x08		// expression is an inline null terminated string

#define EXP_LEFT_PAREN		0x10
#define EXP_RIGHT_PAREN		0x11
#define EXP_RIGHT_BRACKET   0x12
#define EXP_QUESTION 		0x13
#define EXP_COLON			0x14
#define EXP_NOT				0x15

// these are in the same order as their token numbers

#define EXP_PLUS			0x21
#define EXP_MINUS			0x22
#define EXP_TIMES			0x23
#define EXP_DIVIDE			0x24
#define EXP_EQ				0x25
#define EXP_NE				0x26
#define EXP_GT				0x27
#define EXP_GE				0x28
#define EXP_LT				0x29
#define EXP_LE				0x2A
#define EXP_BITWISE_OR		0x2B
#define EXP_BITWISE_AND		0x2C
#define EXP_LOGICAL_OR		0x2D
#define EXP_LOGICAL_AND		0x2E

#define EXP_END				0xFF


#define EXP_



//-----------------------------------
// methods
//-----------------------------------

extern uint16_t rigNumericExpression(int tt);
extern uint16_t rigValueNumExpression(int tt);
extern uint16_t rigAreaNumExpression(int tt);
extern uint16_t rigMidiChannelExpression(int tt);
extern uint16_t rigMidiValueExpression(int tt);

extern uint16_t rigStringExpression(int tt);
extern uint16_t rigLedColorExpression(int tt);
extern uint16_t rigDisplayColorExpression(int tt);


// end of rigExpression.h
