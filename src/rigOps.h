//-------------------------------------------------------
// rigOps.h
//-------------------------------------------------------
// Contains the definition of opcodes used in Statements and Expressions

#pragma once


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


#define EXP_TYPE_NONE				0x00		// illegal expression type
#define EXP_TYPE_NUMBER				0x01		// expression is a Number (8 bit constant) 0..127 result
#define EXP_TYPE_STRING				0x02		// expression is a String (16 bit string index)
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


//-----------------------------------------------------
// Statements
//-----------------------------------------------------
// Statements consist of an opcode a number of expression arguments

#define RIG_OP_STATEMENT_ASSIGN 			0x01
	// EXP_TYPE_VALUE		lvalue			REFERENCES alllowed
	// EXP_TYPE_NUMBER		rvalue			REFERENCES alllowed
#define RIG_OP_STATEMENT_DISPLAY 			0x02
	// EXP_TYPE_AREA		display_area	CONSTANT only
	// EXP_TYPE_FONT		font_size		CONSTANT only
	// EXP_TYPE_NUMBER		bold			REFERENCES allowed; non-zero is bold
	// EXP_TYPE_DISP_COLOR  color			REFERENCES allowed
	// EXP_TYPE_STRING		string			REFERENCES allowed
#define RIG_OP_STATEMENT_SEND_CC 			0x10
	// EXP_TYPE_PORT		midi_port		CONSTANT only
	// EXP_TYPE_CHANNEL		midi_channel	CONSTANT only
	// EXP_TYPE_CC			cc_numvwe		CONSTANT only
	// EXP_TYPE_NUMBER		value			REFERENCES alllowe
#define RIG_OP_STATEMENT_SEND_PGM_CHF		0x11
	// EXP_TYPE_PORT		midi_port		CONSTANT only
	// EXP_TYPE_CHANNEL		midi_channel	CONSTANT only
	// EXP_TYPE_NUMBER		patch_num		REFERENCES alllowe
#define RIG_OP_STATEMENT_NOTE_ON			0x12
	// EXP_TYPE_PORT		midi_port		CONSTANT only
	// EXP_TYPE_CHANNEL		midi_channel	CONSTANT only
	// EXP_TYPE_NUMBER		note_num		REFERENCES alllowe
	// EXP_TYPE_NUMBER		velocity		REFERENCES alllowe
#define RIG_OP_STATEMENT_NOTE_OFF			0x13
	// EXP_TYPE_PORT		midi_port		CONSTANT only
	// EXP_TYPE_CHANNEL		midi_channel	CONSTANT only
	// EXP_TYPE_NUMBER		note_num		REFERENCES alllowe
#define RIG_OP_STATEMENT_ALL_NOTES_OFF		0x14
	// EXP_TYPE_PORT		midi_port		CONSTANT only
	// EXP_TYPE_CHANNEL		midi_channel	CONSTANT only
#define RIG_OP_STATEMENT_FTP_TUNER			0x40
#define RIG_OP_STATEMENT_FTP_SENSITIVITY	0x41
	// no arguments


//-------------------------------------------------
// Tokens
//-------------------------------------------------

#define RIG_TOKEN_BASE_RIG		"BaseRig"
#define RIG_TOKEN_OVERLAY		"Overlay"

#define RIG_TOKEN_LISTEN		"LISTEN"
#define RIG_TOKEN_STRING		"STRING"

#define RIG_TOKEN_LED			"LED"
#define RIG_TOKEN_COLOR			"color"
#define RIG_TOKEN_BLINK			"blink"

#define RIG_TOKEN_BUTTON		"BUTTON"
#define RIG_TOKEN_PRESS			"press"
#define RIG_TOKEN_CLICK			"click"
#define RIG_TOKEN_LONG			"long"
#define RIG_TOKEN_RELEASE		"release"
#define RIG_TOKEN_REPEAT		"repeat"

#define RIG_TOKEN_VALUE			"VALUE"
#define RIG_TOKEN_NOT			"NOT"

#define RIG_TOKEN_LEFT_PAREN	"("
#define RIG_TOKEN_RIGHT_PAREN	")"
#define RIG_TOKEN_COLON			":"
#define RIG_TOKEN_COMMA			","
#define RIG_TOKEN_SEMICOLON		";"

#define RIG_TOKEN_QUOTE			"\""

#define RIG_TOKEN_ASSIGN		"="
#define RIG_TOKEN_PLUS			"+"
#define RIG_TOKEN_TIMES			"*"
#define RIG_TOKEN_BITWISE_OR	"|"
#define RIG_TOKEN_BITWISE_AND	"&"
#define RIG_TOKEN_LOGICAL_OR	"||"
#define RIG_TOKEN_LOGICAL_AND	"&&"
#define RIG_TOKEN_EQUALS		"=="
#define RIG_TOKEN_QUESTION_MARK "?"

// following TOKEN types are generic and depend on their token values for resolution

#define RIG_TOKEN_NUMBER			"number"
#define RIG_TOKEN_LED_COLOR			"LED_COLOR"
#define RIG_TOKEN_DISPLAY_COLOR		"DISPLAY_COLOR"

// statement Tokens

#define RIG_TOKEN_DISPLAY 				"display"
#define RIG_TOKEN_SEND_CC 				"sendCC"
#define RIG_TOKEN_SEND_PGM_CHG			"sendPgmChg"
#define RIG_TOKEN_NOTE_ON				"noteOn"
#define RIG_TOKEN_NOTE_OFF				"noteOff"
#define RIG_TOKEN_ALL_NOTES_OFF			"allNotesOff"
#define RIG_TOKEN_FTP_TUNER				"ftpTuner"
#define RIG_TOKEN_FTP_SENSITIVITY		"ftpSensitivity"



