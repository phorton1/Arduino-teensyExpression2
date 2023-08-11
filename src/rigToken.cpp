//-------------------------------------------------------
// rigToken.cpp
//-------------------------------------------------------
// Implements the getToken method for Rig Text files

#include <myDebug.h>
#include "rigToken.h"
#include "fileSystem.h"


#define dbg_token 	1



static File rig_file;
static int rig_text_len;
static int parse_ptr;
static int parse_line_num;
static int parse_char_num;

// extern
token_t rig_token;




//------------------------------
// utilities
//------------------------------

#define ERROR_COLOR_STRING      "\033[91m"       // red

// extern
void rig_error(const char *format, ...)
{
	if (!dbgSerial)
		return;

	char rig_error_buffer[255];
	sprintf(rig_error_buffer,"%d:%d ",rig_token.line_num,rig_token.char_num);
	char *text = &rig_error_buffer[strlen(rig_error_buffer)];

	va_list var;
	va_start(var, format);
	vsprintf(text,format,var);

	dbgSerial->print(ERROR_COLOR_STRING);
	dbgSerial->print("RIG_ERROR - ");
	dbgSerial->println(rig_error_buffer);
}






// extern
const char *rigTokenToString(int token_id)
	// used to match UC identifiers and debug parse
{
	switch(token_id)
	{
		case RIG_TOKEN_EOF 					: return "EOF";

		case RIG_TOKEN_BASERIG				: return "BASERIG";
		case RIG_TOKEN_OVERLAY				: return "OVERLAY";

		case RIG_TOKEN_PEDAL				: return "PEDAL";
		case RIG_TOKEN_AREA					: return "AREA";
		case RIG_TOKEN_LISTEN				: return "LISTEN";
		case RIG_TOKEN_STRING_DEF			: return "STRING_DEF";

		case RIG_TOKEN_BUTTON				: return "BUTTON";
		case RIG_TOKEN_COLOR				: return "COLOR";
		case RIG_TOKEN_BLINK				: return "BLINK";
		case RIG_TOKEN_PRESS				: return "PRESS";
		case RIG_TOKEN_CLICK				: return "CLICK";
		case RIG_TOKEN_LONG					: return "LONG";
		case RIG_TOKEN_RELEASE				: return "RELEASE";
		case RIG_TOKEN_REPEAT				: return "REPEAT";

		case RIG_TOKEN_STRING				: return "STRING";
		case RIG_TOKEN_VALUE				: return "VALUE";
		case RIG_TOKEN_NOT					: return "NOT";

		case RIG_TOKEN_SETVALUE				: return "SETVALUE";
		case RIG_TOKEN_DISPLAY 				: return "DISPLAY";
		case RIG_TOKEN_SEND_CC 				: return "SENDCC";
		case RIG_TOKEN_SEND_PGM_CHG			: return "SENDPGMCHG";
		case RIG_TOKEN_NOTE_ON				: return "NOTEON";
		case RIG_TOKEN_NOTE_OFF				: return "NOTEOFF";
		case RIG_TOKEN_ALL_NOTES_OFF		: return "ALLNOTESOFF";
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

		case RIG_TOKEN_MIDI					: return "MIDI";
		case RIG_TOKEN_SERIAL				: return "SERIAL";

		case RIG_TOKEN_BOLD					: return "BOLD";
		case RIG_TOKEN_NORMAL				: return "NORMAL";

		case RIG_TOKEN_TEXT					: return "TEXT";
		case RIG_TOKEN_NUMBER				: return "NUMBER";
		case RIG_TOKEN_IDENTIFIER			: return "IDENTIFIER";

		case RIG_TOKEN_LEFT_PAREN			: return "LEFT_PAREN";
		case RIG_TOKEN_RIGHT_PAREN			: return "RIGHT_PAREN";
		case RIG_TOKEN_LEFT_BRACKET			: return "LEFT_BRACKET";
		case RIG_TOKEN_RIGHT_BRACKET		: return "RIGHT_BRACKET";
		case RIG_TOKEN_COLON				: return "COLON";
		case RIG_TOKEN_COMMA				: return "COMMA";
		case RIG_TOKEN_SEMICOLON			: return "SEMICOLON";
		case RIG_TOKEN_ASSIGN				: return "ASSIGN";
		case RIG_TOKEN_PLUS					: return "PLUS";
		case RIG_TOKEN_TIMES				: return "TIMES";
		case RIG_TOKEN_BITWISE_OR			: return "OR";
		case RIG_TOKEN_BITWISE_AND			: return "AND";
		case RIG_TOKEN_LOGICAL_OR			: return "L_OR";
		case RIG_TOKEN_LOGICAL_AND			: return "L_AND";
		case RIG_TOKEN_EQUALS				: return "EQUALS";
		case RIG_TOKEN_QUESTION_MARK 		: return "QUESTION_MARK";

	}

	// statement Tokens

	rig_error("UNKNOWN_TOKEN(%d)",token_id);
	return "UNKNOWN_TOKEN";

}


// extern
const char *rigTokenToText(int token_id)
	// used to generate readable files
{
	switch(token_id)
	{
		case RIG_TOKEN_BASERIG				: return "BaseRig";
		case RIG_TOKEN_OVERLAY				: return "Overlay";

		case RIG_TOKEN_PEDAL				: return "PEDAL";
		case RIG_TOKEN_AREA					: return "AREA";
		case RIG_TOKEN_LISTEN				: return "LISTEN";
		case RIG_TOKEN_STRING_DEF			: return "STRING_DEF";

		case RIG_TOKEN_BUTTON				: return "BUTTON";
		case RIG_TOKEN_COLOR				: return "color";
		case RIG_TOKEN_BLINK				: return "blink";
		case RIG_TOKEN_PRESS				: return "press";
		case RIG_TOKEN_CLICK				: return "click";
		case RIG_TOKEN_LONG					: return "long";
		case RIG_TOKEN_RELEASE				: return "release";
		case RIG_TOKEN_REPEAT				: return "repeat";

		case RIG_TOKEN_STRING				: return "STRING";
		case RIG_TOKEN_VALUE				: return "VALUE";
		case RIG_TOKEN_NOT					: return "NOT";

		case RIG_TOKEN_SETVALUE				: return "setValue";
		case RIG_TOKEN_DISPLAY 				: return "display";
		case RIG_TOKEN_SEND_CC 				: return "sendCC";
		case RIG_TOKEN_SEND_PGM_CHG			: return "sendPgmChg";
		case RIG_TOKEN_NOTE_ON				: return "noteOn";
		case RIG_TOKEN_NOTE_OFF				: return "noteOff";
		case RIG_TOKEN_ALL_NOTES_OFF		: return "allNotesOff";
		case RIG_TOKEN_FTP_TUNER			: return "ftpTuner";
		case RIG_TOKEN_FTP_SENSITIVITY		: return "ftpSensitivity";

		case RIG_TOKEN_LED_BLACK       		: return "LED_BLACK";
		case RIG_TOKEN_LED_RED         		: return "LED_RED";
		case RIG_TOKEN_LED_GREEN       		: return "LED_GREEN";
		case RIG_TOKEN_LED_BLUE        		: return "LED_BLUE";
		case RIG_TOKEN_LED_YELLOW      		: return "LED_YELLOW";
		case RIG_TOKEN_LED_PURPLE      		: return "LED_PURPLE";
		case RIG_TOKEN_LED_ORANGE      		: return "LED_ORANGE";
		case RIG_TOKEN_LED_WHITE       		: return "LED_WHITE";
		case RIG_TOKEN_LED_CYAN        		: return "LED_CYAN";

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

		case RIG_TOKEN_MIDI					: return "MIDI";
		case RIG_TOKEN_SERIAL				: return "SERIAL";

		case RIG_TOKEN_BOLD					: return "BOLD";
		case RIG_TOKEN_NORMAL				: return "NORMAL";

		case RIG_TOKEN_TEXT					: return "TEXT";
		case RIG_TOKEN_NUMBER				: return "NUMBER";
		case RIG_TOKEN_IDENTIFIER			: return "IDENTIFIER";

		case RIG_TOKEN_LEFT_PAREN			: return "(";
		case RIG_TOKEN_RIGHT_PAREN			: return ")";
		case RIG_TOKEN_LEFT_BRACKET			: return "[";
		case RIG_TOKEN_RIGHT_BRACKET		: return "]";
		case RIG_TOKEN_COLON				: return ":";
		case RIG_TOKEN_COMMA				: return ",";
		case RIG_TOKEN_SEMICOLON			: return ";";
		case RIG_TOKEN_ASSIGN				: return "=";
		case RIG_TOKEN_PLUS					: return "+";
		case RIG_TOKEN_TIMES				: return "*";
		case RIG_TOKEN_BITWISE_OR			: return "|";
		case RIG_TOKEN_BITWISE_AND			: return "&";
		case RIG_TOKEN_LOGICAL_OR			: return "||";
		case RIG_TOKEN_LOGICAL_AND			: return "&&";
		case RIG_TOKEN_EQUALS				: return "==";
		case RIG_TOKEN_QUESTION_MARK 		: return "?";

	}

	// statement Tokens

	rig_error("UnknownToken(%d)",token_id);
	return "UnknownToken";

}




//--------------------------------------
// getRigToken
//--------------------------------------

static int getRigDelim(char c)
{
	switch (c)
	{
		case '(' : return RIG_TOKEN_LEFT_PAREN;
		case ')' : return RIG_TOKEN_RIGHT_PAREN;
		case '[' : return RIG_TOKEN_LEFT_BRACKET;
		case ']' : return RIG_TOKEN_RIGHT_BRACKET;
		case ':' : return RIG_TOKEN_COLON;
		case ',' : return RIG_TOKEN_COMMA;
		case ';' : return RIG_TOKEN_SEMICOLON;
		case '+' : return RIG_TOKEN_PLUS;
		case '*' : return RIG_TOKEN_TIMES;
		case '?' : return RIG_TOKEN_QUESTION_MARK;

		case '=' : return RIG_TOKEN_ASSIGN;
		case '|' : return RIG_TOKEN_BITWISE_OR;
		case '&' : return RIG_TOKEN_BITWISE_AND;
	}
	return false;
}


static bool addTokenChar(char c)
{
    if (rig_token.len >= MAX_RIG_TOKEN)
    {
        rig_error("token too long");
        return false;
    }
    rig_token.text[rig_token.len++] = c;
    return true;
}



// extern
int getRigToken()
{
	memset(&rig_token,0,sizeof(token_t));

	int delim;
    bool done = false;
    bool in_comment = false;

    while (!done && parse_ptr < rig_text_len)
    {
		char c = rig_file.read();
		parse_ptr++;
		if (c == -1)
		{
			rig_error("File Read Error");
			return -1;
		}

        parse_char_num++;
        if (!rig_token.len)
        {
            rig_token.line_num = parse_line_num;
            rig_token.char_num  = parse_char_num;
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
            if (rig_token.id == RIG_TOKEN_STRING)
            {
                rig_error("unclosed quote");
                return -1;
            }
            else if (rig_token.len)
            {
                done = true;
            }
        }

        // line break

        else if (c == 13)
        {
            display(dbg_token+2,"line_break",0);

            if (rig_token.id == RIG_TOKEN_STRING)
            {
                rig_error("unclosed quote");
                return -1;
            }

            if (in_comment)
            {
                display(dbg_token+2,"end_comment",0);
                in_comment = false;
            }

            parse_line_num++;
            parse_char_num = 0;

            if (rig_token.len)
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
                rig_token.text[rig_token.len] = 0;
                done = true;
            }
            else if (rig_token.len)
            {
                rig_error("unexpected quote");
                return -1;
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
				return -1;
		}

        // white space

        else if (c == 9 || c == 32)
        {
            display(dbg_token+2,"white_space",0);
            if (rig_token.len)
            {
                done = true;
            }
        }

       // delims

        else if ((delim = getRigDelim(c)))
        {
            done = true;
            display(dbg_token+2,"delim",0);

			if (rig_token.id)		// we're parsing something else, so backup one
			{
				display(dbg_token+2,"",0);
				display(dbg_token+2,"backing up",0);
				parse_ptr--;
				parse_char_num--;
				rig_file.seek(parse_ptr);
			}
			else	// it's actual delimiter token
			{
				addTokenChar(c);
				rig_token.is_delim = 1;

				if (delim == RIG_TOKEN_ASSIGN && rig_file.peek() == '=')
				{
					rig_file.read();
					rig_token.id = RIG_TOKEN_EQUALS;
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

        else if (!rig_token.len && c >= '0' && c <= '9')
        {
            rig_token.id = RIG_TOKEN_NUMBER;
            rig_token.int_value = c - '0';
            if (!addTokenChar(c))
                return -1;
        }
        else if (rig_token.id == RIG_TOKEN_NUMBER)
        {
            if (c >= '0' && c <= '9')
            {
                if (rig_token.len >= 3)
                {
                    rig_error("number too long");
                    return -1;
                }
                rig_token.int_value *= 10;
                rig_token.int_value += c - '0';
                if (!addTokenChar(c))
                    return -1;
            }
            else
            {
                rig_error("bad number");
                return -1;
            }
        }

        // building possible identifier we only allow letters

        else if ((c == '_') ||
				 (c >= 'A' && c <='Z') ||
                 (c >= 'a' && c <='z'))
        {
			rig_token.id = RIG_TOKEN_IDENTIFIER;
			if (c >= 'a' && c <= 'z')
				c = c - 'a' + 'A';

            if (!addTokenChar(c))
                return -1;
        }
        else
        {
            rig_error("illegal character %d='%c'",c,c>=32?c:'.');
            return -1;
        }

    }   // while !(done)


    // see if the buffer matches a known token

    rig_token.text[rig_token.len++] = 0;

	int id = rig_token.id;
	if (id == RIG_TOKEN_IDENTIFIER)
	{
		for (int i=1; i<=LAST_RIG_IDENTIFIER; i++)
		{
			// display(0,"cmp %s <> %s",rig_token.text, rigTokenToString(i));

			if (!strcmp(rig_token.text, rigTokenToString(i)))
			{
				id = i;
				break;
			}
		}
		if (!id)
		{
			rig_error("illegal identifier: %s",rig_token.text);
			return -1;
		}
		rig_token.id = id;
	}

	rig_token.is_init_statement =
		(id >= RIG_TOKEN_PEDAL && id <= RIG_TOKEN_STRING_DEF) ||
		(id >= RIG_TOKEN_SETVALUE && id <= RIG_TOKEN_ALL_NOTES_OFF);
	rig_token.is_button_statement =
		(id >= RIG_TOKEN_SETVALUE && id <= RIG_TOKEN_FTP_SENSITIVITY);
	rig_token.is_led_color =
		(id >= RIG_TOKEN_LED_BLACK && id <= RIG_TOKEN_LED_CYAN);
	rig_token.is_display_color =
		(id >= RIG_TOKEN_DISPLAY_BLACK && id <= RIG_TOKEN_DISPLAY_PINK);
	rig_token.is_bin_op =
		(id >= RIG_TOKEN_PLUS && id <= RIG_TOKEN_EQUALS);

	display(dbg_token,"",0);
	if (rig_token.id == RIG_TOKEN_NUMBER)
		display(dbg_token,"getToken(%d:%d) %d=%s -->  %d",
			rig_token.line_num,
			rig_token.char_num,
			rig_token.id,
			rigTokenToString(rig_token.id),
			rig_token.int_value);
	else if (rig_token.id == RIG_TOKEN_IDENTIFIER || rig_token.id == RIG_TOKEN_TEXT)
		display(dbg_token,"getToken(%d:%d) %d=%s -->  %s",
			rig_token.line_num,
			rig_token.char_num,
			rig_token.id,
			rigTokenToString(rig_token.id),
			rig_token.text);
	else
		display(dbg_token,"getToken(%d:%d) %d=%s",
			rig_token.line_num,
			rig_token.char_num,
			rig_token.id,
			rigTokenToString(rig_token.id),
			rig_token.is_bin_op);

	// was going too fast for console

	if (dbg_token <= 0)
		delay(5);

    return rig_token.id;
}



//-------------------------------------------------
// dumpRigCode()
//-------------------------------------------------

// extern
void dumpRigCode(int code_len, uint8_t *code_buf)
{

	dbgSerial->print("# ");
	dbgSerial->print(rig_name);
	dbgSerial->println(".rig");
	dbgSerial->println();

	bool at_newline = 1;	// whether we are at a newline
	int in_subsection = 0;	// whether we are in a subsection
	bool colon_line = 0;		// print a new line after the next colon
	bool started = 0;		// we have encountered the first button

	int rig_code_ptr = 0;

	while (rig_code_ptr < code_len)
	{
		int tt = code_buf[rig_code_ptr++];

		if (tt == RIG_TOKEN_BASERIG ||
			tt == RIG_TOKEN_OVERLAY)
		{
			dbgSerial->print(rigTokenToText(tt));
			dbgSerial->println();
			dbgSerial->println();
			continue;
		}
		if (!started && (
			tt == RIG_TOKEN_LISTEN ||
			tt == RIG_TOKEN_STRING ||
			tt == RIG_TOKEN_AREA ))
		{
			colon_line = 0;
		}
		else if (tt == RIG_TOKEN_BUTTON )
		{
			started = 1;
			colon_line = 1;					// cr after next colon
			in_subsection = 0;
			dbgSerial->println();			// extra cr
		}
		else if (
			tt == RIG_TOKEN_COLOR ||
			tt == RIG_TOKEN_BLINK ||
			tt == RIG_TOKEN_PRESS ||
			tt == RIG_TOKEN_CLICK ||
			tt == RIG_TOKEN_LONG ||
			tt == RIG_TOKEN_RELEASE ||
			tt == RIG_TOKEN_REPEAT )
		{
			in_subsection = 1;
			colon_line = 1;				// cr + 8 spaces after next colon
			dbgSerial->print("    ");
		}


		if (at_newline)
		{
			at_newline = 0;
			if (in_subsection == 2)
				dbgSerial->print("        ");
		}


		if (tt == RIG_TOKEN_TEXT)
		{
			int len = strlen((char *) &code_buf[rig_code_ptr]);
			dbgSerial->print("\"");
			dbgSerial->print((char *) &code_buf[rig_code_ptr]);
			dbgSerial->print("\"");
			rig_code_ptr += len + 1;
		}
		else if (tt == RIG_TOKEN_NUMBER)
		{
			int16_t *p = (int16_t *) &code_buf[rig_code_ptr];
			dbgSerial->print(*p);
			rig_code_ptr += 2;
		}
		else
		{
			if ((!colon_line && tt == RIG_TOKEN_COLON) ||
				tt == RIG_TOKEN_ASSIGN ||
				tt == RIG_TOKEN_PLUS ||
				tt == RIG_TOKEN_BITWISE_OR ||
				tt == RIG_TOKEN_BITWISE_AND ||
				tt == RIG_TOKEN_LOGICAL_OR ||
				tt == RIG_TOKEN_LOGICAL_AND ||
				tt == RIG_TOKEN_QUESTION_MARK)
				dbgSerial->print(" ");
			dbgSerial->print(rigTokenToText(tt));
			if (tt == RIG_TOKEN_COMMA ||
				tt == RIG_TOKEN_NOT ||
				tt == RIG_TOKEN_COLON ||
				tt == RIG_TOKEN_ASSIGN ||
				tt == RIG_TOKEN_PLUS ||
				tt == RIG_TOKEN_BITWISE_OR ||
				tt == RIG_TOKEN_BITWISE_AND ||
				tt == RIG_TOKEN_LOGICAL_OR ||
				tt == RIG_TOKEN_LOGICAL_AND ||
				tt == RIG_TOKEN_QUESTION_MARK)
				dbgSerial->print(" ");
			if (tt == RIG_TOKEN_SEMICOLON)
			{
				at_newline = 1;
				dbgSerial->println();
			}
		}

		if (colon_line && tt == RIG_TOKEN_COLON)
		{
			dbgSerial->println();
			colon_line = 0;
			at_newline = 1;
			if (in_subsection)
				in_subsection = 2;
		}
	}

	dbgSerial->println();
	dbgSerial->println();
	dbgSerial->print("# end of ");
	dbgSerial->print(rig_name);
	dbgSerial->println(".rig");
	dbgSerial->println();

}


//-------------------------------------------------
// parser
//-------------------------------------------------

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

	rig_text_len = 0;

    char name_buffer[128];
    strcpy(name_buffer,"/");
    strcat(name_buffer,name);
    strcat(name_buffer,".rig");
    display(0,"openRigFile(%s)",name);

	rig_file = SD.open(name_buffer, FILE_READ );
    if (!rig_file)
    {
        rig_error("Could not open Rig file: %s",name_buffer);
        return 0;
    }

    rig_text_len = rig_file.size();
    display(0,"Rig file(%s) opened - length=%d",name,rig_text_len);
	return 1;
}
