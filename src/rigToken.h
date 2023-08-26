//-------------------------------------------------------
// rigToken.h
//-------------------------------------------------------
// Defines the rigTokens available in a Rig File

#pragma once

#define MAX_RIG_TOKEN 	79


typedef struct
{
	int id;
	int type;

	int len;
	int line_num;
	int char_num;

	int int_value;
	char text[MAX_RIG_TOKEN+1];

} token_t;


extern bool openRigFile(const char *name);
extern int getRigToken();
extern void closeRigFile();

extern const char *rigTokenToString(int token_id);				// for debugging and UC comparison
extern const char *rigTokenToText(int token_id);				// for text output
extern void rig_error(const char *format, ...);					// parse error reporting with token line_num and char_num


extern int parse_section;		// 0=none, 1=program, 2=buttons, 3=end
extern token_t rig_token;		// the current token
extern bool rig_error_found;	// true if any rig_errors occured; cleared in parseRig::init_parse()



//------------------------------
// Macros
//------------------------------

#define IS_INIT_HEADER_STATEMENT(id) \
	(id >= RIG_TOKEN_DEFINE_DEF && id <= RIG_TOKEN_STRING_DEF)
	// differentiates 'real' statements from those that are parsed into the rig

#define IS_STATEMENT(id) ( \
	IS_INIT_HEADER_STATEMENT(id) || \
	(id >= RIG_TOKEN_AREA && id <= RIG_TOKEN_FTP_SENSITIVITY))
	// used in the parseStatementList loop

#define IS_INIT_STATEMENT(id) ( \
	IS_INIT_HEADER_STATEMENT(id) || \
	(id >= RIG_TOKEN_AREA && id <= RIG_TOKEN_ALL_NOTES_OFF))
	// used to determine if an init statement list shold be parsed

#define IS_INIT_ONLY_STATEMENT(id) \
	(id >= RIG_TOKEN_AREA && id <= RIG_TOKEN_LISTEN)
	// used to limit statements by section in rigToken

#define IS_BUTTON_STATEMENT(id)  \
	(id >= RIG_TOKEN_PEDAL && id <= RIG_TOKEN_FTP_SENSITIVITY)
	// used to limit statements by section in rigToken
	// and to make sure that there is at least one statement in a button section

#define IS_BUTTON_ONLY_STATEMENT(id)  \
	(id >= RIG_TOKEN_FTP_TUNER && id <= RIG_TOKEN_FTP_SENSITIVITY)
	// used to limit statements by section in rigToken

#define IS_MIDI_PORT(id)	(id >= RIG_TOKEN_USB1 && id <= RIG_TOKEN_SERIAL)

#define IS_LED_COLOR(id)	(id >= RIG_TOKEN_LED_BLACK && id <= RIG_TOKEN_LED_CYAN)
#define LED_COLOR(id)		(id - RIG_TOKEN_LED_BLACK)

#define IS_DISPLAY_COLOR(id)	(id >= RIG_TOKEN_DISPLAY_BLACK && id <= RIG_TOKEN_DISPLAY_PINK)
#define DISPLAY_COLOR(id)		(id - RIG_TOKEN_DISPLAY_BLACK)

#define IS_BIN_OP(id)		(id >= RIG_TOKEN_PLUS && id <= RIG_TOKEN_LOGICAL_AND)
#define BIN_OP(id)			(id - RIG_TOKEN_PLUS)


#define IS_SUBSECTION(id)  (id >= RIG_TOKEN_COLOR && id <= RIG_TOKEN_REPEAT)
	// for any of the ids

#define NUM_SUBSECTIONS ((RIG_TOKEN_REPEAT - RIG_TOKEN_COLOR) + 1)
	// for all of the ids

#define SUBSECTION_NUM(id) (id - RIG_TOKEN_COLOR)
	// for the id within the range

#define SUBSECTION_FIRST_CODE (RIG_TOKEN_PRESS - RIG_TOKEN_COLOR)
#define SUBSECTION_NUM_CODE   ((RIG_TOKEN_REPEAT - RIG_TOKEN_PRESS) + 1)



//------------------------------------------
// tokens
//------------------------------------------


#define RIG_TOKEN_EOF					0

#define RIG_TOKEN_BASERIG				1	// "BaseRig"	// identifier range start
#define RIG_TOKEN_OVERLAY				2	// "Overlay"

#define RIG_TOKEN_DEFINE_DEF			3	// "define"			// start init_header range
#define RIG_TOKEN_STRING_DEF			4	// "define_string"	// end init_header range

#define RIG_TOKEN_BUTTON				5	// "BUTTON"
#define RIG_TOKEN_COLOR					6	// "color"		// start subsections range
#define RIG_TOKEN_BLINK					7	// "blink"		// end subsection_with_expression range
#define RIG_TOKEN_PRESS					8	// "press"		// start subsections with statements range
#define RIG_TOKEN_CLICK					9	// "click"
#define RIG_TOKEN_LONG					10	// "long"
#define RIG_TOKEN_RELEASE				11	// "release"
#define RIG_TOKEN_REPEAT				12	// "repeat"		// end subsections range

// Statements

#define RIG_TOKEN_AREA					13	// "AREA"			// start init_only statement range
#define RIG_TOKEN_LISTEN				14	// "LISTEN"			// end init_only statement range
#define RIG_TOKEN_PEDAL					15	// "PEDAL"
#define RIG_TOKEN_ROTARY				16	// "ROTARY"
#define RIG_TOKEN_SETVALUE				17	// "setValue"
#define RIG_TOKEN_DISPLAY 				18	// "display"
#define RIG_TOKEN_SEND_CC 				19	// "sendCC"
#define RIG_TOKEN_SEND_PGM_CHG			20	// "sendPgmChg"
#define RIG_TOKEN_NOTE_ON				21	// "noteOn"
#define RIG_TOKEN_NOTE_OFF				22	// "noteOff"
#define RIG_TOKEN_ALL_NOTES_OFF			23	// "allNotesOff"	// end init statement range
#define RIG_TOKEN_FTP_TUNER				24	// "ftpTuner"
#define RIG_TOKEN_FTP_SENSITIVITY		25	// "ftpSensitivity"	// end statement range

// LED COLORS

#define RIG_TOKEN_LED_BLACK  			26	// start led_color range
#define RIG_TOKEN_LED_RED    			27
#define RIG_TOKEN_LED_GREEN  			28
#define RIG_TOKEN_LED_BLUE   			29
#define RIG_TOKEN_LED_YELLOW 			30
#define RIG_TOKEN_LED_PURPLE 			31
#define RIG_TOKEN_LED_ORANGE 			32
#define RIG_TOKEN_LED_WHITE  			33
#define RIG_TOKEN_LED_CYAN   			34	// end led_color range

// DISPLAY COLORS

#define RIG_TOKEN_DISPLAY_BLACK     	35	// start display_color range
#define RIG_TOKEN_DISPLAY_BLUE      	36
#define RIG_TOKEN_DISPLAY_RED       	37
#define RIG_TOKEN_DISPLAY_GREEN     	38
#define RIG_TOKEN_DISPLAY_CYAN      	39
#define RIG_TOKEN_DISPLAY_MAGENTA   	40
#define RIG_TOKEN_DISPLAY_YELLOW    	41
#define RIG_TOKEN_DISPLAY_WHITE     	42
#define RIG_TOKEN_DISPLAY_NAVY      	43
#define RIG_TOKEN_DISPLAY_DARKGREEN 	44
#define RIG_TOKEN_DISPLAY_DARKCYAN  	45
#define RIG_TOKEN_DISPLAY_MAROON    	46
#define RIG_TOKEN_DISPLAY_PURPLE    	47
#define RIG_TOKEN_DISPLAY_OLIVE     	48
#define RIG_TOKEN_DISPLAY_LIGHTGREY 	49
#define RIG_TOKEN_DISPLAY_DARKGREY  	50
#define RIG_TOKEN_DISPLAY_ORANGE    	51
#define RIG_TOKEN_DISPLAY_GREENYELLOW 	52
#define RIG_TOKEN_DISPLAY_PINK			53	// end display_color range

// midi ports

#define RIG_TOKEN_USB1					54	// in code gen order
#define RIG_TOKEN_USB2					55
#define RIG_TOKEN_USB3					56
#define RIG_TOKEN_USB4					57
#define RIG_TOKEN_HOST1					58
#define RIG_TOKEN_HOST2					59
#define RIG_TOKEN_SERIAL				60

// font decoration

#define RIG_TOKEN_NORMAL         		61	// in code gen order
#define RIG_TOKEN_BOLD         			62

// font justification

#define RIG_TOKEN_LEFT					63	// in code gen order
#define RIG_TOKEN_CENTER				64
#define RIG_TOKEN_RIGHT					65

// Expression Tokens

#define RIG_TOKEN_STRING				66	// "STRING"
#define RIG_TOKEN_VALUE					67	// "VALUE"

#define LAST_RIG_IDENTIFIER				67	// end of identifier range

// Literal Types

#define RIG_TOKEN_TEXT					101	// a quoted string
#define RIG_TOKEN_NUMBER				102	// a literal number
#define RIG_TOKEN_IDENTIFIER			103	// user defined identifier

// Symbols

#define RIG_TOKEN_COMMA					110 // ","
#define RIG_TOKEN_SEMICOLON				111 // ";"
#define RIG_TOKEN_LEFT_BRACKET			112 // "["
#define RIG_TOKEN_RIGHT_BRACKET			113 // "]"
#define RIG_TOKEN_LEFT_PAREN	 		118 // "("		// ranged in eval for display
#define RIG_TOKEN_RIGHT_PAREN			119 // ")"
#define RIG_TOKEN_QUESTION 				120	// "?"

#define RIG_TOKEN_COLON					121 // ":"		// ranged in eval for OPs
#define RIG_TOKEN_NOT					122 // "!"
#define RIG_TOKEN_PLUS					123	// "+"		// ranged in expressions
#define RIG_TOKEN_MINUS					124 // "-"
#define RIG_TOKEN_TIMES					125 // "*"
#define RIG_TOKEN_DIVIDE				126	// "/"
#define RIG_TOKEN_EQ					127	// "=="
#define RIG_TOKEN_NE					128	// "!="
#define RIG_TOKEN_GT					129	// ">"
#define RIG_TOKEN_GE					130	// ">="
#define RIG_TOKEN_LT					131	// "<"
#define RIG_TOKEN_LE					132	// "<="
#define RIG_TOKEN_BITWISE_OR			133	// "|"
#define RIG_TOKEN_BITWISE_AND			134	// "&"
#define RIG_TOKEN_LOGICAL_OR			136	// "||"
#define RIG_TOKEN_LOGICAL_AND			137	// "&&"

#define RIG_TOKEN_ASSIGN				201 	// "="  	// transient


// binop range
