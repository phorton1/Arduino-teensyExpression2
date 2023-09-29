//-------------------------------------------------------
// rigToken.cpp
//-------------------------------------------------------
// Implements the getToken method for Rig Text files

#include <myDebug.h>
#include "rigToken.h"
#include "fileSystem.h"
#include "theSystem.h"
#include "winDialogs.h"


#define dbg_open	1
#define dbg_chars	1
	// shows chars as they are parsed
#define dbg_low 	1
	// 0 = show tokens that returned
	// -1 = show token processing
	// -2 = show gruesome processing
#define dbg_rewind  0
	// 0 = show calls to rewindRigFile


static myFile_t rig_file;
static uint32_t rig_text_len;
static uint32_t parse_ptr;
static int parse_line_num;
static int parse_char_num;
static int rig_token_len;
static bool modal_rig;

// externa

rig_token_t rig_token;
bool parse_error_found;
int parse_section;



//------------------------------
// utilities
//------------------------------

#define ERROR_COLOR_STRING      "\033[91m"       // red

// extern
void parse_error(const char *format, ...)
{
	parse_error_found = 1;

	char error_buffer[255];
	if (rig_token.line_num)
		sprintf(error_buffer,"line(%d) char(%d) - ",rig_token.line_num,rig_token.char_num);
	else
		error_buffer[0] = 0;
	char *text = &error_buffer[strlen(error_buffer)];

	va_list var;
	va_start(var, format);
	vsprintf(text,format,var);

	if (dbgSerial)
	{
		dbgSerial->print(ERROR_COLOR_STRING);
		dbgSerial->print("PARSE ERROR - ");
		dbgSerial->println(error_buffer);
	}

	ok_dlg.setMessage(TFT_RED,"Parse Error",error_buffer);
	the_system.startWindow(&ok_dlg);
}






// extern
const char *rigTokenToString(int token_id)
	// used to match UC identifiers and debug parse
{
	switch(token_id)
	{
		case RIG_TOKEN_EOF 					: return "EOF";

		case RIG_TOKEN_BASERIG				: return "BASERIG";
		case RIG_TOKEN_MODAL				: return "MODALRIG";
		case RIG_TOKEN_ON_UPDATE			: return "ONUPDATE";

		case RIG_TOKEN_DEFINE_DEF			: return "DEFINE";
		case RIG_TOKEN_STRING_DEF			: return "DEFINE_STRING";

		case RIG_TOKEN_BUTTON				: return "BUTTON";
		case RIG_TOKEN_CLICK				: return "CLICK";
		case RIG_TOKEN_LONG					: return "LONG";
		case RIG_TOKEN_PRESS				: return "PRESS";
		case RIG_TOKEN_REPEAT				: return "REPEAT";
		case RIG_TOKEN_RELEASE				: return "RELEASE";

		case RIG_TOKEN_PEDAL				: return "PEDAL";
		case RIG_TOKEN_ROTARY				: return "ROTARY";
		case RIG_TOKEN_LISTEN				: return "LISTEN";
		case RIG_TOKEN_LISTEN_RANGED		: return "LISTEN_RANGED";

		case RIG_TOKEN_AREA					: return "AREA";
		case RIG_TOKEN_METER				: return "METER";
		case RIG_TOKEN_SETVALUE				: return "SETVALUE";
		case RIG_TOKEN_SETTITLE				: return "SETTITLE";
		case RIG_TOKEN_DISPLAY 				: return "DISPLAY";
		case RIG_TOKEN_DISPLAY_NUMBER		: return "DISPLAYNUMBER";
		case RIG_TOKEN_SET_METER			: return "SETMETER";
		case RIG_TOKEN_SEND_CC 				: return "SENDCC";
		case RIG_TOKEN_SEND_PGM_CHG			: return "SENDPGMCHG";
		case RIG_TOKEN_NOTE_ON				: return "NOTEON";
		case RIG_TOKEN_NOTE_OFF				: return "NOTEOFF";
		case RIG_TOKEN_ALL_NOTES_OFF		: return "ALLNOTESOFF";
		case RIG_TOKEN_SET_BUTTON_COLOR		: return "SETBUTTONCOLOR";
		case RIG_TOKEN_SET_BUTTON_BLINK		: return "SETBUTTONBLINK";
		case RIG_TOKEN_LOAD_RIG				: return "LOADRIG";
		case RIG_TOKEN_END_MODAL			: return "ENDMODAL";
		case RIG_TOKEN_FTP_TUNER			: return "FTPTUNER";
		case RIG_TOKEN_FTP_SENSITIVITY		: return "FTPSENSITIVITY";

		case RIG_TOKEN_LED_BLACK    		: return "LED_BLACK";
		case RIG_TOKEN_LED_RED				: return "LED_RED";
		case RIG_TOKEN_LED_GREEN			: return "LED_GREEN";
		case RIG_TOKEN_LED_BLUE				: return "LED_BLUE";
		case RIG_TOKEN_LED_YELLOW			: return "LED_YELLOW";
		case RIG_TOKEN_LED_PURPLE			: return "LED_PURPLE";
		case RIG_TOKEN_LED_ORANGE			: return "LED_ORANGE";
		case RIG_TOKEN_LED_WHITE			: return "LED_WHITE";
		case RIG_TOKEN_LED_CYAN				: return "LED_CYAN";

		case RIG_TOKEN_DISPLAY_BLACK      	: return "BLACK";
		case RIG_TOKEN_DISPLAY_BLUE       	: return "BLUE";
		case RIG_TOKEN_DISPLAY_RED        	: return "RED";
		case RIG_TOKEN_DISPLAY_GREEN      	: return "GREEN";
		case RIG_TOKEN_DISPLAY_CYAN       	: return "CYAN";
		case RIG_TOKEN_DISPLAY_MAGENTA    	: return "MAGENTA";
		case RIG_TOKEN_DISPLAY_YELLOW     	: return "YELLOW";
		case RIG_TOKEN_DISPLAY_WHITE      	: return "WHITE";
		case RIG_TOKEN_DISPLAY_NAVY       	: return "NAVY";
		case RIG_TOKEN_DISPLAY_DARKGREEN  	: return "DARKGREEN";
		case RIG_TOKEN_DISPLAY_DARKCYAN   	: return "DARKCYAN";
		case RIG_TOKEN_DISPLAY_MAROON     	: return "MAROON";
		case RIG_TOKEN_DISPLAY_PURPLE     	: return "PURPLE";
		case RIG_TOKEN_DISPLAY_OLIVE      	: return "OLIVE";
		case RIG_TOKEN_DISPLAY_LIGHTGREY  	: return "LIGHTGREY";
		case RIG_TOKEN_DISPLAY_DARKGREY   	: return "DARKGREY";
		case RIG_TOKEN_DISPLAY_ORANGE     	: return "ORANGE";
		case RIG_TOKEN_DISPLAY_GREENYELLOW	: return "GREENYELLOW";
		case RIG_TOKEN_DISPLAY_PINK       	: return "PINK";

		case RIG_TOKEN_USB1					: return "USB1";
		case RIG_TOKEN_USB2					: return "USB2";
		case RIG_TOKEN_USB3					: return "USB3";
		case RIG_TOKEN_USB4					: return "USB4";
		case RIG_TOKEN_HOST1				: return "HOST1";
		case RIG_TOKEN_HOST2				: return "HOST2";
		case RIG_TOKEN_SERIAL				: return "SERIAL";

		case RIG_TOKEN_BOLD					: return "BOLD";
		case RIG_TOKEN_NORMAL				: return "NORMAL";

		case RIG_TOKEN_LEFT					: return "LEFT";
		case RIG_TOKEN_CENTER				: return "CENTER";
		case RIG_TOKEN_RIGHT				: return "RIGHT";

		case RIG_TOKEN_STRING				: return "STRING";
		case RIG_TOKEN_VALUE				: return "VALUE";

		case RIG_TOKEN_INHERIT				: return "INHERIT";
		case RIG_TOKEN_BUTTON_NUM			: return "_BUTTON_NUM";
		case RIG_TOKEN_BUTTON_ROW			: return "_BUTTON_ROW";
		case RIG_TOKEN_BUTTON_COL			: return "_BUTTON_COL";

		case RIG_TOKEN_HORZ					: return "HORZ";
		case RIG_TOKEN_VERT					: return "VERT";

		case RIG_TOKEN_INPUT				: return("INPUT");
		case RIG_TOKEN_OUTPUT				: return("OUTPUT");
		case RIG_TOKEN_BOTH					: return("BOTH");

		case RIG_TOKEN_TEXT					: return "TEXT";
		case RIG_TOKEN_NUMBER				: return "NUMBER";
		case RIG_TOKEN_IDENTIFIER			: return "IDENTIFIER";

		case RIG_TOKEN_COMMA				: return "COMMA";
		case RIG_TOKEN_SEMICOLON			: return "SEMICOLON";
		case RIG_TOKEN_LEFT_BRACKET			: return "LEFT_BRACKET";
		case RIG_TOKEN_RIGHT_BRACKET		: return "RIGHT_BRACKET";

		case RIG_TOKEN_LEFT_PAREN			: return "LEFT_PAREN";
		case RIG_TOKEN_RIGHT_PAREN			: return "RIGHT_PAREN";
		case RIG_TOKEN_QUESTION 			: return "QUESTION";

		case RIG_TOKEN_COLON				: return "COLON";
		case RIG_TOKEN_NOT					: return "NOT";
		case RIG_TOKEN_PLUS					: return "PLUS";
		case RIG_TOKEN_MINUS				: return "MINUS";
		case RIG_TOKEN_TIMES				: return "TIMES";
		case RIG_TOKEN_DIVIDE				: return "DIVIDED_BY";
		case RIG_TOKEN_EQ					: return "EQ";
		case RIG_TOKEN_NE					: return "NE";
		case RIG_TOKEN_GT					: return "GT";
		case RIG_TOKEN_GE					: return "GE";
		case RIG_TOKEN_LT					: return "LT";
		case RIG_TOKEN_LE					: return "LE";
		case RIG_TOKEN_BITWISE_OR			: return "OR";
		case RIG_TOKEN_BITWISE_AND			: return "AND";
		case RIG_TOKEN_LOGICAL_OR			: return "L_OR";
		case RIG_TOKEN_LOGICAL_AND			: return "L_AND";

		case RIG_TOKEN_ASSIGN				: return "ASSIGN";

	}

	// statement Tokens

	parse_error("UNKNOWN_TOKEN(%d)",token_id);
	return "UNKNOWN_TOKEN";

}


//---------------------------------------------
// API
//---------------------------------------------

// extern
void closeRigFile()
{
	rig_file.close();
}


// extern
bool openRigFile(const char *name)
{
	parse_ptr = 0;
	parse_line_num = 1;
	parse_char_num = 0;
	parse_section = 0;
	rig_text_len = 0;
	modal_rig = 0;

	memset(&rig_token,0,sizeof(rig_token_t));

	#if DUMP_PARSE
		initDump();
	#endif

    char name_buffer[128];
    strcpy(name_buffer,"/");
    strcat(name_buffer,name);
    strcat(name_buffer,".rig");
    display(dbg_open,"openRigFile(%s)",name);

	rig_file = SD.open(name_buffer, FILE_READ );
    if (!rig_file)
    {
        parse_error("Could not open Rig file: %s",name_buffer);
        return 0;
    }

    rig_text_len = rig_file.size();
    display(dbg_open,"Rig file(%s) opened - length=%d",name,rig_text_len);
	return 1;
}


// extern
const char *rigTokenToText(int token_id)
	// used to generate readable files
{
	switch(token_id)
	{
		case RIG_TOKEN_BASERIG				: return "BaseRig";
		case RIG_TOKEN_MODAL				: return "ModalRig";
		case RIG_TOKEN_ON_UPDATE			: return "onUpdate";

		case RIG_TOKEN_DEFINE_DEF			: return "define";
		case RIG_TOKEN_STRING_DEF			: return "define_string";

		case RIG_TOKEN_CLICK				: return "click";
		case RIG_TOKEN_LONG					: return "long";
		case RIG_TOKEN_PRESS				: return "press";
		case RIG_TOKEN_REPEAT				: return "repeat";
		case RIG_TOKEN_RELEASE				: return "release";

		case RIG_TOKEN_AREA					: return "Area";
		case RIG_TOKEN_METER				: return "Meter";

		case RIG_TOKEN_SETVALUE				: return "setValue";
		case RIG_TOKEN_SETTITLE				: return "setTitle";
		case RIG_TOKEN_DISPLAY 				: return "display";
		case RIG_TOKEN_DISPLAY_NUMBER		: return "displayNumber";
		case RIG_TOKEN_SET_METER			: return "setMeter";
		case RIG_TOKEN_SEND_CC 				: return "sendCC";
		case RIG_TOKEN_SEND_PGM_CHG			: return "sendPgmChg";
		case RIG_TOKEN_NOTE_ON				: return "noteOn";
		case RIG_TOKEN_NOTE_OFF				: return "noteOff";
		case RIG_TOKEN_ALL_NOTES_OFF		: return "allNotesOff";
		case RIG_TOKEN_SET_BUTTON_COLOR		: return "setButtonColor";
		case RIG_TOKEN_SET_BUTTON_BLINK		: return "setButtonBlink";
		case RIG_TOKEN_LOAD_RIG				: return "loadRig";
		case RIG_TOKEN_END_MODAL			: return "endModal";
		case RIG_TOKEN_FTP_TUNER			: return "ftpTuner";
		case RIG_TOKEN_FTP_SENSITIVITY		: return "ftpSensitivity";

		case RIG_TOKEN_COMMA				: return ",";
		case RIG_TOKEN_SEMICOLON			: return ";";
		case RIG_TOKEN_LEFT_BRACKET			: return "[";
		case RIG_TOKEN_RIGHT_BRACKET		: return "]";

		case RIG_TOKEN_LEFT_PAREN			: return "(";
		case RIG_TOKEN_RIGHT_PAREN			: return ")";
		case RIG_TOKEN_QUESTION 			: return "?";
		case RIG_TOKEN_COLON				: return ":";
		case RIG_TOKEN_NOT					: return "!";

		case RIG_TOKEN_PLUS					: return "+";
		case RIG_TOKEN_MINUS				: return "-";
		case RIG_TOKEN_TIMES				: return "*";
		case RIG_TOKEN_DIVIDE				: return "/";

		case RIG_TOKEN_EQ					: return "==";
		case RIG_TOKEN_NE					: return "!=";
		case RIG_TOKEN_GT					: return ">";
		case RIG_TOKEN_GE					: return ">=";
		case RIG_TOKEN_LT					: return "<";
		case RIG_TOKEN_LE					: return "<=";

		case RIG_TOKEN_BITWISE_OR			: return "|";
		case RIG_TOKEN_BITWISE_AND			: return "&";
		case RIG_TOKEN_LOGICAL_OR			: return "||";
		case RIG_TOKEN_LOGICAL_AND			: return "&&";

		case RIG_TOKEN_IDENTIFIER			: return "ID";
		case RIG_TOKEN_ASSIGN				: return "=";

	}

	return rigTokenToString(token_id);

}




//--------------------------------------
// getRigToken
//--------------------------------------

static int getRigDelim(char c)
{
	switch (c)
	{
		case ',' : return RIG_TOKEN_COMMA;
		case ';' : return RIG_TOKEN_SEMICOLON;
		case '(' : return RIG_TOKEN_LEFT_PAREN;
		case ')' : return RIG_TOKEN_RIGHT_PAREN;
		case '[' : return RIG_TOKEN_LEFT_BRACKET;
		case ']' : return RIG_TOKEN_RIGHT_BRACKET;
		case ':' : return RIG_TOKEN_COLON;
		case '?' : return RIG_TOKEN_QUESTION;
		case '!' : return RIG_TOKEN_NOT;
		case '+' : return RIG_TOKEN_PLUS;
		case '-' : return RIG_TOKEN_MINUS;
		case '*' : return RIG_TOKEN_TIMES;
		case '/' : return RIG_TOKEN_DIVIDE;
		case '>' : return RIG_TOKEN_GT;
		case '<' : return RIG_TOKEN_LT;
		case '|' : return RIG_TOKEN_BITWISE_OR;
		case '&' : return RIG_TOKEN_BITWISE_AND;
		case '=' : return RIG_TOKEN_ASSIGN;
	}
	return 0;
}


static bool addTokenChar(char c)
{
    if (rig_token_len >= MAX_RIG_TOKEN)
    {
        parse_error("token too long");
        return false;
    }
    rig_token.text[rig_token_len++] = c;
    return true;
}


static bool myStrcmpI(const char *id, const char *TOKEN)
{
	while (*id && *TOKEN)
	{
		char c = *id++;
		if (c >= 'a' && c <= 'z')
			c -= 'a' - 'A';
		if (c != *TOKEN++)
			return false;
	}
	if (*id || *TOKEN)
		return false;
	return true;
}


// extern
int getRigToken()
{
	rig_token_len = 0;
	memset(&rig_token,0,sizeof(rig_token_t));
	rig_token.line_num = parse_line_num;
	rig_token.char_num = parse_char_num;

	int delim;
    bool done = false;
    bool in_comment = false;

    while (!done && parse_ptr < rig_text_len)
    {
		char c = rig_file.read();
		parse_ptr++;
		if (c == -1)
		{
			parse_error("File Read Error");
			return 0;
		}

        parse_char_num++;
        if (!rig_token_len)
        {
            rig_token.line_num = parse_line_num;
            rig_token.char_num  = parse_char_num;
        }

		if (dbg_chars <= 0 && dbgSerial)
			dbgSerial->print(c);

        if (c == 10)
        {
            // skip LF, though it also resets character number
            parse_char_num = 0;
        }
        else if (c == '#')
        {
            display(dbg_low+1,"start comment",0);

            in_comment = true;
            if (rig_token.id == RIG_TOKEN_STRING)
            {
                parse_error("unclosed quote");
                return 0;
            }
            else if (rig_token_len)
            {
                done = true;
            }
        }

        // line break

        else if (c == 13)
        {
            display(dbg_low+2,"line_break",0);

            if (rig_token.id == RIG_TOKEN_STRING)
            {
                parse_error("unclosed quote");
                return 0;
            }

            if (in_comment)
            {
                display(dbg_low+2,"end_comment",0);
                in_comment = false;
            }

            parse_line_num++;
            parse_char_num = 0;

            if (rig_token_len)
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
            if (rig_token.id == RIG_TOKEN_TEXT)
            {
                rig_token.text[rig_token_len] = 0;
                done = true;
            }
            else if (rig_token_len)
            {
                parse_error("unexpected quote");
                return 0;
            }
            else
            {
                rig_token.id = RIG_TOKEN_TEXT;
            }
        }
        else if (rig_token.id == RIG_TOKEN_TEXT)
        {
			if (c=='\\' && rig_file.peek()=='n')
			{
				rig_file.read();
				parse_ptr++;
				parse_char_num++;
				c = 13;
			}
			if (!addTokenChar(c))
				return 0;
		}

        // white space

        else if (c == 9 || c == 32)
        {
            display(dbg_low+2,"white_space",0);
            if (rig_token_len)
            {
                done = true;
            }
        }

       // delims

        else if ((delim = getRigDelim(c)))
        {
            done = true;
            display(dbg_low+1,"delim",0);

			if (rig_token.id)		// we're parsing something else, so backup one
			{
				display(dbg_low+1,"",0);
				display(dbg_low+1,"backing up",0);
				parse_ptr--;
				parse_char_num--;
				rig_file.seek(parse_ptr);
			}
			else	// it's actual delimiter token
			{
				addTokenChar(c);

				if (delim == RIG_TOKEN_ASSIGN)
				{
					if (rig_file.peek() == '=')
					{
						rig_file.read();
						rig_token.id = RIG_TOKEN_EQ;
						parse_ptr++;
						parse_char_num++;
						addTokenChar(c);
					}
					else
					{
						parse_error("illegal symbol '%c'",c);
						return 0;
					}
				}
				else if (rig_file.peek() == '=' && (
						delim == RIG_TOKEN_NOT ||
						delim == RIG_TOKEN_GT ||
						delim == RIG_TOKEN_LT))
				{
					rig_file.read();
					rig_token.id =
						delim == RIG_TOKEN_NOT ? RIG_TOKEN_NE :
						delim == RIG_TOKEN_GT  ? RIG_TOKEN_GE :
						RIG_TOKEN_LE;
					;	// must be in order!
					parse_ptr++;
					parse_char_num++;
					addTokenChar(c);
				}
				else if (delim == RIG_TOKEN_BITWISE_OR && rig_file.peek() == '|')
				{
					rig_file.read();
					rig_token.id = RIG_TOKEN_LOGICAL_OR;
					parse_ptr++;
					parse_char_num++;
					addTokenChar(c);
				}
				else if (delim == RIG_TOKEN_BITWISE_AND && rig_file.peek() == '&')
				{
					rig_file.read();
					rig_token.id = RIG_TOKEN_LOGICAL_AND;
					parse_ptr++;
					parse_char_num++;
					addTokenChar(c);
				}
				else
				{
					rig_token.id = delim;
				}
			}
        }

        // numbers

        else if (!rig_token_len && c >= '0' && c <= '9')
        {
            rig_token.id = RIG_TOKEN_NUMBER;
            rig_token.int_value = c - '0';
            if (!addTokenChar(c))
                return 0;
        }
        else if (rig_token.id == RIG_TOKEN_NUMBER)
        {
            if (c >= '0' && c <= '9')
            {
                if (rig_token_len >= 3)
                {
                    parse_error("NUMBER too long");
                    return 0;
                }
                rig_token.int_value *= 10;
                rig_token.int_value += c - '0';
                if (!addTokenChar(c))
                    return 0;
            }
            else
            {
                parse_error("bad number");
                return 0;
            }
        }

        // building possible identifier we only allow letters

        else if ((c == '_') ||
				 (c >= 'A' && c <='Z') ||
                 (c >= 'a' && c <='z') ||
				 (c >= '0' && c <='9'))
        {
			display(dbg_low+1,"identifier char '%c'",c);
			rig_token.id = RIG_TOKEN_IDENTIFIER;
            if (!addTokenChar(c))
                return 0;
        }
        else
        {
            parse_error("illegal character %d='%c'",c,c>=32?c:'.');
            return 0;
        }

    }   // while !(done)


    // see if the buffer matches a known token

    rig_token.text[rig_token_len++] = 0;

	int id = rig_token.id;
	if (id == RIG_TOKEN_IDENTIFIER)
	{
		for (int i=1; i<=LAST_RIG_IDENTIFIER; i++)
		{
			// display(0,"cmp %s <> %s",rig_token.text, rigTokenToString(i));

			if (myStrcmpI(rig_token.text, rigTokenToString(i)))
			{
				id = i;
				break;
			}
		}
		if (id)
		{
			rig_token.id = id;		// found reserved word

		}
		else // user defined identifier
		{
			bool id_ok = 1;
			for (uint16_t i=0; i<=strlen(rig_token.text); i++)
			{
				char c = rig_token.text[i];
				if (c == '_' ||
					(c >= 'A' && c <= 'Z') ||
					(c >= 'a' && c <= 'z') ||
					(c >= '0' && c <= '9'))
				{
					// ok empty case
				}
				else
				{
					id_ok = 0;
					break;
				}
			}

			if (!id_ok)
			{
				parse_error("illegal identifier: %s",rig_token.text);
				return 0;
			}
		}
	}

	display(dbg_low,"",0);
	if (rig_token.id == RIG_TOKEN_NUMBER)
		display(dbg_low,"getToken(NUMBER,%d:%d) %d=%s -->  %d",
			rig_token.line_num,
			rig_token.char_num,
			rig_token.id,
			rigTokenToString(rig_token.id),
			rig_token.int_value);
	else if (rig_token.id == RIG_TOKEN_IDENTIFIER || rig_token.id == RIG_TOKEN_TEXT)
		display(dbg_low,"getToken(ID/TEXT,%d:%d) %d=%s -->  %s",
			rig_token.line_num,
			rig_token.char_num,
			rig_token.id,
			rigTokenToString(rig_token.id),
			rig_token.text);
	else
		display(dbg_low,"getToken(%d:%d) %d=%s",
			rig_token.line_num,
			rig_token.char_num,
			rig_token.id,
			rigTokenToString(rig_token.id));

	// basic lexical structure

	if (parse_section == PARSE_SECTION_NONE)
	{
		if (id != RIG_TOKEN_BASERIG &&
			id != RIG_TOKEN_MODAL)
		{
			parse_error("Rig must start with BaseRig or ModalRig");
			return 0;
		}
		else
		{
			modal_rig = id == RIG_TOKEN_MODAL;
			parse_section = PARSE_SECTION_INIT;
		}
	}
	else if (parse_section != PARSE_SECTION_NONE && (
			id == RIG_TOKEN_BASERIG ||
			id == RIG_TOKEN_MODAL))
	{
		parse_error("BaseRig or ModalRig only allowed as first Token");
		return 0;
	}
	else if (parse_section != PARSE_SECTION_INIT && IS_INIT_HEADER_STATEMENT(id))
	{
		parse_error("%s statement only allowed in init_header_section",rigTokenToString(id));
		return 0;
	}
	else if (parse_section != PARSE_SECTION_INIT && IS_INIT_ONLY_STATEMENT(id))
	{
		parse_error("%s statement only allowed in init_section",rigTokenToString(id));
		return 0;
	}
	else if (parse_section != PARSE_SECTION_BUTTONS && IS_BUTTON_ONLY_STATEMENT(id))
	{
		parse_error("%s statement only allowed in button_section",rigTokenToString(id));
		return 0;
	}
	else if (id == RIG_TOKEN_ON_UPDATE)
	{
		parse_section = PARSE_SECTION_UPDATE;
	}
	else if (id == RIG_TOKEN_BUTTON)
	{
		parse_section = PARSE_SECTION_BUTTONS;
	}
	else if (modal_rig && id == RIG_TOKEN_LISTEN)
	{
		parse_error("LISTEN statement only allowed in baseRigs");
		return 0;
	}
	else if (!modal_rig && id == RIG_TOKEN_END_MODAL)
	{
		parse_error("endModal statement only allowed in modalRigs");
		return 0;
	}
	else if (!modal_rig && id == RIG_TOKEN_INHERIT)
	{
		parse_error("buttons may only INHERIT in modalRigs");
	}

	// finished

	if (!rig_token.id)
		display(dbg_low,"getRigToken() returning EOF!",0);

    return rig_token.id;
}
