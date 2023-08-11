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

	bool is_init_statement;
	bool is_button_statement;
	bool is_led_color;
	bool is_display_color;
	bool is_delim;

	bool is_bin_op;

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
extern void dumpRigCode(int code_len, uint8_t *code_buf);		// dump the code buffer

extern token_t rig_token;		// the current token


//------------------------------------------
// tokens
//------------------------------------------


#define RIG_TOKEN_EOF					0

#define RIG_TOKEN_BASERIG				1	// "BaseRig"	// identifier range start
#define RIG_TOKEN_OVERLAY				2	// "Overlay"

#define RIG_TOKEN_PEDAL					3	// "PEDAL"		// range of init_only statements
#define RIG_TOKEN_AREA					4	// "AREA"
#define RIG_TOKEN_LISTEN				5	// "LISTEN"
#define RIG_TOKEN_STRING_DEF			6	// "STRING_DEF"	// end range

#define RIG_TOKEN_BUTTON				7	// "BUTTON"
#define RIG_TOKEN_COLOR					8	// "color"
#define RIG_TOKEN_BLINK					9	// "blink"
#define RIG_TOKEN_PRESS					10	// "press"
#define RIG_TOKEN_CLICK					11	// "click"
#define RIG_TOKEN_LONG					12	// "long"
#define RIG_TOKEN_RELEASE				13	// "release"
#define RIG_TOKEN_REPEAT				14	// "repeat"


#define RIG_TOKEN_STRING				15	// "STRING"
#define RIG_TOKEN_VALUE					16	// "VALUE"
#define RIG_TOKEN_NOT					17	// "NOT"

// Statements

#define RIG_TOKEN_SETVALUE				18	// "setValue"
#define RIG_TOKEN_DISPLAY 				19	// "display"			// statement range start
#define RIG_TOKEN_SEND_CC 				20	// "sendCC"
#define RIG_TOKEN_SEND_PGM_CHG			21	// "sendPgmChg"
#define RIG_TOKEN_NOTE_ON				22	// "noteOn"
#define RIG_TOKEN_NOTE_OFF				23	// "noteOff"
#define RIG_TOKEN_ALL_NOTES_OFF			24	// "allNotesOff"		// init statment range end

#define RIG_TOKEN_FTP_TUNER				25	// "ftpTuner"
#define RIG_TOKEN_FTP_SENSITIVITY		26	// "ftpSensitivity"		// button_statement range end

// LED COLORS

#define RIG_TOKEN_LED_BLACK  			27	// led_color range start
#define RIG_TOKEN_LED_RED    			28
#define RIG_TOKEN_LED_GREEN  			29
#define RIG_TOKEN_LED_BLUE   			30
#define RIG_TOKEN_LED_YELLOW 			31
#define RIG_TOKEN_LED_PURPLE 			32
#define RIG_TOKEN_LED_ORANGE 			33
#define RIG_TOKEN_LED_WHITE  			34
#define RIG_TOKEN_LED_CYAN   			35	// led_color_range end

// DISPLAY COLORS

#define RIG_TOKEN_DISPLAY_BLACK     	36	// display_color range start
#define RIG_TOKEN_DISPLAY_BLUE      	37
#define RIG_TOKEN_DISPLAY_RED       	38
#define RIG_TOKEN_DISPLAY_GREEN     	39
#define RIG_TOKEN_DISPLAY_CYAN      	40
#define RIG_TOKEN_DISPLAY_MAGENTA   	41
#define RIG_TOKEN_DISPLAY_YELLOW    	42
#define RIG_TOKEN_DISPLAY_WHITE     	43
#define RIG_TOKEN_DISPLAY_NAVY      	44
#define RIG_TOKEN_DISPLAY_DARKGREEN 	45
#define RIG_TOKEN_DISPLAY_DARKCYAN  	46
#define RIG_TOKEN_DISPLAY_MAROON    	47
#define RIG_TOKEN_DISPLAY_PURPLE    	48
#define RIG_TOKEN_DISPLAY_OLIVE     	49
#define RIG_TOKEN_DISPLAY_LIGHTGREY 	50
#define RIG_TOKEN_DISPLAY_DARKGREY  	51
#define RIG_TOKEN_DISPLAY_ORANGE    	52
#define RIG_TOKEN_DISPLAY_GREENYELLOW 	53
#define RIG_TOKEN_DISPLAY_PINK			54		// display_range end

// midi ports

#define RIG_TOKEN_MIDI					55
#define RIG_TOKEN_SERIAL				56

// font decoration

#define RIG_TOKEN_BOLD         			57
#define RIG_TOKEN_NORMAL         		58


#define LAST_RIG_IDENTIFIER				58		// identifier range end

// Special Types

#define RIG_TOKEN_TEXT					101		// a quoted string
#define RIG_TOKEN_NUMBER				102		// a literal number
#define RIG_TOKEN_IDENTIFIER			103		// an unresolved identifier

// Symbols

#define RIG_TOKEN_ASSIGN				110		// "="
#define RIG_TOKEN_LEFT_PAREN	 		111  	// "("
#define RIG_TOKEN_RIGHT_PAREN			112 	// ")"
#define RIG_TOKEN_LEFT_BRACKET			113 	// "["
#define RIG_TOKEN_RIGHT_BRACKET			114 	// "]"
#define RIG_TOKEN_COLON					115 	// ":"
#define RIG_TOKEN_COMMA					116 	// ","
#define RIG_TOKEN_SEMICOLON				117 	// ";"

#define RIG_TOKEN_PLUS					118		// "+"	// binop range
#define RIG_TOKEN_TIMES					119		// "*"
#define RIG_TOKEN_BITWISE_OR			120		// "|"
#define RIG_TOKEN_BITWISE_AND			121		// "&"
#define RIG_TOKEN_LOGICAL_OR			122		// "||"
#define RIG_TOKEN_LOGICAL_AND			123		// "&&"
#define RIG_TOKEN_EQUALS				124		// "=="	// binop range

#define RIG_TOKEN_QUESTION_MARK 		125		// "?"
