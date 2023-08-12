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

#define EXPRESSION_INLINE			0x8000


// opcodes

#define EXP_NUMBER			0x01		// expression is a Number (8 bit constant) 0..127 result
#define EXP_LED_COLOR		0x02		// expression is a LED color (8 bit color number 0..10 or whatever)
#define EXP_DISPLAY_COLOR	0x03		// expression is a DISPLAY color (8 bit display color number 0..30 or whatever)
#define EXP_VALUE			0x04		// expression VALUE[value]
#define EXP_STRING			0x05		// expression STRING[value]

//-----------------------------------
// methods
//-----------------------------------

extern uint8_t *rigNumericExpression(int tt);
extern uint8_t *rigStringExpression(int tt);
extern uint8_t *rigLedColorExpression(int tt);
extern uint8_t *rigDisplayColorExpression(int tt);


// end of rigExpression.h
