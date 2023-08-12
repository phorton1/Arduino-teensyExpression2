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
//




//-------------------------------------------------
// Expressions
//-------------------------------------------------
// If EXP_TYPE_REFERENCE the EXP_TYPE is followed by a 16 bit offset into the expression pool
// Otherwise, the EXP_TYPE is followed by one or two bytes of constant, depending on the type
//
// the length of an expression can be determined at run time
// by evaluation.  for example, NOT expects one expression
// to follow, LEFT_PAREN expects an expression followed by RIGHT_PAREN
// binary expression delimiters expect a second operand
// and QUESTION_MARK expects an expression COLON and then another
// expression.

extern uint8_t *rigNumericExpression(int tt);
extern uint8_t *rigStringExpression(int tt);
extern uint8_t *rigLedColorExpression(int tt);
extern uint8_t *rigDisplayColorExpression(int tt);



#define EXP_TYPE_NONE				0x00		// illegal expression type
#define EXP_TYPE_NUMBER				0x01		// expression is a Number (8 bit constant) 0..127 result
#define EXP_TYPE_STRING				0x02		// expression is a String (length byte then chars, inline)
#define EXP_TYPE_VALUE				0x03		// expression is a Value  (8 bit value index 0..127)
#define EXP_TYPE_LED_COLOR			0x04		// expression is a LED color (8 bit color number 0..10 or whatever)
#define EXP_TYPE_DISP_COLOR			0x05		// expression is a DISPLAY color (8 bit display color number 0..30 or whatever)
#define EXP_TYPE_FONT				0x06		// constant 8 bit allowed font sizes
#define EXP_TYPE_AREA				0x07		// constant DISPLAY_AREA 0..15 only
#define EXP_TYPE_PORT				0x08		// constant 0 or 1 only
#define EXP_TYPE_CHANNEL			0x09		// constant 1..16 only
#define EXP_TYPE_CC					0x0A		// constant 0..127 only
#define EXP_TYPE_REFERENCE			0x80		// expression is an 16 bit offset into expression pool

// compound expressions are stored in human readable form
// and evaluated at runtime.

#define EXP_TYPE_NOT				0x10
#define EXP_TYPE_LEFT_PAREN			0x11
#define EXP_TYPE_RIGHT_PAREN		0x12

#define EXP_TYPE_RIGHT_PLUS			0x20
#define EXP_TYPE_RIGHT_MINUS		0x21
#define EXP_TYPE_BITWISE_OR         0x22
#define EXP_TYPE_BITWISE_AND        0x23
#define EXP_TYPE_LOGICAL_OR         0x24
#define EXP_TYPE_LOGICAL_AND        0x25
#define EXP_TYPE_EQUALITY			0x26

#define EXP_TYPE_QUESTION_MARK      0x30
#define EXP_TYPE_COLON      		0x31
