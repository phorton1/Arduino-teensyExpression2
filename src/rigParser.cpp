//-------------------------------------------------------
// rigParser.cpp
//-------------------------------------------------------
// Implements the class that parses Rig Text files
// and creates Rig memory images

#include <myDebug.h>
#include "rigOps.h"
#include "rigCode.h"
#include "fileSystem.h"

#define dbg_token 	-1
#define dbg_parse 	0


#define MAX_RIG_TEXT  	32767
#define MAX_RIG_TOKEN 	79

typedef struct
{
	int id;
	int type;

	bool is_statement;
	bool is_button_only_statement;
	bool is_led_color;
	bool is_display_color;
	bool is_delim;

	int len;
	int line_num;
	int char_num;

	int int_value;
	char text[MAX_RIG_TOKEN+1];

} token_t;

static int rig_text_len;
static char rig_text[MAX_RIG_TEXT];

static int parse_ptr;
static int parse_line_num;
static int parse_char_num;

token_t the_token;




//------------------------------
// utilities
//------------------------------

#define rig_error(f,...)        { error_fxn(f,__VA_ARGS__); }
	// for now just call myDebug::error_fxn()


void token_error(const char *errmsg)
{
    rig_error("%d:%d %s",parse_line_num,parse_char_num,errmsg);
}

void parse_error(const char *errmsg,const char *param)
{
    rig_error("%d:%d %s(%s)",the_token.line_num,the_token.char_num,errmsg,param);
}


static const char *tokenIdToString(int token_id)
{
	switch(token_id)
	{
		case RIG_ID_EOF 			: return "EOF";

		case RIG_ID_BASE_RIG		: return "BASERIG";
		case RIG_ID_OVERLAY			: return "OVERLAY";

		case RIG_ID_AREA			: return "AREA";
		case RIG_ID_LISTEN			: return "LISTEN";
		case RIG_ID_STRING			: return "STRING";

		case RIG_ID_LED				: return "LED";
		case RIG_ID_COLOR			: return "COLOR";
		case RIG_ID_BLINK			: return "BLINK";
		case RIG_ID_BUTTON			: return "BUTTON";
		case RIG_ID_PRESS			: return "PRESS";
		case RIG_ID_CLICK			: return "CLICK";
		case RIG_ID_LONG			: return "LONG";
		case RIG_ID_RELEASE			: return "RELEASE";
		case RIG_ID_REPEAT			: return "REPEAT";
		case RIG_ID_VALUE			: return "VALUE";
		case RIG_ID_NOT				: return "NOT";

		case RIG_ID_DISPLAY 		: return "DISPLAY";
		case RIG_ID_SEND_CC 		: return "SENDCC";
		case RIG_ID_SEND_PGM_CHG	: return "SENDPGMCHG";
		case RIG_ID_NOTE_ON			: return "NOTEON";
		case RIG_ID_NOTE_OFF		: return "NOTEOFF";
		case RIG_ID_ALL_NOTES_OFF	: return "ALLNOTESOFF";
		case RIG_ID_FTP_TUNER		: return "FTPTUNER";
		case RIG_ID_FTP_SENSITIVITY	: return "FTPSENSITIVITY";

		case RIG_ID_LED_BLACK       : return "LED_BLACK";
		case RIG_ID_LED_RED         : return "LED_RED";
		case RIG_ID_LED_GREEN       : return "LED_GREEN";
		case RIG_ID_LED_BLUE        : return "LED_BLUE";
		case RIG_ID_LED_YELLOW      : return "LED_YELLOW";
		case RIG_ID_LED_PURPLE      : return "LED_PURPLE";
		case RIG_ID_LED_ORANGE      : return "LED_ORANGE";
		case RIG_ID_LED_WHITE       : return "LED_WHITE";
		case RIG_ID_LED_CYAN        : return "LED_CYAN";

		case RIG_ID_DISPLAY_BLACK      	: return "BLACK";
		case RIG_ID_DISPLAY_BLUE       	: return "BLUE";
		case RIG_ID_DISPLAY_RED        	: return "RED";
		case RIG_ID_DISPLAY_GREEN      	: return "GREEN";
		case RIG_ID_DISPLAY_CYAN       	: return "CYAN";
		case RIG_ID_DISPLAY_MAGENTA    	: return "MAGENTA";
		case RIG_ID_DISPLAY_YELLOW     	: return "YELLOW";
		case RIG_ID_DISPLAY_WHITE      	: return "WHITE";
		case RIG_ID_DISPLAY_NAVY       	: return "NAVY";
		case RIG_ID_DISPLAY_DARKGREEN  	: return "DARKGREEN";
		case RIG_ID_DISPLAY_DARKCYAN   	: return "DARKCYAN";
		case RIG_ID_DISPLAY_MAROON     	: return "MAROON";
		case RIG_ID_DISPLAY_PURPLE     	: return "PURPLE";
		case RIG_ID_DISPLAY_OLIVE      	: return "OLIVE";
		case RIG_ID_DISPLAY_LIGHTGREY  	: return "LIGHTGREY";
		case RIG_ID_DISPLAY_DARKGREY   	: return "DARKGREY";
		case RIG_ID_DISPLAY_ORANGE     	: return "ORANGE";
		case RIG_ID_DISPLAY_GREENYELLOW	: return "GREENYELLOW";
		case RIG_ID_DISPLAY_PINK       	: return "PINK";

		case RIG_ID_TEXT			: return "TEXT";
		case RIG_ID_NUMBER			: return "NUMBER";
		case RIG_ID_IDENTIFIER		: return "IDENTIFIER";

		case RIG_ID_LEFT_PAREN		: return "LEFT_PAREN";
		case RIG_ID_RIGHT_PAREN		: return "RIGHT_PAREN";
		case RIG_ID_LEFT_BRACKET	: return "LEFT_BRACKET";
		case RIG_ID_RIGHT_BRACKET	: return "RIGHT_BRACKET";
		case RIG_ID_COLON			: return "COLON";
		case RIG_ID_COMMA			: return "COMMA";
		case RIG_ID_SEMICOLON		: return "SEMICOLON";
		case RIG_ID_ASSIGN			: return "ASSIGN";
		case RIG_ID_PLUS			: return "PLUS";
		case RIG_ID_TIMES			: return "TIMES";
		case RIG_ID_BITWISE_OR		: return "OR";
		case RIG_ID_BITWISE_AND		: return "AND";
		case RIG_ID_LOGICAL_OR		: return "L_OR";
		case RIG_ID_LOGICAL_AND		: return "L_AND";
		case RIG_ID_EQUALS			: return "EQUALS";
		case RIG_ID_QUESTION_MARK 	: return "QUESTION_MARK";

	}

	// statement Tokens

	token_error("UNKNOWN TOKEN TYPE");
	return "UNKNOWN_TOKEN_TYPE";

}


#if 0

	static const char *tokenToText(int token_id)
	{
		switch(token_id)
		{
			case RIG_ID_BASE_RIG		: return  "BaseRig";
			case RIG_ID_OVERLAY			: return  "Overlay";

			case RIG_ID_AREA			: return  "AREA";
			case RIG_ID_LISTEN			: return  "LISTEN";
			case RIG_ID_STRING			: return  "STRING";

			case RIG_ID_LED				: return  "LED";
			case RIG_ID_COLOR			: return  "color";
			case RIG_ID_BLINK			: return  "blink";
			case RIG_ID_BUTTON			: return  "BUTTON";
			case RIG_ID_PRESS			: return  "press";
			case RIG_ID_CLICK			: return  "click";
			case RIG_ID_LONG			: return  "long";
			case RIG_ID_RELEASE			: return  "release";
			case RIG_ID_REPEAT			: return  "repeat";
			case RIG_ID_VALUE			: return  "VALUE";
			case RIG_ID_NOT				: return  "NOT";

			case RIG_ID_DISPLAY 		: return  "display";
			case RIG_ID_SEND_CC 		: return  "sendCC";
			case RIG_ID_SEND_PGM_CHG	: return  "sendPgmChg";
			case RIG_ID_NOTE_ON			: return  "noteOn";
			case RIG_ID_NOTE_OFF		: return  "noteOff";
			case RIG_ID_ALL_NOTES_OFF	: return  "allNotesOff";
			case RIG_ID_FTP_TUNER		: return  "ftpTuner";
			case RIG_ID_FTP_SENSITIVITY	: return  "ftpSensitivity";

			case RIG_ID_LED_BLACK       : return "LED_BLACK";
			case RIG_ID_LED_RED         : return "LED_RED";
			case RIG_ID_LED_GREEN       : return "LED_GREEN";
			case RIG_ID_LED_BLUE        : return "LED_BLUE";
			case RIG_ID_LED_YELLOW      : return "LED_YELLOW";
			case RIG_ID_LED_PURPLE      : return "LED_PURPLE";
			case RIG_ID_LED_ORANGE      : return "LED_ORANGE";
			case RIG_ID_LED_WHITE       : return "LED_WHITE";
			case RIG_ID_LED_CYAN        : return "LED_CYAN";

			case RIG_ID_DISPLAY_BLACK       : return "BLACK";
			case RIG_ID_DISPLAY_BLUE        : return "BLUE";
			case RIG_ID_DISPLAY_RED         : return "RED";
			case RIG_ID_DISPLAY_GREEN       : return "GREEN";
			case RIG_ID_DISPLAY_CYAN        : return "CYAN";
			case RIG_ID_DISPLAY_MAGENTA     : return "MAGENTA";
			case RIG_ID_DISPLAY_YELLOW      : return "YELLOW";
			case RIG_ID_DISPLAY_WHITE       : return "WHITE";
			case RIG_ID_DISPLAY_NAVY        : return "NAVY";
			case RIG_ID_DISPLAY_DARKGREEN   : return "DARKGREEN";
			case RIG_ID_DISPLAY_DARKCYAN    : return "DARKCYAN";
			case RIG_ID_DISPLAY_MAROON      : return "MAROON";
			case RIG_ID_DISPLAY_PURPLE      : return "PURPLE";
			case RIG_ID_DISPLAY_OLIVE       : return "OLIVE";
			case RIG_ID_DISPLAY_LIGHTGREY   : return "LIGHTGREY";
			case RIG_ID_DISPLAY_DARKGREY    : return "DARKGREY";
			case RIG_ID_DISPLAY_ORANGE      : return "ORANGE";
			case RIG_ID_DISPLAY_GREENYELLOW : return "GREENYELLOW";
			case RIG_ID_DISPLAY_PINK        : return "PINK";

			case RIG_ID_TEXT			: return  "TEXT";
			case RIG_ID_NUMBER			: return  "NUMBER";
			case RIG_ID_IDENTIFIER		: return  "IDENTIFIER";

			case RIG_ID_LEFT_PAREN		: return  "(";
			case RIG_ID_RIGHT_PAREN		: return  ")";
			case RIG_ID_LEFT_BRACKET	: return  "[";
			case RIG_ID_RIGHT_BRACKET	: return  "]";
			case RIG_ID_COLON			: return  "";
			case RIG_ID_COMMA			: return  ",";
			case RIG_ID_SEMICOLON		: return  ";";
			case RIG_ID_ASSIGN			: return  "=";
			case RIG_ID_PLUS			: return  "+";
			case RIG_ID_TIMES			: return  "*";
			case RIG_ID_BITWISE_OR		: return  "|";
			case RIG_ID_BITWISE_AND		: return  "&";
			case RIG_ID_LOGICAL_OR		: return  "||";
			case RIG_ID_LOGICAL_AND		: return  "&&";
			case RIG_ID_EQUALS			: return  "==";
			case RIG_ID_QUESTION_MARK 	: return  "?";

		}

		// statement Tokens

		return "UNKNOWN_TOKEN_TYPE";

	}
#endif




//--------------------------------------
// getToken
//--------------------------------------

static int getDelim(char c)
{
	switch (c)
	{
		case '(' : return RIG_ID_LEFT_PAREN;
		case ')' : return RIG_ID_RIGHT_PAREN;
		case '[' : return RIG_ID_LEFT_BRACKET;
		case ']' : return RIG_ID_RIGHT_BRACKET;
		case ':' : return RIG_ID_COLON;
		case ',' : return RIG_ID_COMMA;
		case ';' : return RIG_ID_SEMICOLON;
		case '+' : return RIG_ID_PLUS;
		case '*' : return RIG_ID_TIMES;
		case '?' : return RIG_ID_QUESTION_MARK;

		case '=' : return RIG_ID_ASSIGN;
		case '|' : return RIG_ID_BITWISE_OR;
		case '&' : return RIG_ID_BITWISE_AND;
	}
	return false;
}


static bool addTokenChar(char c)
{
    if (the_token.len >= MAX_RIG_TOKEN)
    {
        token_error("token too long");
        return false;
    }
    the_token.text[the_token.len++] = c;
    return true;
}



static int getToken()
{
	memset(&the_token,0,sizeof(token_t));

	int delim;
    bool done = false;
    bool in_comment = false;

    while (!done && parse_ptr < rig_text_len)
    {
        char c = rig_text[parse_ptr++];
        parse_char_num++;
        if (!the_token.len)
        {
            the_token.line_num = parse_line_num;
            the_token.char_num  = parse_char_num;
        }

		if (dbg_token <= 0)
			dbgSerial->print(c);

        if (c == 10)
        {
            // skip LF, though it also resets character number
            parse_char_num = 0;
        }
        else if (c == '#')
        {
            display(dbg_token+2,"start comment",0);

            in_comment = true;
            if (the_token.id == RIG_ID_STRING)
            {
                token_error("unclosed quote");
                return -1;
            }
            else if (the_token.len)
            {
                done = true;
            }
        }

        // line break

        else if (c == 13)
        {
            display(dbg_token+2,"line_break",0);

            if (the_token.id == RIG_ID_STRING)
            {
                token_error("unclosed quote");
                return -1;
            }

            if (in_comment)
            {
                display(dbg_token+2,"end_comment",0);
                in_comment = false;
            }

            parse_line_num++;
            parse_char_num = 0;

            if (the_token.len)
            {
                done = true;
            }
        }

        else if (in_comment)
        {
            // do nothing
        }

        // quoted strings

        else if (c == '"')
        {
            if (the_token.id == RIG_ID_TEXT)
            {
                the_token.text[the_token.len] = 0;
                done = true;
            }
            else if (the_token.len)
            {
                token_error("unexpected quote");
                return -1;
            }
            else
            {
                the_token.id = RIG_ID_TEXT;
            }
        }
        else if (the_token.id == RIG_ID_TEXT)
        {
            if (c=='\\' && rig_text[parse_ptr]=='n')
            {
                parse_ptr++;
                parse_char_num++;
                c = 13;
            }
            if (!addTokenChar(c))
                return -1;
        }


        // white space

        else if (c == 9 || c == 32)
        {
            display(dbg_token+2,"white_space",0);
            if (the_token.len)
            {
                done = true;
            }
        }

       // delims

        else if ((delim = getDelim(c)))
        {
            done = true;
            display(dbg_token+2,"delim",0);

			if (the_token.id)		// we're parsing something else, so backup one
			{
				display(0,"",0);
				display(dbg_token+2,"backing up",0);
				parse_ptr--;
				parse_char_num--;
			}
			else	// it's actual delimiter token
			{
				addTokenChar(c);
				the_token.is_delim = 1;

				if (delim == RIG_ID_ASSIGN && rig_text[parse_ptr] == '=')
				{
					the_token.id = RIG_ID_EQUALS;
					parse_ptr++;
					parse_char_num++;
					addTokenChar(c);
				}
				else if (delim == RIG_ID_BITWISE_OR && rig_text[parse_ptr] == '|')
				{
					the_token.id = RIG_ID_LOGICAL_OR;
					parse_ptr++;
					parse_char_num++;
					addTokenChar(c);
				}
				else if (delim == RIG_ID_BITWISE_AND && rig_text[parse_ptr] == '&')
				{
					the_token.id = RIG_ID_LOGICAL_AND;
					parse_ptr++;
					parse_char_num++;
					addTokenChar(c);
				}
				else
				{
					the_token.id = delim;
				}
			}
        }

        // numbers

        else if (!the_token.len && c >= '0' && c <= '9')
        {
            the_token.id = RIG_ID_NUMBER;
            the_token.int_value = c - '0';
            if (!addTokenChar(c))
                return -1;
        }
        else if (the_token.id == RIG_ID_NUMBER)
        {
            if (c >= '0' && c <= '9')
            {
                if (the_token.len >= 3)
                {
                    token_error("number too long");
                    return -1;
                }
                the_token.int_value *= 10;
                the_token.int_value += c - '0';
                if (!addTokenChar(c))
                    return -1;
            }
            else
            {
                token_error("bad number");
                return -1;
            }
        }

        // building possible identifier we only allow letters

        else if ((c == '_') ||
				 (c >= 'A' && c <='Z') ||
                 (c >= 'a' && c <='z'))
        {
			the_token.id = RIG_ID_IDENTIFIER;
			if (c >= 'a' && c <= 'z')
				c = c - 'a' + 'A';

            if (!addTokenChar(c))
                return -1;
        }
        else
        {
            token_error("illegal character");
            return -1;
        }

    }   // while !(done)


    // see if the buffer matches a known token

    the_token.text[the_token.len++] = 0;

	if (the_token.id == RIG_ID_IDENTIFIER)
	{
		int id = 0;
		for (int i=1; i<=LAST_RIG_IDENTIFIER; i++)
		{
			// display(0,"cmp %s <> %s",the_token.text, tokenIdToString(i));

			if (!strcmp(the_token.text, tokenIdToString(i)))
			{
				id = i;
				break;
			}
		}
		if (!id)
		{
			parse_error("illegal identifier",the_token.text);
			return -1;
		}
		the_token.id = id;
		the_token.is_statement = id >= RIG_ID_DISPLAY && id <= RIG_ID_ALL_NOTES_OFF;
		the_token.is_button_only_statement = id >= RIG_ID_FTP_TUNER && id <= RIG_ID_FTP_SENSITIVITY;
		the_token.is_led_color = id >= RIG_ID_LED_BLACK && id <= RIG_ID_LED_CYAN;
		the_token.is_display_color = id >= RIG_ID_DISPLAY_BLACK && id <= RIG_ID_DISPLAY_PINK;
	}

	display(0,"",0);
	if (the_token.id == RIG_ID_NUMBER)
		display(dbg_token,"getToken(%d:%d) %d=%s -->  %d",
			the_token.line_num,
			the_token.char_num,
			the_token.id,
			tokenIdToString(the_token.id),
			the_token.int_value);
	else if (the_token.id == RIG_ID_IDENTIFIER || the_token.id == RIG_ID_TEXT)
		display(dbg_token,"getToken(%d:%d) %d=%s -->  %s",
			the_token.line_num,
			the_token.char_num,
			the_token.id,
			tokenIdToString(the_token.id),
			the_token.text);
	else
		display(dbg_token,"getToken(%d:%d) %d=%s",
			the_token.line_num,
			the_token.char_num,
			the_token.id,
			tokenIdToString(the_token.id));

	// was going too fast for console

	if (dbg_token <= 0)
		delay(5);

    return the_token.id;
}



//-------------------------------------------------
// Parser Main
//-------------------------------------------------



static bool openRigFile(const char *name)
{
    rig_text_len = 0;
	memset(rig_text,0,MAX_RIG_TEXT);

    char name_buffer[128];
    strcpy(name_buffer,"/");
    strcat(name_buffer,name);
    strcat(name_buffer,".rig");

    display(0,"openRigFile(%s)",name);

    File the_file = SD.open(name_buffer);
    if (!the_file)
    {
        rig_error("Could not open Rig file: %s",name_buffer);
        return 0;
    }
    uint32_t size = the_file.size();
    if (size > MAX_RIG_TEXT)
    {
        rig_error("Rig(%s) size(%d) exceeds MAX_RIG_TEXT=%d",name,size,MAX_RIG_TEXT);
        the_file.close();
        return 0;
    }

    uint32_t got = the_file.read(rig_text,size);
    if (got != size)
    {
        rig_error("Reading Rig(%s) got(%d) size(%d)",name,got,size);
        the_file.close();
        return 0;
    }

    the_file.close();
    rig_text_len = size;
    display(0,"Rig filee(%s) opened - length=%d",name,rig_text_len);

	return 1;
}






static void init_parse()
{
	parse_ptr = 0;
	parse_line_num = 1;
	parse_char_num = 0;
}



// extern
void parseRig()
{
	display(0,"ParseRig()",0);
	if (openRigFile("test"))
	{
		init_parse();

		int tt = getToken();
		while (tt > 0)
		{
			tt = getToken();
		}
	}
}
