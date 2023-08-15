//-------------------------------------------------------
// rigToken.h
//-------------------------------------------------------
// Defines the rigTokens available in a Rig File

#pragma once

#define rig_name "test"

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

#define IS_STATEMENT(id) ( \
	(id >= RIG_TOKEN_DEFINE_DEF && id <= RIG_TOKEN_STRING_DEF) || \
	(id >= RIG_TOKEN_SETVALUE && id <= RIG_TOKEN_FTP_SENSITIVITY) )

#define IS_INIT_ONLY_STATEMENT(id) \
	(id >= RIG_TOKEN_DEFINE_DEF && id <= RIG_TOKEN_STRING_DEF)

#define IS_INIT_STATEMENT(id) ( \
	(id >= RIG_TOKEN_DEFINE_DEF && id <= RIG_TOKEN_STRING_DEF) || \
	(id >= RIG_TOKEN_SETVALUE && id <= RIG_TOKEN_ALL_NOTES_OFF) )

#define IS_BUTTON_STATEMENT(id)  \
	(id >= RIG_TOKEN_SETVALUE && id <= RIG_TOKEN_FTP_SENSITIVITY)

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

#define RIG_TOKEN_DEFINE_DEF			3	// "define"
#define RIG_TOKEN_PEDAL					4	// "PEDAL"
#define RIG_TOKEN_AREA					5	// "AREA"
#define RIG_TOKEN_LISTEN				6	// "LISTEN"
#define RIG_TOKEN_STRING_DEF			7	// "STRING_DEF"	// init_only statements end range

#define RIG_TOKEN_BUTTON				8	// "BUTTON"
#define RIG_TOKEN_COLOR					9	// "color"		// subsection start range
#define RIG_TOKEN_BLINK					10	// "blink"		// subsection_with_expression end_range
#define RIG_TOKEN_PRESS					11	// "press"		// subsections with statements tart range
#define RIG_TOKEN_CLICK					12	// "click"
#define RIG_TOKEN_LONG					13	// "long"
#define RIG_TOKEN_RELEASE				14	// "release"
#define RIG_TOKEN_REPEAT				15	// "repeat"		// subsections end

// Statements

#define RIG_TOKEN_SETVALUE				16	// "setValue"			// statement range start
#define RIG_TOKEN_DISPLAY 				17	// "display"
#define RIG_TOKEN_SEND_CC 				18	// "sendCC"
#define RIG_TOKEN_SEND_PGM_CHG			19	// "sendPgmChg"
#define RIG_TOKEN_NOTE_ON				20	// "noteOn"
#define RIG_TOKEN_NOTE_OFF				21	// "noteOff"
#define RIG_TOKEN_ALL_NOTES_OFF			22	// "allNotesOff"		// init available statement range end

#define RIG_TOKEN_FTP_TUNER				23	// "ftpTuner"
#define RIG_TOKEN_FTP_SENSITIVITY		24	// "ftpSensitivity"		// statement range end

// LED COLORS

#define RIG_TOKEN_LED_BLACK  			25	// led_color range start
#define RIG_TOKEN_LED_RED    			26
#define RIG_TOKEN_LED_GREEN  			27
#define RIG_TOKEN_LED_BLUE   			28
#define RIG_TOKEN_LED_YELLOW 			29
#define RIG_TOKEN_LED_PURPLE 			30
#define RIG_TOKEN_LED_ORANGE 			31
#define RIG_TOKEN_LED_WHITE  			32
#define RIG_TOKEN_LED_CYAN   			33	// led_color range end

// DISPLAY COLORS

#define RIG_TOKEN_DISPLAY_BLACK     	34	// display_color range start
#define RIG_TOKEN_DISPLAY_BLUE      	35
#define RIG_TOKEN_DISPLAY_RED       	36
#define RIG_TOKEN_DISPLAY_GREEN     	37
#define RIG_TOKEN_DISPLAY_CYAN      	38
#define RIG_TOKEN_DISPLAY_MAGENTA   	39
#define RIG_TOKEN_DISPLAY_YELLOW    	40
#define RIG_TOKEN_DISPLAY_WHITE     	41
#define RIG_TOKEN_DISPLAY_NAVY      	42
#define RIG_TOKEN_DISPLAY_DARKGREEN 	43
#define RIG_TOKEN_DISPLAY_DARKCYAN  	44
#define RIG_TOKEN_DISPLAY_MAROON    	45
#define RIG_TOKEN_DISPLAY_PURPLE    	46
#define RIG_TOKEN_DISPLAY_OLIVE     	47
#define RIG_TOKEN_DISPLAY_LIGHTGREY 	48
#define RIG_TOKEN_DISPLAY_DARKGREY  	49
#define RIG_TOKEN_DISPLAY_ORANGE    	50
#define RIG_TOKEN_DISPLAY_GREENYELLOW 	51
#define RIG_TOKEN_DISPLAY_PINK			52	// display_color range end

// midi ports

#define RIG_TOKEN_MIDI					53	// in code gen order
#define RIG_TOKEN_SERIAL				54

// font decoration

#define RIG_TOKEN_NORMAL         		55	// in code gen order
#define RIG_TOKEN_BOLD         			56


// font justification

#define RIG_TOKEN_LEFT					57	// in code gen order
#define RIG_TOKEN_CENTER				58
#define RIG_TOKEN_RIGHT					59

// Expression Tokens

#define RIG_TOKEN_STRING				60	// "STRING"
#define RIG_TOKEN_VALUE					61	// "VALUE"

#define LAST_RIG_IDENTIFIER				61	// end of identifier range

// Literal Types

#define RIG_TOKEN_TEXT					101	// a quoted string
#define RIG_TOKEN_NUMBER				102	// a literal number
#define RIG_TOKEN_IDENTIFIER			103	// user defined identifier

// Symbols

#define RIG_TOKEN_COMMA					110 // ","
#define RIG_TOKEN_SEMICOLON				111 // ";"
#define RIG_TOKEN_LEFT_PAREN	 		112 // "("
#define RIG_TOKEN_RIGHT_PAREN			113 // ")"
#define RIG_TOKEN_COLON					114 // ":"

#define RIG_TOKEN_LEFT_BRACKET			120 // "["
#define RIG_TOKEN_RIGHT_BRACKET			121 // "]"

#define RIG_TOKEN_NOT					122 // "!"		// ='s versions must follow!
#define RIG_TOKEN_QUESTION 				123	// "?"		// end ranged in expressions

#define RIG_TOKEN_PLUS					130	// "+"		// ranged in expressions
#define RIG_TOKEN_MINUS					131 // "-"
#define RIG_TOKEN_TIMES					132 // "*"
#define RIG_TOKEN_DIVIDE				133	// "/"
#define RIG_TOKEN_EQ					134	// "=="
#define RIG_TOKEN_NE					135	// "!="
#define RIG_TOKEN_GT					136	// ">"
#define RIG_TOKEN_GE					137	// ">="
#define RIG_TOKEN_LT					138	// "<"
#define RIG_TOKEN_LE					139	// "<="
#define RIG_TOKEN_BITWISE_OR			140	// "|"
#define RIG_TOKEN_BITWISE_AND			141	// "&"
#define RIG_TOKEN_LOGICAL_OR			142	// "||"
#define RIG_TOKEN_LOGICAL_AND			143	// "&&"

#define RIG_TOKEN_ASSIGN				201 	// "="  	// transient


// binop range
