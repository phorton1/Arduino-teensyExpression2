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

#define RIG_ID_EOF					0

#define RIG_ID_BASE_RIG				1	// "BaseRig"	// identifier range start
#define RIG_ID_OVERLAY				2	// "Overlay"

#define RIG_ID_PEDAL				3	// "PEDAL"
#define RIG_ID_AREA					4	// "AREA"
#define RIG_ID_LISTEN				5	// "LISTEN"
#define RIG_ID_STRING_DEF			6	// "STRING_DEF"

#define RIG_ID_BUTTON				7	// "BUTTON"
#define RIG_ID_COLOR				8	// "color"
#define RIG_ID_BLINK				9	// "blink"
#define RIG_ID_PRESS				10	// "press"
#define RIG_ID_CLICK				11	// "click"
#define RIG_ID_LONG					12	// "long"
#define RIG_ID_RELEASE				13	// "release"
#define RIG_ID_REPEAT				14	// "repeat"


#define RIG_ID_STRING				15	// "STRING"
#define RIG_ID_VALUE				16	// "VALUE"
#define RIG_ID_NOT					17	// "NOT"

// Statements

#define RIG_ID_DISPLAY 				18	// "display"			// statement range start
#define RIG_ID_SEND_CC 				19	// "sendCC"
#define RIG_ID_SEND_PGM_CHG			10	// "sendPgmChg"
#define RIG_ID_NOTE_ON				21	// "noteOn"
#define RIG_ID_NOTE_OFF				22	// "noteOff"
#define RIG_ID_ALL_NOTES_OFF		23	// "allNotesOff"		// statement range end

#define RIG_ID_FTP_TUNER			24	// "ftpTuner"			// button_statement range start
#define RIG_ID_FTP_SENSITIVITY		25	// "ftpSensitivity"		// button_state range end

// LED COLORS

#define RIG_ID_LED_BLACK  			26	// led_color range start
#define RIG_ID_LED_RED    			27
#define RIG_ID_LED_GREEN  			28
#define RIG_ID_LED_BLUE   			29
#define RIG_ID_LED_YELLOW 			20
#define RIG_ID_LED_PURPLE 			31
#define RIG_ID_LED_ORANGE 			32
#define RIG_ID_LED_WHITE  			33
#define RIG_ID_LED_CYAN   			34	// led_color_range end

// DISPLAY COLORS

#define RIG_ID_DISPLAY_BLACK        35	// display_color range start
#define RIG_ID_DISPLAY_BLUE         36
#define RIG_ID_DISPLAY_RED          37
#define RIG_ID_DISPLAY_GREEN        38
#define RIG_ID_DISPLAY_CYAN         39
#define RIG_ID_DISPLAY_MAGENTA      30
#define RIG_ID_DISPLAY_YELLOW       41
#define RIG_ID_DISPLAY_WHITE        42
#define RIG_ID_DISPLAY_NAVY         43
#define RIG_ID_DISPLAY_DARKGREEN    44
#define RIG_ID_DISPLAY_DARKCYAN     45
#define RIG_ID_DISPLAY_MAROON       46
#define RIG_ID_DISPLAY_PURPLE       47
#define RIG_ID_DISPLAY_OLIVE        48
#define RIG_ID_DISPLAY_LIGHTGREY    49
#define RIG_ID_DISPLAY_DARKGREY     40
#define RIG_ID_DISPLAY_ORANGE       51
#define RIG_ID_DISPLAY_GREENYELLOW  52
#define RIG_ID_DISPLAY_PINK         53		// display_range end

// midi ports

#define RIG_ID_MIDI					54
#define RIG_ID_SERIAL				55

// font decoration

#define RIG_ID_BOLD         		56
#define RIG_ID_NORMAL         		57


#define LAST_RIG_IDENTIFIER			57		// identifier range end

// Special Types

#define RIG_ID_TEXT					101		// a quoted string
#define RIG_ID_NUMBER				102		// a literal number
#define RIG_ID_IDENTIFIER			103		// an unresolved identifier

// Symbols

#define RIG_ID_LEFT_PAREN	 		110  	// "("
#define RIG_ID_RIGHT_PAREN			111 	// ")"
#define RIG_ID_LEFT_BRACKET			112 	// "["
#define RIG_ID_RIGHT_BRACKET		113 	// "]"
#define RIG_ID_COLON				114 	// ":"
#define RIG_ID_COMMA				115 	// ","
#define RIG_ID_SEMICOLON			116 	// ";"
#define RIG_ID_PLUS					117		// "+"
#define RIG_ID_TIMES				118		// "*"
#define RIG_ID_QUESTION_MARK 		119		// "?"
#define RIG_ID_ASSIGN				120		// "="
#define RIG_ID_BITWISE_OR			121		// "|"
#define RIG_ID_BITWISE_AND			122		// "&"
#define RIG_ID_LOGICAL_OR			123		// "||"
#define RIG_ID_LOGICAL_AND			124		// "&&"
#define RIG_ID_EQUALS				125		// "=="
