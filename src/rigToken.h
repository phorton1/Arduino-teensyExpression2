//-------------------------------------------------------
// rigToken.h
//-------------------------------------------------------
// Defines the rigTokens available in a Rig File

#pragma once

#define MAX_RIG_TOKEN 	79

typedef struct
{
	int 	 id;
	uint32_t offset;
	int 	 line_num;
	int 	 char_num;
	int 	 int_value;
	char 	 text[MAX_RIG_TOKEN+1];
} token_t;


extern bool openRigFile(const char *name);
extern void closeRigFile();
extern int getRigToken();
extern void rewindRigFile(uint32_t offset, int line_num, int char_num);


extern const char *rigTokenToString(int token_id);				// for debugging and UC comparison
extern const char *rigTokenToText(int token_id);				// for text output


#define PARSE_SECTION_NONE      0
#define PARSE_SECTION_INIT		1
#define PARSE_SECTION_UPDATE	2
#define PARSE_SECTION_BUTTONS	3
#define PARSE_SECTION_END		4

extern int parse_section;

extern token_t rig_token;		// the current token now includes the file offset


//---------------------------------------------------------------
// Rig Error Reporting differs depending upon when it is found
//---------------------------------------------------------------
// They are normally reported through a dialog while parsing
// and executing statements, but are not reported while
// evaluating button sections.


extern bool suppress_rig_dialogs;

extern void rig_error(const char *format, ...);					// parse error reporting with token line_num and char_num
extern bool rig_error_found;	// true if any rig_errors occured; cleared in parseRig::init_parse()



//------------------------------
// Macros
//------------------------------

#define IS_INIT_HEADER_STATEMENT(id) \
	(id >= RIG_TOKEN_DEFINE_DEF && id <= RIG_TOKEN_STRING_DEF)
	// differentiates 'real' statements from those that are parsed into the rig

#define IS_STATEMENT(id) ( \
	IS_INIT_HEADER_STATEMENT(id) || \
	(id >= RIG_TOKEN_LISTEN && id <= RIG_TOKEN_FTP_SENSITIVITY))
	// used in the parseStatementList loop

#define IS_INIT_STATEMENT(id) ( \
	IS_INIT_HEADER_STATEMENT(id) || \
	(id >= RIG_TOKEN_LISTEN && id <= RIG_TOKEN_ALL_NOTES_OFF))
	// used to determine if an init statement list shold be parsed

#define IS_INIT_ONLY_STATEMENT(id) \
	(id >= RIG_TOKEN_LISTEN && id <= RIG_TOKEN_METER)
	// used to limit statements by section in rigToken

#define IS_BUTTON_STATEMENT(id)  \
	(id >= RIG_TOKEN_PEDAL && id <= RIG_TOKEN_FTP_SENSITIVITY)
	// used to limit statements by section in rigToken
	// and to make sure that there is at least one statement in a button section

#define IS_BUTTON_ONLY_STATEMENT(id)  \
	(id >= RIG_TOKEN_FTP_TUNER && id <= RIG_TOKEN_FTP_SENSITIVITY)
	// used to limit statements by section in rigToken

#define IS_MIDI_PORT(id)		(id >= RIG_TOKEN_USB1 && id <= RIG_TOKEN_SERIAL)

#define IS_LED_COLOR(id)		(id >= RIG_TOKEN_LED_BLACK && id <= RIG_TOKEN_LED_CYAN)
#define RIG_LED_COLOR(id)		(id - RIG_TOKEN_LED_BLACK)

#define IS_DISPLAY_COLOR(id)	(id >= RIG_TOKEN_DISPLAY_BLACK && id <= RIG_TOKEN_DISPLAY_PINK)
#define RIG_DISPLAY_COLOR(id)	(id - RIG_TOKEN_DISPLAY_BLACK)

#define IS_BIN_OP(id)			(id >= RIG_TOKEN_PLUS && id <= RIG_TOKEN_LOGICAL_AND)
#define BIN_OP(id)				(id - RIG_TOKEN_PLUS)

#define IS_SUBSECTION(id)  		(id >= RIG_TOKEN_CLICK && id <= RIG_TOKEN_REPEAT)



//------------------------------------------
// tokens
//------------------------------------------
// add token
//		BUTTON_NUM
// add section
//		onUpdate
// add statments
//		setButtonColor
//		setButtonBlink
//      Meter (derivative of Area)
//      setMeter
//	    displayNumber



#define RIG_TOKEN_EOF					0

#define RIG_TOKEN_BASERIG				1	// "BaseRig"	// identifier range start
#define RIG_TOKEN_MODAL					2	// "ModalRig"
#define RIG_TOKEN_ON_UPDATE				3	// "onUpdate"

#define RIG_TOKEN_DEFINE_DEF			4	// "define"			// start init_only statement range
#define RIG_TOKEN_STRING_DEF			5	// "define_string"	// end init_only statement range

#define RIG_TOKEN_BUTTON				6	// "BUTTON"
#define RIG_TOKEN_CLICK					7	// "click"
#define RIG_TOKEN_LONG					8	// "long"
#define RIG_TOKEN_PRESS					9	// "press"		// start subsections with statements range
#define RIG_TOKEN_REPEAT				10	// "repeat"		// end subsections range
#define RIG_TOKEN_RELEASE				11	// "release"

// Statements

#define RIG_TOKEN_LISTEN				12	// "LISTEN"			// start init_only statement range (LISTEN not allowed in modalRigs)
#define RIG_TOKEN_AREA					13	// "Area"
#define RIG_TOKEN_METER					14	// "Meter"			// end init_only statement range
#define RIG_TOKEN_PEDAL					15	// "PEDAL"
#define RIG_TOKEN_ROTARY				16	// "ROTARY"
#define RIG_TOKEN_SETVALUE				17	// "setValue"
#define RIG_TOKEN_DISPLAY 				18	// "display"
#define RIG_TOKEN_DISPLAY_NUMBER 		19	// "displayNumber"
#define RIG_TOKEN_SET_METER				20	// "setMeter'
#define RIG_TOKEN_SEND_CC 				21	// "sendCC"
#define RIG_TOKEN_SEND_PGM_CHG			22	// "sendPgmChg"
#define RIG_TOKEN_NOTE_ON				23	// "noteOn"
#define RIG_TOKEN_NOTE_OFF				24	// "noteOff"
#define RIG_TOKEN_ALL_NOTES_OFF			25	// "allNotesOff"	// end init statement range
#define RIG_TOKEN_SET_BUTTON_COLOR		26	// "setButtonColor"
#define RIG_TOKEN_SET_BUTTON_BLINK		27	// "setButtonBlink"
#define RIG_TOKEN_LOAD_RIG				28	// "loadRig"
#define RIG_TOKEN_END_MODAL				29	// "endModal"
#define RIG_TOKEN_FTP_TUNER				30	// "ftpTuner"
#define RIG_TOKEN_FTP_SENSITIVITY		31	// "ftpSensitivity"	// end statement range

// LED COLORS

#define RIG_TOKEN_LED_BLACK  			32	// start led_color range
#define RIG_TOKEN_LED_RED    			33
#define RIG_TOKEN_LED_GREEN  			34
#define RIG_TOKEN_LED_BLUE   			35
#define RIG_TOKEN_LED_YELLOW 			36
#define RIG_TOKEN_LED_PURPLE 			37
#define RIG_TOKEN_LED_ORANGE 			38
#define RIG_TOKEN_LED_WHITE  			39
#define RIG_TOKEN_LED_CYAN   			40	// end led_color range

// DISPLAY COLORS

#define RIG_TOKEN_DISPLAY_BLACK     	41	// start display_color range
#define RIG_TOKEN_DISPLAY_BLUE      	42
#define RIG_TOKEN_DISPLAY_RED       	43
#define RIG_TOKEN_DISPLAY_GREEN     	44
#define RIG_TOKEN_DISPLAY_CYAN      	45
#define RIG_TOKEN_DISPLAY_MAGENTA   	46
#define RIG_TOKEN_DISPLAY_YELLOW    	47
#define RIG_TOKEN_DISPLAY_WHITE     	48
#define RIG_TOKEN_DISPLAY_NAVY      	49
#define RIG_TOKEN_DISPLAY_DARKGREEN 	50
#define RIG_TOKEN_DISPLAY_DARKCYAN  	51
#define RIG_TOKEN_DISPLAY_MAROON    	52
#define RIG_TOKEN_DISPLAY_PURPLE    	53
#define RIG_TOKEN_DISPLAY_OLIVE     	54
#define RIG_TOKEN_DISPLAY_LIGHTGREY 	55
#define RIG_TOKEN_DISPLAY_DARKGREY  	56
#define RIG_TOKEN_DISPLAY_ORANGE    	57
#define RIG_TOKEN_DISPLAY_GREENYELLOW 	58
#define RIG_TOKEN_DISPLAY_PINK			59	// end display_color range

// midi ports

#define RIG_TOKEN_USB1					60	// in code gen order
#define RIG_TOKEN_USB2					61
#define RIG_TOKEN_USB3					62
#define RIG_TOKEN_USB4					63
#define RIG_TOKEN_HOST1					64
#define RIG_TOKEN_HOST2					65
#define RIG_TOKEN_SERIAL				66

// font decoration

#define RIG_TOKEN_NORMAL         		67	// in code gen order
#define RIG_TOKEN_BOLD         			68

// font justification

#define RIG_TOKEN_LEFT					69	// in code gen order
#define RIG_TOKEN_CENTER				70
#define RIG_TOKEN_RIGHT					71

// Expression Tokens

#define RIG_TOKEN_STRING				72	// "STRING"
#define RIG_TOKEN_VALUE					73	// "VALUE"

// The special button tokens

#define RIG_TOKEN_BUTTON_NUM			74	// "INHERIT"
#define RIG_TOKEN_INHERIT				75	// "_BUTTON_NUM"

// Meter Types

#define RIG_TOKEN_HORZ					76	// "HORZ"
#define RIG_TOKEN_VERT					77	// "VERT"

#define LAST_RIG_IDENTIFIER				77	// end of identifier range

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
