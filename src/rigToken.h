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



//------------------------------
// Macros
//------------------------------

#define IS_STATEMENT(id) ( \
	(id >= RIG_TOKEN_PEDAL && id <= RIG_TOKEN_STRING_DEF) || \
	(id >= RIG_TOKEN_SETVALUE && id <= RIG_TOKEN_FTP_SENSITIVITY) )

#define IS_INIT_ONLY_STATEMENT(id) \
	(id >= RIG_TOKEN_PEDAL && id <= RIG_TOKEN_STRING_DEF)

#define IS_INIT_STATEMENT(id) ( \
	(id >= RIG_TOKEN_PEDAL && id <= RIG_TOKEN_STRING_DEF) || \
	(id >= RIG_TOKEN_SETVALUE && id <= RIG_TOKEN_ALL_NOTES_OFF) )



#define IS_BUTTON_STATEMENT(id)  \
	(id >= RIG_TOKEN_SETVALUE && id <= RIG_TOKEN_FTP_SENSITIVITY)

#define IS_LED_COLOR(id) \
	(id >= RIG_TOKEN_LED_BLACK && id <= RIG_TOKEN_LED_CYAN);

#define IS_DISPLAY_COLOR(id) \
	(id >= RIG_TOKEN_DISPLAY_BLACK && id <= RIG_TOKEN_DISPLAY_PINK);

#define IS_BIN_OP(id) \
	(id >= RIG_TOKEN_PLUS && id <= RIG_TOKEN_EQ);



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

#define RIG_TOKEN_PEDAL					3	// "PEDAL"		// init_only statements range start
#define RIG_TOKEN_AREA					4	// "AREA"
#define RIG_TOKEN_LISTEN				5	// "LISTEN"
#define RIG_TOKEN_STRING_DEF			6	// "STRING_DEF"	// init_only statements end range

#define RIG_TOKEN_BUTTON				7	// "BUTTON"
#define RIG_TOKEN_COLOR					8	// "color"		// subsection start range
#define RIG_TOKEN_BLINK					9	// "blink"		// subsection_with_expression end_range
#define RIG_TOKEN_PRESS					10	// "press"		// subsections with statements tart range
#define RIG_TOKEN_CLICK					11	// "click"
#define RIG_TOKEN_LONG					12	// "long"
#define RIG_TOKEN_RELEASE				13	// "release"
#define RIG_TOKEN_REPEAT				14	// "repeat"		// subsections end

// Statements

#define RIG_TOKEN_SETVALUE				15	// "setValue"			// statement range start
#define RIG_TOKEN_DISPLAY 				16	// "display"
#define RIG_TOKEN_SEND_CC 				17	// "sendCC"
#define RIG_TOKEN_SEND_PGM_CHG			18	// "sendPgmChg"
#define RIG_TOKEN_NOTE_ON				19	// "noteOn"
#define RIG_TOKEN_NOTE_OFF				20	// "noteOff"
#define RIG_TOKEN_ALL_NOTES_OFF			21	// "allNotesOff"		// init available statement range end

#define RIG_TOKEN_FTP_TUNER				22	// "ftpTuner"
#define RIG_TOKEN_FTP_SENSITIVITY		23	// "ftpSensitivity"		// statement range end

// LED COLORS

#define RIG_TOKEN_LED_BLACK  			24	// led_color range start
#define RIG_TOKEN_LED_RED    			25
#define RIG_TOKEN_LED_GREEN  			26
#define RIG_TOKEN_LED_BLUE   			27
#define RIG_TOKEN_LED_YELLOW 			28
#define RIG_TOKEN_LED_PURPLE 			29
#define RIG_TOKEN_LED_ORANGE 			30
#define RIG_TOKEN_LED_WHITE  			31
#define RIG_TOKEN_LED_CYAN   			32	// led_color range end

// DISPLAY COLORS

#define RIG_TOKEN_DISPLAY_BLACK     	33	// display_color range start
#define RIG_TOKEN_DISPLAY_BLUE      	34
#define RIG_TOKEN_DISPLAY_RED       	35
#define RIG_TOKEN_DISPLAY_GREEN     	36
#define RIG_TOKEN_DISPLAY_CYAN      	37
#define RIG_TOKEN_DISPLAY_MAGENTA   	38
#define RIG_TOKEN_DISPLAY_YELLOW    	39
#define RIG_TOKEN_DISPLAY_WHITE     	40
#define RIG_TOKEN_DISPLAY_NAVY      	41
#define RIG_TOKEN_DISPLAY_DARKGREEN 	42
#define RIG_TOKEN_DISPLAY_DARKCYAN  	43
#define RIG_TOKEN_DISPLAY_MAROON    	44
#define RIG_TOKEN_DISPLAY_PURPLE    	45
#define RIG_TOKEN_DISPLAY_OLIVE     	46
#define RIG_TOKEN_DISPLAY_LIGHTGREY 	47
#define RIG_TOKEN_DISPLAY_DARKGREY  	48
#define RIG_TOKEN_DISPLAY_ORANGE    	49
#define RIG_TOKEN_DISPLAY_GREENYELLOW 	50
#define RIG_TOKEN_DISPLAY_PINK			51	// display_color range end

// midi ports

#define RIG_TOKEN_MIDI					52	// in code gen order
#define RIG_TOKEN_SERIAL				53

// font decoration

#define RIG_TOKEN_NORMAL         		54	// in code gen order
#define RIG_TOKEN_BOLD         			55


// font justification

#define RIG_TOKEN_LEFT					56	// in code gen order
#define RIG_TOKEN_CENTER				57
#define RIG_TOKEN_RIGHT					58

// Expression Tokens

#define RIG_TOKEN_STRING				59	// "STRING"
#define RIG_TOKEN_VALUE					60	// "VALUE"

#define LAST_RIG_IDENTIFIER				60	// end of identifier range

// Literal Types

#define RIG_TOKEN_TEXT					101	// a quoted string
#define RIG_TOKEN_NUMBER				102	// a literal number

// Symbols

#define RIG_TOKEN_COMMA					110 // ","
#define RIG_TOKEN_SEMICOLON				111 // ";"
#define RIG_TOKEN_LEFT_PAREN	 		112 // "("
#define RIG_TOKEN_RIGHT_PAREN			113 // ")"
#define RIG_TOKEN_COLON					114 // ":"

#define RIG_TOKEN_LEFT_BRACKET			120 // "["
#define RIG_TOKEN_RIGHT_BRACKET			121 // "]"

#define RIG_TOKEN_PLUS					130	// "+"
#define RIG_TOKEN_MINUS					131 // "-"
#define RIG_TOKEN_TIMES					132 // "*"
#define RIG_TOKEN_DIVIDE				133	// "/"
#define RIG_TOKEN_EQ					134	// "=="

#define RIG_TOKEN_NOT					140 // "!"		// ='s versions must follow!
#define RIG_TOKEN_NE					141	// "!="
#define RIG_TOKEN_GT					142	// ">"
#define RIG_TOKEN_GE					143	// ">="
#define RIG_TOKEN_LT					144	// "<"
#define RIG_TOKEN_LE					145	// "<="

#define RIG_TOKEN_BITWISE_OR			150	// "|"
#define RIG_TOKEN_BITWISE_AND			151	// "&"
#define RIG_TOKEN_LOGICAL_OR			152	// "||"
#define RIG_TOKEN_LOGICAL_AND			153	// "&&"

#define RIG_TOKEN_QUESTION_MARK 		190	// "?"

#define RIG_TOKEN_IDENTIFIER			200		// "ID"		// transient
#define RIG_TOKEN_ASSIGN				201 	// "="  	// transient


// binop range
