//-------------------------------------------------------
// rigParser.cpp
//-------------------------------------------------------

#define WITH_DEFAULT_RIG  1

#include <myDebug.h>
#include "rigParser.h"
#include "myTFT.h"		// for TFT sizes
#include "theSystem.h"	// for client rect
#include "rigExpression.h"
#include "rigDump.h"
#if WITH_DEFAULT_RIG
	#include "default.rig.h"
	#include "default_modal.rig.h"
#endif


#define dbg_parse 	0
	// 0 = just show ParsingRig() and finished messages
	// -1 = show statements and params
	// -2 = show details


#define MAX_RIG_NAME		31
#define RIG_POOL_SIZE		(MAX_RIG_SIZE * 2)
#define AREA_CLIENT_HEIGHT  (client_rect.ye - client_rect.ys + 1)


static uint8_t parse_pool[MAX_RIG_SIZE];
static rig_t *parse_rig = (rig_t *) parse_pool;

static bool any_end_modal;

static uint16_t rig_pool_len;
static uint8_t  rig_pool[RIG_POOL_SIZE];
static const rig_t *base_rig = (const rig_t *) rig_pool;



static void init_parse()
{
	any_end_modal = 0;
	rig_error_found = 0;	 // in rigToken.cpp

	display(0,"MAX_RIG_SIZE = %d", MAX_RIG_SIZE);

	memset(parse_pool,0,MAX_RIG_SIZE);

	uint16_t offset = sizeof(rig_t);
	parse_rig->define_pool = (char *) &parse_pool[offset];
	offset += MAX_DEFINE_POOL;
	parse_rig->string_pool = (char *) &parse_pool[offset];
	offset += MAX_STRING_POOL;
	parse_rig->statement_pool = &parse_pool[offset];
	offset += MAX_STATEMENT_POOL;
	parse_rig->expression_pool = &parse_pool[offset];
}


static rig_t *relocate()
{
	uint16_t rig_size = sizeof(rig_t) +
		parse_rig->define_pool_len +
		parse_rig->string_pool_len +
		parse_rig->statement_pool_len +
		parse_rig->expression_pool_len;

	uint16_t relocate_to = (parse_rig->rig_type & RIG_TYPE_MODAL) ?
		rig_pool_len : 0;

	display(0,"relocate(%s, bytes=%d) to %d",
		parse_rig->rig_type & RIG_TYPE_MODAL ? "ModalRig":"BaseRig",
		rig_size,
		relocate_to);

	if (relocate_to + rig_size >= RIG_POOL_SIZE)
	{
		rig_error("RIG(%d) to big to fit in remaining POOL(%d)",rig_size,RIG_POOL_SIZE-relocate_to);
		return 0;
	}

	rig_t *new_rig = (rig_t *) &rig_pool[relocate_to];
	memcpy(new_rig,parse_rig,sizeof(rig_t));
	relocate_to += sizeof(rig_t);

	new_rig->define_pool =(char *) &rig_pool[relocate_to];;
	memcpy(&rig_pool[relocate_to],parse_rig->define_pool,parse_rig->define_pool_len);
	relocate_to += parse_rig->define_pool_len;

	new_rig->string_pool = (char *) &rig_pool[relocate_to];
	memcpy(&rig_pool[relocate_to],parse_rig->string_pool,parse_rig->string_pool_len);
	relocate_to += parse_rig->string_pool_len;

	new_rig->statement_pool = &rig_pool[relocate_to];
	memcpy(&rig_pool[relocate_to],parse_rig->statement_pool,parse_rig->statement_pool_len);
	relocate_to += parse_rig->statement_pool_len;

	new_rig->expression_pool = &rig_pool[relocate_to];
	memcpy(&rig_pool[relocate_to],parse_rig->expression_pool,parse_rig->expression_pool_len);
	relocate_to += parse_rig->expression_pool_len;

	rig_pool_len = relocate_to;
	return new_rig;
}


//-----------------------------------------------------
// Arguments
//-----------------------------------------------------

const char *argTypeToString(int i)
{
	switch (i)
	{
		case PARAM_DEFINE_NUM		: return "DEFINE_NUM";
		case PARAM_USER_IDENT   	: return "IDENTIFIER";
		case PARAM_DEFINE_VALUE 	: return "DEFINE_VALUE";
		case PARAM_STRING_NUM   	: return "STRING_NUM";
		case PARAM_TEXT  			: return "TEXT";

		case PARAM_AREA_NUM     	: return "AREA_NUM";
		case PARAM_FONT_SIZE    	: return "FONT_SIZE";
		case PARAM_FONT_TYPE    	: return "FONT_TYPE";
		case PARAM_FONT_JUST    	: return "FONT_JUST";
		case PARAM_START_X      	: return "START_X";
		case PARAM_START_Y      	: return "START_Y";
		case PARAM_END_X        	: return "END_X";
		case PARAM_END_Y        	: return "END_Y";

		case PARAM_PEDAL_NUM 		: return "PEDAL_NUM";
		case PARAM_PEDAL_NAME   	: return "PEDAL_NAME";
		case PARAM_ROTARY_NUM 		: return "ROTARY_NUM";

		case PARAM_VALUE_NUM    	: return "VALUE_NUM";
		case PARAM_VALUE			: return "VALUE";

		case PARAM_MIDI_PORT    	: return "MIDI_PORT";
		case PARAM_MIDI_CHANNEL 	: return "MIDI_CHANNEL";
		case PARAM_LISTEN_CHANNEL	: return "LISTEN_CHANNEL";
		case PARAM_MIDI_CC      	: return "MIDI_CC";
		case PARAM_MIDI_VALUE		: return "MIDI_VALUE";

		case PARAM_RIG_NAME			: return "RIG_NAME";

		case PARAM_STRING_EXPRESSION			: return "STRING_EXPRESSION";
		case PARAM_LED_COLOR_EXPRESSION			: return "LED_COLOR_EXPRESSION";
		case PARAM_DISPLAY_COLOR_EXPRESSION		: return "DISPLAY_COLOR_EXPRESSION";
	}
	return "unknownArgType";
}


// init_header only statements

static const int DEFINE_ARGS[] = {		// DEFINE(0, _bank,  1);
	PARAM_DEFINE_NUM,
	PARAM_USER_IDENT,
	PARAM_DEFINE_VALUE,
	0
};
static const int STRING_DEF_ARGS[] = {		// STRING_DEF(0, "BASS1");
	PARAM_STRING_NUM,
	PARAM_TEXT,
	0
};

// init only statements

static const int AREA_ARGS[] = {			// AREA(0, 32, BOLD, LEFT, 5, 5, 299, 40);
	PARAM_AREA_NUM,
	PARAM_FONT_SIZE,
	PARAM_FONT_TYPE,
	PARAM_FONT_JUST,
	PARAM_START_X,
	PARAM_START_Y,
	PARAM_END_X,
	PARAM_END_Y,
	0
};
static const int LISTEN_ARGS[] = {			// LISTEN(37, SERIAL, 0, 20);
	PARAM_VALUE_NUM,
	PARAM_MIDI_PORT,
	PARAM_LISTEN_CHANNEL,
	PARAM_MIDI_CC,
	0
};

// generic statements

static const int PEDAL_ARGS[] = {		// PEDAL(0, "Synth", MIDI, 1, 7);
	PARAM_PEDAL_NUM,
	PARAM_PEDAL_NAME,
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_MIDI_CC,
	0
};
static const int ROTARY_ARGS[] = {		// ROTARY(0, SERIAL, 1, LOOP_CONTROL_BASE + n);
	PARAM_ROTARY_NUM,
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_MIDI_CC,
	0
};

static const int SETVALUE_ARGS[] = {		// setValue(20, VALUE[0] + 2);
	PARAM_VALUE_NUM,
	PARAM_VALUE,
	0 };

static const int DISPLAY_ARGS[] = {			// display(AREA, VALUE[0] ? BLUE : BLACK, "test");
	PARAM_AREA_NUM,
	PARAM_DISPLAY_COLOR_EXPRESSION,
	PARAM_STRING_EXPRESSION,
	0 };

static const int SEND_CC_ARGS[] = {			// sendCC(SERIAL, 1, 192, VALUE[3]);
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_MIDI_CC,
	PARAM_MIDI_VALUE,						// control value
	0
};

static const int SEND_PGM_CHG_ARGS[] = {	// sendPgmChg(SERIAL, 1, VALUE[3]);
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_MIDI_VALUE,						// patch number value
	0
};

static const int NOTE_ON_ARGS[] = {			// noteOn(MIDI0, 9, VALUE[3], VALUE[4]);
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_MIDI_VALUE,						// note number value
	PARAM_MIDI_VALUE,						// velocity value
	0
};

static const int NOTE_OFF_ARGS[] = {		// noteOff(MIDI0, 9, VALUE[4]);
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_MIDI_VALUE,						// note number value
	0
};

static const int ALL_NOTE_OFF_ARGS[] = {	// AllNotesOff(MIDI0, 9,);
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	0
};

static const int LOAD_RIG_ARGS[] = {		// LoadRig("modal")
	PARAM_RIG_NAME,
	0
};

static const int END_MODAL_ARGS[] = {		// LoadRig("modal")
	PARAM_VALUE_NUM,
	PARAM_VALUE,
	0
};

static const int NO_ARGS[] = {
	0
};



static const statement_param_t statement_params[] = {
	{ RIG_TOKEN_DEFINE_DEF,		DEFINE_ARGS },
	{ RIG_TOKEN_STRING_DEF, 	STRING_DEF_ARGS },

	{ RIG_TOKEN_AREA, 			AREA_ARGS },
	{ RIG_TOKEN_LISTEN, 		LISTEN_ARGS },

	{ RIG_TOKEN_PEDAL, 			PEDAL_ARGS },
	{ RIG_TOKEN_ROTARY, 		ROTARY_ARGS },
	{ RIG_TOKEN_SETVALUE, 		SETVALUE_ARGS },
	{ RIG_TOKEN_DISPLAY, 		DISPLAY_ARGS },
	{ RIG_TOKEN_SEND_CC, 		SEND_CC_ARGS },
	{ RIG_TOKEN_SEND_PGM_CHG,	SEND_PGM_CHG_ARGS },
	{ RIG_TOKEN_NOTE_ON,		NOTE_ON_ARGS },
	{ RIG_TOKEN_NOTE_OFF,		NOTE_OFF_ARGS },
	{ RIG_TOKEN_ALL_NOTES_OFF,	ALL_NOTE_OFF_ARGS },

	{ RIG_TOKEN_LOAD_RIG,		LOAD_RIG_ARGS },
	{ RIG_TOKEN_END_MODAL,		END_MODAL_ARGS },

	{ RIG_TOKEN_FTP_TUNER,		NO_ARGS },
	{ RIG_TOKEN_FTP_SENSITIVITY,NO_ARGS },
	{ 0, 0 } };


const statement_param_t *findParams(int tt)
{
	const statement_param_t *ptr = statement_params;
	while (ptr->id && ptr->id != tt)
		ptr++;
	if (!ptr->id)
		rig_error("unknown statement %s",rigTokenToString(tt));
	return ptr;
}


//-------------------------------------------
// Pool accessors
//-------------------------------------------

static bool addDefinePool(const char *s)
{
	int len = strlen(s);
	if (parse_rig->define_pool_len >= MAX_DEFINE_POOL - len - 1)
	{
		rig_error("DEFINE POOL OVERLFLOW");
		return false;
	}
	char *pool = (char *) parse_rig->define_pool;
	strcpy(&pool[parse_rig->define_pool_len],s);
	parse_rig->define_pool_len += len + 1;
	return true;
}

static bool addStringPool(const char *s)
{
	int len = strlen(s);
	if (parse_rig->string_pool_len >= MAX_STRING_POOL - len - 1)
	{
		rig_error("STRING POOL OVERLFLOW");
		return false;
	}
	char *pool = (char *) parse_rig->string_pool;
	strcpy(&pool[parse_rig->string_pool_len],s);
	parse_rig->string_pool_len += len + 1;
	return true;
}

static bool addStatementByte(uint8_t byte)
{
	if (parse_rig->statement_pool_len >= MAX_STATEMENT_POOL)
	{
		rig_error("STATMENT(BYTE) POOL OVERLFLOW");
		return false;
	}
	uint8_t *pool = (uint8_t *) parse_rig->statement_pool;
	pool[parse_rig->statement_pool_len++] = byte;
	return true;
}

static bool addStatementInt(uint16_t i)
{
	if (parse_rig->statement_pool_len >= MAX_STATEMENT_POOL - 2)
	{
		rig_error("STATMENT(INT) POOL OVERLFLOW");
		return false;
	}
	uint8_t *pool = (uint8_t *) parse_rig->statement_pool;
	uint16_t *ptr = (uint16_t *) &pool[parse_rig->statement_pool_len];
	*ptr = i;
	parse_rig->statement_pool_len += 2;
	return true;
}



//--------------------------------------------------------
// token primitives
//--------------------------------------------------------
// we are ALWAYS looking at the current token
// and then get another one as needed ..

static bool skip(int tt, bool may_be_end = 0)
{
	if (rig_token.id != tt)
	{
		rig_error("expected %s got %s",rigTokenToString(tt),rigTokenToString(rig_token.id));
		return false;
	}
	if (!getRigToken() && !may_be_end)
		return false;
	return true;
}

static bool getNumberAny(int *retval, const char *what)
{
	if (rig_token.id != RIG_TOKEN_NUMBER)
	{
		rig_error("%s Expected",what);
		return false;
	}
	*retval = rig_token.int_value;
	display(dbg_parse + 1,"%s = %d",what,*retval);

	if (!getRigToken())
		return false;
	return true;
}

static bool getNumber(int *retval, const char *what, int max)
{
	if (rig_token.id != RIG_TOKEN_NUMBER)
	{
		rig_error("%s Expected",what);
		return false;
	}
	if (rig_token.int_value > max)
	{
		rig_error("%s must be less than %d",what,max);
		return false;
	}
	*retval = rig_token.int_value;
	display(dbg_parse + 1,"%s = %d",what,*retval);

	if (!getRigToken())
		return false;
	return true;
}




static const char *getText(const char *what, int max_len)
{
	static char buf[MAX_RIG_TOKEN+1];

	if (rig_token.id != RIG_TOKEN_TEXT)
	{
		rig_error("%s Expected",what);
		return 0;
	}

	int len = strlen(rig_token.text);
	if (len > max_len)
	{
		rig_error("%s limited to %d bytes");
		return 0;
	}

	strcpy(buf,rig_token.text);
	display(dbg_parse + 1,"%s = \"%s\"",what,buf);

	if (!getRigToken())
		return 0;
	return buf;
}


static const char *getUserIdent()
{
	static char buf[MAX_RIG_TOKEN+1];

	if (rig_token.id != RIG_TOKEN_IDENTIFIER)
	{
		rig_error("IDENTIFIER Expected",0);
		return 0;
	}

	strcpy(buf,rig_token.text);
	display(dbg_parse + 1,"IDENTIFER = %s",buf);

	if (!getRigToken())
		return 0;
	return buf;
}


//--------------------------------------------------------
// token handlers
//--------------------------------------------------------


static bool handleArg(int statement_type, int arg_type)
{
	display(dbg_parse + 2,"handleArg(%s) token=%s",argTypeToString(arg_type),rigTokenToString(rig_token.id));
	proc_entry();

	int value;
	const char *text;
	bool ok = 1;

	static int define_num;
	static int string_num;

	if (IS_INIT_HEADER_STATEMENT(statement_type))
	{
		switch (arg_type)
		{
			case PARAM_DEFINE_NUM :
				if (!getNumber(&value,"VALUE_NUM",RIG_NUM_DEFINES-1))
					ok = 0;
				else if (parse_rig->define_ids[value])
				{
					rig_error("define(%d) used more than once",value);
					ok = 0;
				}
				else
				{
					define_num = value;
				}
				break;
			case PARAM_USER_IDENT :
				text = getUserIdent();
				if (!text)
					ok = 0;
				else
				{
					// the offset is incremented so that we can identify
					// accesses to string 0 explicitly.
					parse_rig->define_ids[define_num] = parse_rig->define_pool_len + 1;
					ok = addDefinePool(text);
				}
				break;
			case PARAM_DEFINE_VALUE :
				if (!getNumber(&value,"DEFINE_VALUE",MAX_DEFINE_VALUE))
					ok = 0;
				else
					parse_rig->define_values[define_num] = value;
				break;
			case PARAM_STRING_NUM   :
				if (!getNumber(&value,"STRING_NUM",RIG_NUM_STRINGS-1))
					ok = 0;
				else if (parse_rig->strings[value])
				{
					rig_error("define_string(%d) used more than once",value);
					ok = 0;
				}
				else
					string_num = value;
				break;
			case PARAM_TEXT  :
				text = getText("TEXT",MAX_RIG_TOKEN);
				if (!text)
					ok = 0;
				else
				{
					// the offset is incremented so that we can identify
					// accesses to string 0 explicitly.
					parse_rig->strings[string_num] = parse_rig->string_pool_len + 1;
					ok = addStringPool(text);
				}
				break;

			default:
				rig_error("implementation error: unknown init_header param %d",arg_type);
				break;
		}
	}

	else  // REGULAR STATEMENTS store values in line with commas and a paren
	{
		switch (arg_type)
		{
			// AREA(areaExpression, FONT_SIZE, FONT_TYPE, STARTX, STARTY, ENDX, ENDY )

			case PARAM_AREA_NUM :
				value = rigAreaNumExpression(parse_rig,rig_token.id);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_FONT_SIZE :
				if (!getNumberAny(&value,"FONT_SIZE"))
					ok = 0;
				else if (value != 12 &&
						 value != 14 &&
						 value != 16 &&
						 value != 18 &&
						 value != 20 &&
						 value != 24 &&
						 value != 28 &&
						 value != 32 &&
						 value != 40 &&
						 value != 48)
				{
					rig_error("FONT_SIZE(%d) must be 12, 14, 16, 18, 20, 24, 28, 32, 40, or 48");
					ok = 0;
				}
				else
				{
					ok = ok && addStatementByte(value);
				}
				break;
			case PARAM_FONT_TYPE    :
				if (rig_token.id < RIG_TOKEN_NORMAL ||
					rig_token.id > RIG_TOKEN_BOLD)
				{
					rig_error("FONT_TYPE must be NORMAL or BOLD");
					ok = 0;
				}
				else
				{
					uint8_t use_type = rig_token.id - RIG_TOKEN_NORMAL;
					display(dbg_parse + 1, "FONT_TYPE = %s (%d)",rigTokenToString(rig_token.id),use_type);
					ok = addStatementByte(use_type);
					ok = ok && getRigToken();
				}
				break;
			case PARAM_FONT_JUST    :
				if (rig_token.id < RIG_TOKEN_LEFT ||
					rig_token.id > RIG_TOKEN_RIGHT)
				{
					rig_error("FONT_JUST must be LEFT, CENTER, or RIGHT");
					ok = 0;
				}
				else
				{
					uint8_t use_just = rig_token.id - RIG_TOKEN_LEFT;
					display(dbg_parse + 1, "FONT_JUST = %s (%d)",rigTokenToString(rig_token.id),use_just);
					ok = addStatementByte(use_just);
					ok = ok && getRigToken();
				}
				break;
			case PARAM_START_X      :
				ok = getNumber(&value,"START_X",TFT_WIDTH-1);
				ok = ok && addStatementInt(value);
				break;
			case PARAM_START_Y      :
				ok = getNumber(&value,"START_Y",AREA_CLIENT_HEIGHT-1);
				ok = ok && addStatementInt(value);
				break;
			case PARAM_END_X        :
				ok = getNumber(&value,"END_X",TFT_WIDTH-1);
				ok = ok && addStatementInt(value);
				break;
			case PARAM_END_Y        :
				ok = getNumber(&value,"END_Y",AREA_CLIENT_HEIGHT-1);
				ok = ok && addStatementInt(value);
				break;

			// LISTEN and setValue

			case PARAM_VALUE_NUM :	// in setValue and LISTEN
				value = rigValueNumExpression(parse_rig,rig_token.id);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_VALUE :	    // in setValue
				value = rigValueExpression(parse_rig,rig_token.id);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;


			case PARAM_RIG_NAME  :
				text = getText("RIG_NAME",MAX_RIG_NAME);
				if (!text)
					ok = 0;
				else
				{
					for (uint16_t i=0; i<strlen(text) && ok; i++)
					{
						if (!((text[i] == '-' || text[i] == '.' || text[i] == '_' ) ||
							  (text[i] >= 'A' && text[i] <= 'Z') ||
							  (text[i] >= 'a' && text[i] <= 'z') ||
							  (text[i] >= '0' && text[i] <= '9') ))
						{
							rig_error("RIG_NAME(%s) may only contain A-Z,a-z,0-9,dash,period, or underscore characters");
							ok = 0;
						}
					}

					if (ok)
					{
						ok = ok && addStatementInt(parse_rig->string_pool_len);
						ok = ok && addStringPool(text);
					}
				}
				break;

			// PEDAL

			case PARAM_PEDAL_NUM :
				value = rigPedalNumExpression(parse_rig,rig_token.id);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_PEDAL_NAME   :
				text = getText("PEDAL_NAME",MAX_PEDAL_NAME);
				ok = ok && text;
				ok = ok && addStatementInt(parse_rig->string_pool_len);
				ok = ok && addStringPool(text);
				break;
			case PARAM_ROTARY_NUM :
				value = rigRotaryNumExpression(parse_rig,rig_token.id);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			// in a variety of calls

			case PARAM_MIDI_PORT    :
				if (!IS_MIDI_PORT(rig_token.id))
				{
					rig_error("MIDI_PORT must be MIDI0, MIDI1, or SERIAL");
					ok = 0;
				}
				else
				{
					uint8_t use_port = rig_token.id - RIG_TOKEN_USB1;
					display(dbg_parse + 1, "MIDI_PORT = %s (%d)",rigTokenToString(rig_token.id),use_port);
					ok = addStatementByte(use_port);
					ok = ok && getRigToken();
				}
				break;
			case PARAM_MIDI_CHANNEL :
				value = rigMidiChannelExpression(parse_rig,rig_token.id);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_LISTEN_CHANNEL :
				value = rigListenChannelExpression(parse_rig,rig_token.id);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_MIDI_CC :
			case PARAM_MIDI_VALUE :
				value = rigMidiValueExpression(parse_rig,rig_token.id);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;

			// Generic Expression

			case PARAM_STRING_EXPRESSION :
				value = rigStringExpression(parse_rig,rig_token.id);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_LED_COLOR_EXPRESSION :
				value = rigLedColorExpression(parse_rig,rig_token.id);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_DISPLAY_COLOR_EXPRESSION	:
				value = rigDisplayColorExpression(parse_rig,rig_token.id);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;

			default:
				rig_error("implementation error: unknown statement param %d",arg_type);
				break;
		}
	}

	proc_leave();
	if (!ok)
		return false;
	display(dbg_parse + 2,"handleArg(%s) finished",argTypeToString(arg_type));
	return true;
}



static bool handleStatement(int tt)
{
	display(dbg_parse + 1,"handleStatement(%s)",rigTokenToString(tt));
	proc_entry();

	int statement_type = tt;

	// note if we have gotten any endModal statements in this parse

	any_end_modal = (statement_type == RIG_TOKEN_END_MODAL);

	// this parses either statements that actually generate statement rig_code,
	// or init only statements that store things in the rig_header. If it is
	// not an init_only statement, we write the token as the first byte
	// of the statement

	if (!IS_INIT_HEADER_STATEMENT(statement_type) && !addStatementByte(tt))
	{
		proc_leave();
		return false;
	}

	// get the next token

	if (!getRigToken())
	{
		proc_leave();
		return false;
	}
	if (!skip(RIG_TOKEN_LEFT_PAREN))
	{
		proc_leave();
		return false;
	}

	const statement_param_t *params = findParams(tt);
	if (!params)
	{
		proc_leave();
		return false;
	}

	const int *arg = params->args;
	while (*arg)
	{
		if (!handleArg(statement_type, *arg++))
		{
			proc_leave();
			return false;
		}
		if (*arg && !skip(RIG_TOKEN_COMMA))
		{
			proc_leave();
			return false;
		}
	}
	if (!skip(RIG_TOKEN_RIGHT_PAREN))
	{
		proc_leave();
		return false;
	}

	// the parse WILL end on an eof in the button section
	// at the semi-colon at the end of a statement.

	if (!skip(RIG_TOKEN_SEMICOLON, parse_section == 2))
	{
		proc_leave();
		return false;
	}

	display(dbg_parse + 2,"handleStatement() finished",0);
	proc_leave();
	return true;
}



static bool handleStatementList(int tt)
{
	display(dbg_parse + 2,"handleStatementList(%d) %s",parse_rig->num_statements,rigTokenToString(tt));
	proc_entry();

	// set pool offset into statements array
	// allow one for terminating length

	if (parse_rig->num_statements >= MAX_STATEMENTS - 1)
	{
		rig_error("implementation error: too many STATMENTS (lists)");
		proc_leave();
		return false;
	}
	parse_rig->statements[parse_rig->num_statements++] = parse_rig->statement_pool_len;

	// process statements

	while (IS_STATEMENT(tt))
	{
		if (!handleStatement(tt))
		{
			rig_error("handleStatement() failed!");
			proc_leave();
			return false;
		}
		tt = rig_token.id;
		display(dbg_parse + 2,"back from handleStatement() token=%s",rigTokenToString(tt));
	}

	proc_leave();
	display(dbg_parse + 2,"handleStatementList() finished on %s",rigTokenToString(tt));
	return true;
}



static bool handleSubsection(int button_num, int sub_id)
{
	bool ok = 1;
	int sub_num = SUBSECTION_NUM(sub_id);
	display(dbg_parse + 1,"handleSubsection(%d,%s)",button_num,rigTokenToText(sub_id));
	proc_entry();

	ok = getRigToken();					// skip the ID
	ok = ok && skip(RIG_TOKEN_COLON);	// skip the colon

	if (sub_id >= RIG_TOKEN_PRESS)
	{
		if (!IS_BUTTON_STATEMENT(rig_token.id))
		{
			rig_error("Button statement expected");
			proc_leave();
			return false;
		}

		// set this button's statement list index for the given subsection
		// to the next statement list that will be parsed.

		display(dbg_parse + 1,"    uses statement(%d)",parse_rig->num_statements);
		display(dbg_parse + 1,"    button_ref set to %d",parse_rig->num_statements+1);

		parse_rig->button_refs[button_num][sub_num] = parse_rig->num_statements + 1;
			// 1 based so that we can identify a used statement list

		ok = handleStatementList(rig_token.id);
	}
	else if (sub_id == RIG_TOKEN_COLOR)
	{
		uint16_t offset = rigLedColorExpression(parse_rig,rig_token.id);
		ok = offset;
		if (ok)
			parse_rig->button_refs[button_num][sub_num] = offset;
		ok = ok && skip(RIG_TOKEN_SEMICOLON);
	}
	else	// BLINK
	{
		uint16_t offset = rigNumericExpression(parse_rig,rig_token.id);
		ok = offset;
		if (ok)
			parse_rig->button_refs[button_num][sub_num] = offset;
		ok = ok && skip(RIG_TOKEN_SEMICOLON);
	}

	proc_leave();
	if (ok)
		display(dbg_parse + 2,"handleSubsection(%d,%s) finished",button_num,rigTokenToText(sub_id));
	return ok;
}


static bool handleSubsections(int button_num)
{
	bool done[NUM_SUBSECTIONS];
	for (int i=0; i<NUM_SUBSECTIONS; i++)
		done[i] =0;

	display(dbg_parse + 2,"handleSubsections(%d) %s",button_num,rigTokenToText(rig_token.id));

	bool ok = 1;
	while (ok && IS_SUBSECTION(rig_token.id))
	{
		if (done[SUBSECTION_NUM(rig_token.id)])
		{
			rig_error("Multiple definitions of BUTTON Subsection(%s)",rigTokenToString(rig_token.id));
			ok = 0;
		}
		else
		{
			done[SUBSECTION_NUM(rig_token.id)] = 1;
			ok = handleSubsection(button_num,rig_token.id);
		}
	}

	if (ok)
		display(dbg_parse + 2,"handleSubsections(%d) finished",button_num);
	return ok;
}


static bool handleButton()
{
	display(dbg_parse + 1,"handleButton()",0);
	proc_entry();

	bool ok = getRigToken();
	ok = ok && skip(RIG_TOKEN_LEFT_PAREN);
	if (ok && rig_token.id != RIG_TOKEN_NUMBER)
	{
		ok = 0;
		rig_error("BUTTON number expected");
	}
	int button_num = rig_token.int_value;
	if (button_num < 0 || button_num >= NUM_BUTTONS)
	{
		ok = 0;
		rig_error("BUTTON number must be between 0 and 24");
	}
	if (ok)
	{
		display(dbg_parse + 1,"BUTTON_NUM = %d",button_num);

		ok = getRigToken();		// skip the BUTTON number
		ok = ok && skip(RIG_TOKEN_RIGHT_PAREN);
		ok = ok && skip(RIG_TOKEN_COLON);

		bool is_inherit = rig_token.id == RIG_TOKEN_INHERIT;

		if (ok && is_inherit)
		{
			display(dbg_parse,"    INHERIT(%d)",button_num);
			parse_rig->button_refs[button_num][0] = BUTTON_INHERIT_FLAG;
			ok = getRigToken();							// skip the INHERIT
			ok = ok && skip(RIG_TOKEN_SEMICOLON, true);	// may end on this
		}
		else
		{
			if (ok && !IS_SUBSECTION(rig_token.id))
			{
				ok = 0;
				rig_error("BUTTON subsection expected");
			}
			if (ok)
				ok = handleSubsections(button_num);
		}
	}

	proc_leave();
	if (ok)
		display(dbg_parse + 2,"handleButton(%d) finished",button_num);
	return ok;
}




//---------------------------------------
// loadDefaultRig()
//---------------------------------------

#if WITH_DEFAULT_RIG

	const rig_t *loadDefaultRig(const char *rig_name)
	{
		display(dbg_parse,"loadDefaultRig(%s)",rig_name);

		bool is_base_rig = !strcmp(rig_name,DEFAULT_RIG_NAME);
		uint16_t offset = is_base_rig ? 0 : rig_pool_len;
		const rig_t *src_rig = is_base_rig ?
			&default_rig : &default_modal_rig;

		uint16_t size = sizeof(rig_t) +
			src_rig->define_pool_len +
			src_rig->string_pool_len +
			src_rig->statement_pool_len +
			src_rig->expression_pool_len;
		memset(&rig_pool[offset],0,size);

		rig_t *rig = (rig_t *) &rig_pool[offset];
		memcpy(&rig_pool[offset],src_rig,sizeof(rig_t));
		offset += sizeof(rig_t);

		size = src_rig->define_pool_len;
		memcpy(&rig_pool[offset],src_rig->define_pool,size);
		rig->define_pool = (char *) &rig_pool[offset];
		offset += size;

		size = src_rig->string_pool_len;
		memcpy(&rig_pool[offset],src_rig->string_pool,size);
		rig->string_pool = (char *) &rig_pool[offset];
		offset += size;

		size = src_rig->statement_pool_len;
		memcpy(&rig_pool[offset],src_rig->statement_pool,size);
		rig->statement_pool = &rig_pool[offset];
		offset += size;

		size = src_rig->expression_pool_len;
		memcpy(&rig_pool[offset],src_rig->expression_pool,size);
		rig->expression_pool = &rig_pool[offset];
		offset += size;

		rig_pool_len = offset;

		proc_leave();
		return rig;
	}
#endif




//--------------------------------------------------------
// parseRig
//--------------------------------------------------------


// extern
const rig_t *parseRig(const char *rig_name)
	// generates the intermediate structure of a
	// syntactically valid set of bytes that are
	// RIG_ID's, and with inline TEXT and int_values;

{
	warning(dbg_parse,"ParseRig(%s.rig)",rig_name);

	#if WITH_DEFAULT_RIG
		if (!strcmp(rig_name,DEFAULT_RIG_NAME) || (
			rig_pool_len &&
			(base_rig->rig_type & RIG_TYPE_SYSTEM) &&
			!strcmp(rig_name,DEFAULT_MODAL_NAME)))
		{
			return loadDefaultRig(rig_name);
		}
	#endif

	bool ok = 0;
	rig_t *new_rig = 0;
	if (openRigFile(rig_name))
	{
		ok = 1;

		init_parse();
		int tt = getRigToken();

		// rig type

		if (tt < RIG_TOKEN_BASERIG ||
			tt > RIG_TOKEN_MODAL)
		{
			rig_error("must start with BaseRig or Overlay");
			ok = 0;
		}
		else if (tt == RIG_TOKEN_MODAL)
		{
			parse_rig->rig_type |= RIG_TYPE_MODAL;
		}

		tt = getRigToken();
		if (!tt)
		{
			rig_error("unexpected end of program");
			ok = false;
		}

		// init_statements

		if (ok && IS_INIT_STATEMENT(tt))
			ok = ok && handleStatementList(tt);
		parse_rig->num_statements = 1;
			// even if empty

		// buttons

		tt = rig_token.id;
		if (ok && tt != RIG_TOKEN_BUTTON)
		{
			if (tt == RIG_TOKEN_EOF)
			{
				ok = false;
				rig_error("At least one BUTTON must be implemented");
			}
			else if (tt != RIG_TOKEN_BUTTON)
			{
				ok = false;
				rig_error("BUTTON expected NOT %s",rigTokenToString(tt));
			}
		}
		else if (ok)
		{
			while (ok && rig_token.id == RIG_TOKEN_BUTTON)
			{
				ok = handleButton();
			}
			if (ok && rig_token.id != RIG_TOKEN_EOF)
			{
				ok = false;
				rig_error("BUTTON expected NOT %",rigTokenToString(rig_token.id));
			}
		}

		// done
		// close the file

		closeRigFile();

		// set the index of the +1 last statement list
		// so everyone can get the number of statements
		// by subtracting from the +1 value ..

		parse_rig->statements[parse_rig->num_statements] = parse_rig->statement_pool_len;

		if (ok && (parse_rig->rig_type & RIG_TYPE_MODAL) && !any_end_modal)
		{
			rig_error("A modalRig must contain at least one endModal statement.");
			ok = 0;
		}

		ok = ok && !rig_error_found;
		if (ok)
		{
			warning(dbg_parse,"parseRig(%s.rig) finished OK",0);
			new_rig = relocate();
			if (new_rig)
			{
				dumpRig(new_rig);
				if (!strcmp(rig_name,"default") ||
					!strcmp(rig_name,"default_modal"))
				{
					dumpRigCode(new_rig,rig_name);
				}
			}
			else
			{
				rig_error("There was an error relocating the rig!");
			}

		}
		else
		{
			rig_error("There was an error parsing the rig!");
		}

	}	// file opened

	return new_rig;
}	// parseRig()


// end of rigParser.cpp
