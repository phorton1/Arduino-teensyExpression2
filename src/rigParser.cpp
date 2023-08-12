//-------------------------------------------------------
// rigParser.cpp
//-------------------------------------------------------
// Implements the class that parses Rig Text files
// and creates Rig memory images

#include <myDebug.h>
#include "rigToken.h"
#include "rigCode.h"
#include "myTFT.h"		// for TFT sizes
#include "theSystem.h"	// for client rect
#include "rigExpression.h"


#define dbg_parse 	-1
	// 0 = just show the main header
	// -1 = show statements and params
	// -2 = show details

#define MAX_STRING_POOL 			4096
#define MAX_STATEMENT_POOL 			4096
#define MAX_EXPRESSION_POOL			4096


// memory management
// pools initially implemented as static buffers for a single rig
// we will inevitably have to use the heap ...

static rigHeader_t rig_header;



static int num_statements;
static int num_statement_lists;
static int statement_pool_len;
static uint8_t statement_pool[MAX_STATEMENT_POOL];
static uint16_t statements[MAX_STATEMENTS];
static uint16_t statement_lists[MAX_STATEMENT_LISTS + 1];
	// one extra for the last length

static int expression_pool_len;
static uint8_t expression_pool[MAX_EXPRESSION_POOL];
static uint16_t expressions[MAX_EXPRESSIONS];

static int string_pool_len;
static char string_pool[MAX_STRING_POOL];

static const uint8_t null_expression[] = {237};
static int test_inline_num = 0;



static void init_parse()
{
	string_pool_len = 0;
	num_statements = 0;
	num_statement_lists = 1;	// empty init_section statement list
	statement_pool_len = 0;
	expression_pool_len = 0;
	test_inline_num = 12;

	memset(&rig_header,0,sizeof(rigHeader_t));
	memset(statements,0,MAX_STATEMENTS * sizeof(uint16_t));
	memset(statement_lists,0,(MAX_STATEMENT_LISTS + 1) * sizeof(uint16_t));
	memset(expressions,0,MAX_EXPRESSIONS * sizeof(uint16_t));

	memset(statement_pool, 0, MAX_STATEMENT_POOL);
	memset(expression_pool, 0, MAX_EXPRESSION_POOL);
	memset(string_pool, 0, MAX_STRING_POOL);
}


//-----------------------------------------------------
// defines
//-----------------------------------------------------

#define AREA_CLIENT_HEIGHT    (client_rect.ye - client_rect.ys + 1)

// init_only params

#define PARAM_VALUE_NUM      1

#define PARAM_PEDAL_NUM      10
#define PARAM_PEDAL_NAME     11

#define PARAM_AREA_NUM       20
#define PARAM_FONT_SIZE      21
#define PARAM_FONT_TYPE      22
#define PARAM_FONT_JUST		 23
#define PARAM_START_X        24
#define PARAM_START_Y        25
#define PARAM_END_X          26
#define PARAM_END_Y          27

#define PARAM_STRING_NUM     30
#define PARAM_TEXT    		 31

// monadic params in either

#define PARAM_MIDI_PORT      40
#define PARAM_MIDI_CHANNEL   41
#define PARAM_MIDI_CC        42

// expression params

#define PARAM_NUM_EXPRESSION			100
#define PARAM_STRING_EXPRESSION			101
#define PARAM_LED_COLOR_EXPRESSION		102
#define PARAM_DISPLAY_COLOR_EXPRESSION	103

static const char *argTypeToString(int i)
{
	switch (i)
	{
		case PARAM_VALUE_NUM    : return "VALUE_NUM";
		case PARAM_PEDAL_NUM 	: return "PEDAL_NUM";
		case PARAM_PEDAL_NAME   : return "PEDAL_NAME";
		case PARAM_AREA_NUM     : return "AREA_NUM";
		case PARAM_FONT_SIZE    : return "FONT_SIZE";
		case PARAM_FONT_TYPE    : return "FONT_TYPE";
		case PARAM_FONT_JUST    : return "FONT_JUST";

		case PARAM_START_X      : return "START_X";
		case PARAM_START_Y      : return "START_Y";
		case PARAM_END_X        : return "END_X";
		case PARAM_END_Y        : return "END_Y";
		case PARAM_STRING_NUM   : return "STRING_NUM";
		case PARAM_TEXT  		: return "TEXT";

		case PARAM_MIDI_PORT    : return "MIDI_PORT";
		case PARAM_MIDI_CHANNEL : return "MIDI_CHANNEL";
		case PARAM_MIDI_CC      : return "MIDI_CC";

		case PARAM_NUM_EXPRESSION				: return "NUM_EXPRESSION";
		case PARAM_STRING_EXPRESSION			: return "STRING_EXPRESSION";
		case PARAM_LED_COLOR_EXPRESSION			: return "LED_COLOR_EXPRESSION";
		case PARAM_DISPLAY_COLOR_EXPRESSION		: return "DISPLAY_COLOR_EXPRESSION";
	}
	return "unknownArgType";
}



static const int PEDAL_ARGS[] = {		// PEDAL(0, "Synth", MIDI, 1, 7);
	PARAM_PEDAL_NUM,
	PARAM_PEDAL_NAME,
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_MIDI_CC,
	0
};
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
static const int LISTEN_ARGS[] = {			// LISTEN(37, SERIAL, 1, 20);
	PARAM_VALUE_NUM,
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_MIDI_CC,
	0
};
static const int STRING_DEF_ARGS[] = {		// STRING_DEF(0, "BASS1");
	PARAM_STRING_NUM,
	PARAM_TEXT,
	0
};

static const int SETVALUE_ARGS[] = {		// setValue(20, VALUE[0] + 2);
	PARAM_VALUE_NUM,
	PARAM_NUM_EXPRESSION,
	0 };

static const int DISPLAY_ARGS[] = {			// display(dbg_parse, VALUE[0] ? BLUE : BLACK, "test");
	PARAM_AREA_NUM,
	PARAM_DISPLAY_COLOR_EXPRESSION,
	PARAM_STRING_EXPRESSION,
	0 };

static const int SEND_CC_ARGS[] = {			// sendCC(SERIAL, 1, 192, VALUE[3]);
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_MIDI_CC,
	PARAM_NUM_EXPRESSION,
	0
};

static const int SEND_PGM_CHG_ARGS[] = {	// sendPgmChg(SERIAL, 1, VALUE[3]);
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_NUM_EXPRESSION,
	0
};

static const int NOTE_ON_ARGS[] = {			// noteOn(MIDI, 9, VALUE[3], VALUE[4]);
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_NUM_EXPRESSION,
	PARAM_NUM_EXPRESSION,
	0
};

static const int NOTE_OFF_ARGS[] = {		// noteOff(MIDI, 9, VALUE[4]);
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_NUM_EXPRESSION,
	0
};

static const int ALL_NOTE_OFF_ARGS[] = {	// AllNotesOff(MIDI, 9,);
	PARAM_MIDI_PORT,
	PARAM_NUM_EXPRESSION,
	0
};

static const int NO_ARGS[] = {	// AllNotesOff(midi_port, midi_channel);
	0
};


typedef struct
{
	const int id;
	const int *args;
} statement_param_t;



static const statement_param_t statement_params[] = {
	{ RIG_TOKEN_PEDAL, 			PEDAL_ARGS },
	{ RIG_TOKEN_AREA, 			AREA_ARGS },
	{ RIG_TOKEN_LISTEN, 		LISTEN_ARGS },
	{ RIG_TOKEN_STRING_DEF, 	STRING_DEF_ARGS },

	{ RIG_TOKEN_SETVALUE, 		SETVALUE_ARGS },
	{ RIG_TOKEN_DISPLAY, 		DISPLAY_ARGS },
	{ RIG_TOKEN_SEND_CC, 		SEND_CC_ARGS },
	{ RIG_TOKEN_SEND_PGM_CHG,	SEND_PGM_CHG_ARGS },
	{ RIG_TOKEN_NOTE_ON,		NOTE_ON_ARGS },
	{ RIG_TOKEN_NOTE_OFF,		NOTE_OFF_ARGS },
	{ RIG_TOKEN_ALL_NOTES_OFF,	ALL_NOTE_OFF_ARGS },

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

static bool addStringPool(const char *s)
{
	int len = strlen(s);
	if (string_pool_len >= MAX_STRING_POOL - len - 1)
	{
		rig_error("STRING POOL OVERLFLOW");
		return false;
	}
	strcpy(&string_pool[string_pool_len],s);
	string_pool_len += len + 1;
	return true;
}

static bool addStatementByte(uint8_t byte)
{
	if (statement_pool_len >= MAX_STATEMENT_POOL)
	{
		rig_error("STATMENT(BYTE) POOL OVERLFLOW");
		return false;
	}
	statement_pool[statement_pool_len++] = byte;
	return true;
}

static bool addStatementInt(uint16_t i)
{
	if (statement_pool_len >= MAX_STATEMENT_POOL - 2)
	{
		rig_error("STATMENT(INT) POOL OVERLFLOW");
		return false;
	}
	uint16_t *ptr = (uint16_t *) &statement_pool[statement_pool_len];
	*ptr = i;
	statement_pool_len += 2;
	return true;
}

// static bool addExpression(int len, const uint8_t *bytes)
// {
// 	if (expression_pool_len + len >= MAX_EXPRESSION_POOL)
// 	{
// 		rig_error("EXPRESSION POOL OVERLFLOW");
// 		return false;
// 	}
// 	memcpy(&expression_pool[expression_pool_len],bytes,len);
// 	expression_pool_len += len;
// 	return true;
// }


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

static bool getNumber(int *retval, const char *what, int min, int max)
{
	if (rig_token.id != RIG_TOKEN_NUMBER)
	{
		rig_error("%s Expected",what);
		return false;
	}
	if (rig_token.int_value < min ||
		rig_token.int_value > max)
	{
		rig_error("%s must be between %d and %d",what,min,max);
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
	display(dbg_parse + 1,"%s = %s",what,buf);

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

	static int pedal_num;
	static int area_num;
	static int listen_num;
	static int string_num;
	// uint8_t *expression;

	if (IS_INIT_ONLY_STATEMENT(statement_type))
	{
		switch (arg_type)
		{
			case PARAM_VALUE_NUM :	// only in LISTENS
				if (!getNumber(&value,"VALUE_NUM",0,RIG_NUM_VALUES-1))
					ok = 0;
				else
				{
					listen_num = value;
					rig_header.listens[listen_num].active = 1;
				}
				break;

			case PARAM_PEDAL_NUM :
				if (!getNumber(&value,"PEDAL_NUM",0,NUM_PEDALS-1))
					ok = 0;
				else
					pedal_num = value;
				break;
			case PARAM_PEDAL_NAME   :
				text = getText("PEDAL_NAME",7);
				if (!text)
					ok = 0;
				else
					strcpy(rig_header.pedals[pedal_num].name,text);
				break;
			case PARAM_MIDI_PORT    :
				if (rig_token.id < RIG_TOKEN_MIDI ||
					rig_token.id > RIG_TOKEN_SERIAL)
				{
					rig_error("MIDI_PORT must be MIDI or SERIAL");
					ok = 0;
				}
				else
				{
					uint8_t use_port = rig_token.id - RIG_TOKEN_MIDI;
					display(dbg_parse + 1, "MIDI_PORT = %s (%d)",rigTokenToString(rig_token.id),use_port);
					if (statement_type == RIG_TOKEN_PEDAL)
						rig_header.pedals[pedal_num].port = use_port;
					else if (statement_type == RIG_TOKEN_LISTEN)
						rig_header.listens[listen_num].port = use_port;
					if (!getRigToken())
						ok = 0;
				}
				break;
			case PARAM_MIDI_CHANNEL :
				if (!getNumber(&value,"MIDI_CHANNEL",1,15))
					ok = 0;
				else
				{
					if (statement_type == RIG_TOKEN_PEDAL)
						rig_header.pedals[pedal_num].channel = value;
					else if (statement_type == RIG_TOKEN_LISTEN)
						rig_header.listens[listen_num].channel = value;
				}
				break;
			case PARAM_MIDI_CC      :
				if (!getNumber(&value,"MIDI_CC",0,127))
					ok = 0;
				else
				{
					if (statement_type == RIG_TOKEN_PEDAL)
						rig_header.pedals[pedal_num].cc = value;
					else if (statement_type == RIG_TOKEN_LISTEN)
						rig_header.listens[listen_num].cc = value;
				}
				break;

			case PARAM_AREA_NUM     :
				if (!getNumber(&value,"AREA_NUM",0,RIG_NUM_AREAS-1))
					ok = 0;
				else
					area_num = value;
				break;
			case PARAM_FONT_SIZE    :
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
					// already shown in getNumberAny()
					// display(dbg_parse + 1, "FONT_SIZE = %d",value);
					rig_header.areas[area_num].font_size = value;
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
					rig_header.areas[area_num].font_type = use_type;
					if (!getRigToken())
						ok = 0;
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
					rig_header.areas[area_num].font_just = use_just;
					if (!getRigToken())
						ok = 0;
				}
				break;

			case PARAM_START_X      :
				if (!getNumber(&value,"START_X",0,TFT_WIDTH-1))
					ok = 0;
				else
					rig_header.areas[area_num].xs = value;
				break;
			case PARAM_START_Y      :
				if (!getNumber(&value,"START_Y",0,AREA_CLIENT_HEIGHT-1))
					ok = 0;
				else
					rig_header.areas[area_num].ys = value;
				break;
			case PARAM_END_X        :
				if (!getNumber(&value,"END_X",0,TFT_WIDTH-1))
					ok = 0;
				else
					rig_header.areas[area_num].xe = value;
				break;
			case PARAM_END_Y        :
				if (!getNumber(&value,"END_Y",0,AREA_CLIENT_HEIGHT-1))
					ok = 0;
				else
					rig_header.areas[area_num].ye = value;
				break;

			case PARAM_STRING_NUM   :
				if (!getNumber(&value,"STRING_NUM",0,RIG_NUM_STRINGS-1))
					ok = 0;
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
					rig_header.strings[string_num] = string_pool_len + 1;
					ok = addStringPool(text);
				}
				break;
			default:
				rig_error("implementation error unknown init statement param %d",arg_type);
				break;
		}
	}

	else  // REGULAR STATEMENTS store values in line with commas and a paren
	{
		switch (arg_type)
		{
			case PARAM_VALUE_NUM :	// in setValue
				if (!getNumber(&value,"VALUE_NUM",0,RIG_NUM_VALUES-1))
					ok = 0;
				else
					addStatementByte(value);
				break;
			case PARAM_AREA_NUM     :
				if (!getNumber(&value,"AREA_NUM",0,RIG_NUM_AREAS-1))
					ok = 0;
				else
					addStatementByte(value);
				break;
			case PARAM_MIDI_PORT    :
				if (rig_token.id < RIG_TOKEN_MIDI ||
					rig_token.id > RIG_TOKEN_SERIAL)
				{
					rig_error("MIDI_PORT must be MIDI or SERIAL");
					ok = 0;
				}
				else
				{
					uint8_t use_port = rig_token.id - RIG_TOKEN_MIDI;
					display(dbg_parse + 1, "MIDI_PORT = %s (%d)",rigTokenToString(rig_token.id),use_port);
					addStatementByte(use_port);
					if (!getRigToken())
						ok = 0;
				}
				break;
			case PARAM_MIDI_CHANNEL :
				if (!getNumber(&value,"MIDI_CHANNEL",1,15))
					ok = 0;
				else
					addStatementByte(value);
				break;
			case PARAM_MIDI_CC      :
				if (!getNumber(&value,"MIDI_CC",0,127))
					ok = 0;
				else
					addStatementByte(value);
				break;
			case PARAM_NUM_EXPRESSION				:
				rigNumericExpression(rig_token.id);
					// have to call to skip bytes
				ok = addStatementInt(EXPRESSION_INLINE | (EXP_VALUE << 8) | (test_inline_num++ & 0xff));
				// ok = ok && addExpression(1,null_expression);
				break;
			case PARAM_STRING_EXPRESSION			:
				rigStringExpression(rig_token.id);
					// have to call to skip bytes
				ok = addStatementInt(EXPRESSION_INLINE | (EXP_STRING << 8) | (test_inline_num++ & 0xff));
				// ok = ok && addExpression(1,null_expression);
				break;
			case PARAM_LED_COLOR_EXPRESSION			:
				rigLedColorExpression(rig_token.id);
					// have to call to skip bytes
				ok = addStatementInt(EXPRESSION_INLINE | (EXP_LED_COLOR << 8) );
				// ok = ok && addExpression(1,null_expression);
				break;
			case PARAM_DISPLAY_COLOR_EXPRESSION		:
				rigDisplayColorExpression(rig_token.id);
					// have to call to skip bytes
				ok = addStatementInt(EXPRESSION_INLINE | (EXP_DISPLAY_COLOR << 8) );
				// ok = ok && addExpression(1,null_expression);
				break;

			default:
				rig_error("implementation error unknown button statement param %d",arg_type);
				break;
		}
	}

	proc_leave();
	if (!ok)
		return false;
	display(dbg_parse + 2,"handleArg(%s) finished",argTypeToString(arg_type));
	return true;
}



static bool handleStatement(uint16_t statement_list_num, int tt)
{
	display(dbg_parse + 1,"handleStatement(%d) %s",num_statements,rigTokenToString(tt));
	proc_entry();

	if (num_statements >= MAX_STATEMENTS)
	{
		rig_error("implementation error: too many statements");
		proc_leave();
		return false;
	}

	statements[num_statements++] = statement_pool_len;
	statement_pool[statement_pool_len++] = tt;

	int statement_type = tt;

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
		if (*arg)
		{
			if (!skip(RIG_TOKEN_COMMA))
			{
				proc_leave();
				return false;
			}
		}
		else
		{
			if (!skip(RIG_TOKEN_RIGHT_PAREN))
			{
				proc_leave();
				return false;
			}
		}
	}

	// the parse WILL end on an eof in the button section
	// at the semi-colon at the end of a statement.

	if (!skip(RIG_TOKEN_SEMICOLON, parse_section == 2))
	{
		proc_leave();
		return false;
	}

	display(dbg_parse + 2,"handleStatement(%d) finished",num_statements-1);
	proc_leave();
	return true;
}



static bool handleStatementList(int tt)
{
	display(dbg_parse + 2,"handleStatementList(%d) %s",num_statement_lists,rigTokenToString(tt));
	proc_entry();

	if (num_statement_lists >= MAX_STATEMENT_LISTS)
	{
		rig_error("implementation error: too many statement lists");
		proc_leave();
		return false;
	}

	uint16_t statement_list_num = num_statement_lists++;
	statement_lists[statement_list_num] = num_statements;

	while (IS_STATEMENT(tt))
	{
		if (!handleStatement(statement_list_num,tt))
		{
			rig_error("handleStatement() failed!");
			proc_leave();
			return false;
		}
		tt = rig_token.id;
		display(dbg_parse + 2,"back from handleStatement() token=%s",rigTokenToString(tt));
	}

	proc_leave();
	display(dbg_parse + 2,"handleStatementList(%d) finished",num_statement_lists-1);
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

		rig_header.button_refs[button_num][sub_num] = num_statement_lists + 1;
			// 1 based so that we can identify a used statement list

		ok = handleStatementList(rig_token.id);
	}
	else if (sub_id == RIG_TOKEN_COLOR)
	{
		rigLedColorExpression(rig_token.id);
			// have to call to skip bytes
		rig_header.button_refs[button_num][sub_num] = EXPRESSION_INLINE | (EXP_LED_COLOR << 8);
		// rig_header.button_refs[button_num][sub_num] = expression_pool_len + 1;
		// ok = addExpression(1,null_expression);
		ok = ok && skip(RIG_TOKEN_SEMICOLON);
	}
	else
	{
		rigNumericExpression(rig_token.id);
			// have to call to skip bytes
		rig_header.button_refs[button_num][sub_num] = EXPRESSION_INLINE | (EXP_NUMBER << 8) | (test_inline_num++ & 0xff);
		// rig_header.button_refs[button_num][sub_num] = expression_pool_len + 1;
		// ok = addExpression(1,null_expression);
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
	if (dbg_parse < 0)
		delay(100);		// hrmph
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
		if (ok && !IS_SUBSECTION(rig_token.id))
		{
			ok = 0;
			rig_error("BUTTON subsection expected");
		}
		if (ok)
			ok = handleSubsections(button_num);
	}

	proc_leave();
	if (ok)
		display(dbg_parse + 2,"handleButton(%d) finished",button_num);
	return ok;
}






//--------------------------------------------------------
// parseRig
//--------------------------------------------------------

#include "rigDump.h"



// extern
bool parseRig()
	// generates the intermediate structure of a
	// syntactically valid set of bytes that are
	// RIG_ID's, and with inline TEXT and int_values;

{
	display(dbg_parse,"ParseRig(%s.rig)",rig_name);
	bool ok = 0;
	if (openRigFile(rig_name))
	{
		ok = 1;

		init_parse();
		int tt = getRigToken();

		// rig type

		if (tt == RIG_TOKEN_OVERLAY)
			rig_header.overlay_type = OVERLAY_TYPE_OVERLAY;
		tt = getRigToken();
		if (!tt)
		{
			rig_error("unexpected end of program");
			ok = false;
		}

		// init_statements

		if (ok && IS_INIT_STATEMENT(tt))
			ok = ok && handleStatementList(tt);

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

		statement_lists[num_statement_lists] = num_statements;

		if (ok)
		{
			display(dbg_parse,"",0);
			warning(dbg_parse,"parseRig(%s.rig) finished OK",0);
			dumpRig();
		}

	}	// file opened

	return ok;
}	// parseRig()


// end of rigParser.cpp
