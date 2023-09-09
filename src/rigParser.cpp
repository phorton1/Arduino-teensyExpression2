//-------------------------------------------------------
// rigParser.cpp
//-------------------------------------------------------

#define WITH_DEFAULT_RIG  1

#include <myDebug.h>
#include "rigParser.h"
#include "prefs.h"
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
#define dbg_stats	-1
	// 0 = show rig size
	// -1 = show rig size details

#define MAX_RIG_NAME		31


// externs

bool is_parse_button;		// available to rigExpression.cpp
uint16_t rig_pool_len;
uint8_t  rig_pool[RIG_POOL_SIZE];

// static global variables

static uint8_t parse_pool[MAX_RIG_SIZE];
static rig_t *parse_rig = (rig_t *) parse_pool;
static const rig_t *base_rig = (const rig_t *) parse_pool;
static bool any_end_modal;


//---------------------------------------
// utilities
//---------------------------------------

static void init_parse()
{
	any_end_modal = 0;
	parse_error_found = 0;	 // in rigToken.cpp

	// display(0,"MAX_RIG_SIZE = %d", MAX_RIG_SIZE);

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


// extern
bool legalFilename(const char *name)
	// public, extern'd in defines.h
{
	for (uint16_t i=0; i<strlen(name); i++)
	{
		if (!((name[i] == '-' || name[i] == '.' || name[i] == '_' ) ||
			  (name[i] >= 'A' && name[i] <= 'Z') ||
			  (name[i] >= 'a' && name[i] <= 'z') ||
			  (name[i] >= '0' && name[i] <= '9') ))
		{
			return 0;
		}
	}
	return 1;
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

	display(dbg_parse,"relocate(%s, bytes=%d) to %d",
		parse_rig->rig_type & RIG_TYPE_MODAL ? "ModalRig":"BaseRig",
		rig_size,
		relocate_to);

	if (relocate_to + rig_size >= RIG_POOL_SIZE)
	{
		parse_error("RIG(%d) to big to fit in remaining POOL(%d)",rig_size,RIG_POOL_SIZE-relocate_to);
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
		case PARAM_METER_TYPE		: return "METER_TYPE";
		case PARAM_FONT_SIZE    	: return "FONT_SIZE";
		case PARAM_FONT_TYPE    	: return "FONT_TYPE";
		case PARAM_FONT_JUST    	: return "FONT_JUST";
		case PARAM_START_X      	: return "START_X";
		case PARAM_START_Y      	: return "START_Y";
		case PARAM_END_X        	: return "END_X";
		case PARAM_END_Y        	: return "END_Y";

		case PARAM_BUTTON_NUM		: return "BUTTON_NUM";
		case PARAM_PEDAL_NUM 		: return "PEDAL_NUM";
		case PARAM_PEDAL_NAME   	: return "PEDAL_NAME";
		case PARAM_ROTARY_NUM 		: return "ROTARY_NUM";

		case PARAM_VALUE_NUM    	: return "VALUE_NUM";
		case PARAM_VALUE			: return "VALUE";

		case PARAM_LISTEN_DIR		: return "LISTEN_DIR";
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

static const int LISTEN_ARGS[] = {			// LISTEN(37, SERIAL, 0, 20);
	PARAM_VALUE_NUM,
	PARAM_MIDI_PORT,
	PARAM_LISTEN_DIR,
	PARAM_LISTEN_CHANNEL,
	PARAM_MIDI_CC,
	0
};

static const int LISTEN_RANGED_ARGS[] = {			// LISTEN(16, clip_mute_base, SERIAL, 0, CLIP_MUTE_BASE_CC);
	PARAM_VALUE,				// should really be a PARAM_NUMBER or PARAM_CONSTANT_NUMBER
	PARAM_VALUE_NUM,
	PARAM_MIDI_PORT,
	PARAM_LISTEN_DIR,
	PARAM_LISTEN_CHANNEL,
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

static const int METER_ARGS[] = {			// METER(0, 32, BOLD, LEFT, 5, 5, 299, 40);
	PARAM_AREA_NUM,
	PARAM_METER_TYPE,
	PARAM_START_X,
	PARAM_START_Y,
	PARAM_END_X,
	PARAM_END_Y,
	0
};

// end init only statement range

static const int PEDAL_ARGS[] = {		// PEDAL(0, "Synth", MIDI, 1, 7);
	PARAM_PEDAL_NUM,
	PARAM_PEDAL_NAME,
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_MIDI_CC,
	0
};

static const int ROTARY_ARGS[] = {		// ROTARY(0, "thru", SERIAL, 1, LOOP_CONTROL_BASE + n);
	PARAM_ROTARY_NUM,
	PARAM_PEDAL_NAME,
	PARAM_MIDI_PORT,
	PARAM_MIDI_CHANNEL,
	PARAM_MIDI_CC,
	PARAM_VALUE,
	0
};

static const int SETVALUE_ARGS[] = {		// setValue(20, VALUE[0] + 2);
	PARAM_VALUE_NUM,
	PARAM_VALUE,
	0 };

static const int SETTITLE_ARGS[] = {		// setTitle2"Track ONE");
	PARAM_STRING_EXPRESSION,
	0 };

static const int DISPLAY_ARGS[] = {			// display(AREA, VALUE[0] ? BLUE : BLACK, "test");
	PARAM_AREA_NUM,
	PARAM_DISPLAY_COLOR_EXPRESSION,
	PARAM_STRING_EXPRESSION,
	0 };

static const int DISPLAY_NUMBER_ARGS[] = {		// displayNumber(AREA, LED_YELLOW, VALUE[_blah]);
	PARAM_AREA_NUM,
	PARAM_DISPLAY_COLOR_EXPRESSION,
	PARAM_VALUE,
	0 };

static const int SET_METER_ARGS[] = {		// setMeter(AREA, GREEN, VALUE[_blah]);
	PARAM_AREA_NUM,
	PARAM_DISPLAY_COLOR_EXPRESSION,
	PARAM_VALUE,
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

// end init statement range

static const int SET_BUTTON_COLOR_ARGS[] = {
	PARAM_BUTTON_NUM,
	PARAM_LED_COLOR_EXPRESSION,
	0 };

static const int SET_BUTTON_BLINK_ARGS[] = {
	PARAM_BUTTON_NUM,
	PARAM_VALUE,
	0 };


static const int LOAD_RIG_ARGS[] = {		// LoadRig("modal")
	PARAM_RIG_NAME,
	0
};


static const int NO_ARGS[] = {
	0
};



static const statement_param_t statement_params[] = {
	{ RIG_TOKEN_DEFINE_DEF,			DEFINE_ARGS },
	{ RIG_TOKEN_STRING_DEF, 		STRING_DEF_ARGS },

	{ RIG_TOKEN_LISTEN, 			LISTEN_ARGS },
	{ RIG_TOKEN_LISTEN_RANGED,		LISTEN_RANGED_ARGS },
	{ RIG_TOKEN_AREA, 				AREA_ARGS },
	{ RIG_TOKEN_METER,				METER_ARGS },

	{ RIG_TOKEN_PEDAL, 				PEDAL_ARGS },
	{ RIG_TOKEN_ROTARY, 			ROTARY_ARGS },
	{ RIG_TOKEN_SETVALUE, 			SETVALUE_ARGS },
	{ RIG_TOKEN_SETTITLE, 			SETTITLE_ARGS },
	{ RIG_TOKEN_DISPLAY, 			DISPLAY_ARGS },
	{ RIG_TOKEN_DISPLAY_NUMBER, 	DISPLAY_NUMBER_ARGS },
	{ RIG_TOKEN_SET_METER, 			SET_METER_ARGS },
	{ RIG_TOKEN_SEND_CC, 			SEND_CC_ARGS },
	{ RIG_TOKEN_SEND_PGM_CHG,		SEND_PGM_CHG_ARGS },
	{ RIG_TOKEN_NOTE_ON,			NOTE_ON_ARGS },
	{ RIG_TOKEN_NOTE_OFF,			NOTE_OFF_ARGS },
	{ RIG_TOKEN_ALL_NOTES_OFF,		ALL_NOTE_OFF_ARGS },

	{ RIG_TOKEN_SET_BUTTON_COLOR,	SET_BUTTON_COLOR_ARGS },
	{ RIG_TOKEN_SET_BUTTON_BLINK,	SET_BUTTON_BLINK_ARGS },
	{ RIG_TOKEN_LOAD_RIG,			LOAD_RIG_ARGS },

	{ RIG_TOKEN_END_MODAL,			NO_ARGS },
	{ RIG_TOKEN_FTP_TUNER,			NO_ARGS },
	{ RIG_TOKEN_FTP_SENSITIVITY,	NO_ARGS },
	{ 0, 0 } };


const statement_param_t *findParams(int tt)
{
	const statement_param_t *ptr = statement_params;
	while (ptr->id && ptr->id != tt)
		ptr++;
	if (!ptr->id)
		parse_error("unknown statement %s",rigTokenToString(tt));
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
		parse_error("DEFINE POOL OVERLFLOW");
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
		parse_error("STRING POOL OVERLFLOW");
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
		parse_error("STATMENT(BYTE) POOL OVERLFLOW");
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
		parse_error("STATMENT(INT) POOL OVERLFLOW");
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
		parse_error("expected %s got %s",rigTokenToString(tt),rigTokenToString(rig_token.id));
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
		parse_error("%s Expected",what);
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
		parse_error("%s Expected",what);
		return false;
	}
	if (rig_token.int_value > max)
	{
		parse_error("%s must be less than %d",what,max);
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
		parse_error("%s Expected",what);
		return 0;
	}

	int len = strlen(rig_token.text);
	if (len > max_len)
	{
		parse_error("%s limited to %d bytes");
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
		parse_error("IDENTIFIER Expected");
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

static bool handleArg(int tt, int arg_type)
{
	display(dbg_parse + 2,"handleArg(%s) token=%s",argTypeToString(arg_type),rigTokenToString(rig_token.id));
	proc_entry();

	int value;
	const char *text;
	bool ok = 1;

	static int define_num;
	static int string_num;

	if (IS_INIT_HEADER_STATEMENT(tt))
	{
		switch (arg_type)
		{
			case PARAM_DEFINE_NUM :
				if (!getNumber(&value,"VALUE_NUM",RIG_NUM_DEFINES-1))
					ok = 0;
				else if (parse_rig->define_ids[value])
				{
					parse_error("define(%d) used more than once",value);
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
					parse_error("define_string(%d) used more than once",value);
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
				parse_error("implementation error: unknown init_header param %d",arg_type);
				break;
		}
	}

	else  // REGULAR STATEMENTS store values in line with commas and a paren
	{
		switch (arg_type)
		{
			// AREAs and METERS

			case PARAM_AREA_NUM :
				value = rigAreaNumExpression(parse_rig);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_METER_TYPE :
				if (rig_token.id != RIG_TOKEN_HORZ &&
					rig_token.id != RIG_TOKEN_VERT)
				{
					parse_error("METER_TYPE must be HORZ or VERT");
					ok = 0;
				}
				else
				{
					uint8_t use_type = rig_token.id - RIG_TOKEN_HORZ;
					display(dbg_parse + 1, "METER_TYPE = %s (%d)",rigTokenToString(rig_token.id),use_type);
					ok = addStatementByte(use_type);
					ok = ok && getRigToken();
				}
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
					parse_error("FONT_SIZE(%d) must be 12, 14, 16, 18, 20, 24, 28, 32, 40, or 48");
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
					parse_error("FONT_TYPE must be NORMAL or BOLD");
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
					parse_error("FONT_JUST must be LEFT, CENTER, or RIGHT");
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
				ok = getNumber(&value,"START_Y",client_rect.height());
				ok = ok && addStatementInt(value);
				break;
			case PARAM_END_X        :
				ok = getNumber(&value,"END_X",TFT_WIDTH-1);
				ok = ok && addStatementInt(value);
				break;
			case PARAM_END_Y        :
				ok = getNumber(&value,"END_Y",client_rect.height()+1);
				ok = ok && addStatementInt(value);
				break;

			// Button Num

			case PARAM_BUTTON_NUM :	// prh - to become an expression
				if (rig_token.id == RIG_TOKEN_BUTTON_NUM)
				{
					if (!is_parse_button)
					{
						parse_error("_BUTTON_NUM may only be used in button sections");
						ok = false;
					}
					else
					{
						value = EXP_BUTTON_NUM;
						ok = getRigToken();
					}
				}
				else if (!getNumber(&value,"BUTTON_NUM",NUM_BUTTONS-1))
					ok = 0;
				ok = ok && addStatementByte(value);
				break;

			// Pedals and Rotaries

			case PARAM_PEDAL_NUM :
				value = rigPedalNumExpression(parse_rig);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_ROTARY_NUM :
				value = rigRotaryNumExpression(parse_rig);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_PEDAL_NAME   :
				text = getText("PEDAL_NAME",MAX_PEDAL_NAME);
				ok = ok && text;
				ok = ok && addStatementInt(parse_rig->string_pool_len);
				ok = ok && addStringPool(text);
				break;

			// LISTEN and setValue

			case PARAM_VALUE_NUM :	// in setValue and LISTEN
				value = rigValueNumExpression(parse_rig);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_VALUE :
				value = rigValueExpression(parse_rig);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;

			// LISTEN() and things that do MIDI

			case PARAM_LISTEN_DIR :
				if (rig_token.id < RIG_TOKEN_INPUT ||
					rig_token.id > RIG_TOKEN_BOTH)
				{
					parse_error("LISTEN_DIRECTION must be INPUT, OUTPUT, or BOTH");
					ok = 0;
				}
				else
				{
					uint8_t listen_dir = rig_token.id - RIG_TOKEN_INPUT;
					display(dbg_parse + 1, "LISTEN_DIR = %s (%d)",rigTokenToString(rig_token.id),listen_dir);
					ok = addStatementByte(listen_dir);
					ok = ok && getRigToken();
				}
				break;
			case PARAM_MIDI_PORT    :
				if (!IS_MIDI_PORT(rig_token.id))
				{
					parse_error("MIDI_PORT must be MIDI0, MIDI1, or SERIAL");
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
				value = rigMidiChannelExpression(parse_rig);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_LISTEN_CHANNEL :
				value = rigListenChannelExpression(parse_rig);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_MIDI_CC :
			case PARAM_MIDI_VALUE :
				value = rigMidiValueExpression(parse_rig);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;

			// loadRig()

			case PARAM_RIG_NAME  :
				text = getText("RIG_NAME",MAX_RIG_NAME);
				if (!text)
					ok = 0;
				else
				{
					if (!legalFilename(text))
					{
						parse_error("RIG_NAME(%s) may only contain A-Z,a-z,0-9,dash,period, or underscore characters");
						ok = 0;
					}
					if (ok)
					{
						ok = ok && addStatementInt(parse_rig->string_pool_len);
						ok = ok && addStringPool(text);
					}
				}
				break;

			// Generic Expression

			case PARAM_STRING_EXPRESSION :
				value = rigStringExpression(parse_rig);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_LED_COLOR_EXPRESSION :
				value = rigLedColorExpression(parse_rig);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;
			case PARAM_DISPLAY_COLOR_EXPRESSION	:
				value = rigDisplayColorExpression(parse_rig);
				ok = ok && value;
				ok = ok && addStatementInt(value);
				break;

			default:
				parse_error("implementation error: unknown statement param %d",arg_type);
				break;
		}
	}

	proc_leave();
	if (!ok)
		return false;
	display(dbg_parse + 2,"handleArg(%s) finished",argTypeToString(arg_type));
	return true;
}



static bool handleStatement()
{
	display(dbg_parse + 1,"handleStatement(%s)",rigTokenToString(rig_token.id));
	proc_entry();

	int tt = rig_token.id;

	// note if we have gotten any endModal statements in this parse

	if (tt == RIG_TOKEN_END_MODAL)
		any_end_modal = 1;

	// this parses either statements that actually generate statement rig_code,
	// or init only statements that store things in the rig_header. If it is
	// not an init_only statement, we write the token as the first byte
	// of the statement

	if (!IS_INIT_HEADER_STATEMENT(tt))
	{
		if (!addStatementByte(tt))
		{
			proc_leave();
			return false;
		}

		// statement tokens are folowed by their uint16_t line number

		if (!addStatementInt(rig_token.line_num))
		{
			proc_leave();
			return false;
		}
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
		display(0,"Could not findParams for %d=%s",tt,rigTokenToString(tt));
		proc_leave();
		return false;
	}

	const int *arg = params->args;
	while (*arg)
	{
		if (!handleArg(tt, *arg++))
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

	if (!skip(RIG_TOKEN_SEMICOLON, parse_section == PARSE_SECTION_BUTTONS))
	{
		proc_leave();
		return false;
	}

	display(dbg_parse + 2,"handleStatement() finished",0);
	proc_leave();
	return true;
}



static bool handleStatementList()
{
	display(dbg_parse + 1,"handleStatementList(%d) %s",parse_rig->num_statements,rigTokenToString(rig_token.id));
	proc_entry();

	// set pool offset into statements array
	// allow one for terminating length

	if (parse_rig->num_statements >= MAX_STATEMENTS - 1)
	{
		parse_error("implementation error: too many STATMENTS_LISTS(%d)",parse_rig->num_statements);
		proc_leave();
		return false;
	}
	parse_rig->statements[parse_rig->num_statements++] = parse_rig->statement_pool_len;

	// process statements

	while (IS_STATEMENT(rig_token.id))
	{
		if (!handleStatement())
		{
			my_error("handleStatement() failed!",0);
			proc_leave();
			return false;
		}
		display(dbg_parse + 2,"back from handleStatement() token=%s",rigTokenToString(rig_token.id));
	}

	proc_leave();
	display(dbg_parse + 2,"handleStatementList() finished on %s",rigTokenToString(rig_token.id));
	return true;
}



static bool handleSubsections(uint16_t *type, uint16_t *refs)
{
	display(dbg_parse + 2,"handleSubsections(%s)",rigTokenToText(rig_token.id));

	bool ok = 1;
	while (ok && (IS_SUBSECTION(rig_token.id) || IS_STATEMENT(rig_token.id)))
	{
		int ref_num = 0;
		uint16_t new_type = 0;
		if (IS_SUBSECTION(rig_token.id))
		{
			display(dbg_parse + 1,"parsing subsections(%s)",rigTokenToText(rig_token.id));
			new_type =
				rig_token.id == RIG_TOKEN_RELEASE ? BUTTON_TYPE_RELEASE :
				rig_token.id == RIG_TOKEN_REPEAT ? BUTTON_TYPE_REPEAT :
				rig_token.id == RIG_TOKEN_PRESS ? BUTTON_TYPE_PRESS :
				rig_token.id == RIG_TOKEN_LONG ? BUTTON_TYPE_LONG :
				BUTTON_TYPE_CLICK;

			uint16_t existing_type = *type & ~BUTTON_TYPE_UPDATE;
			bool type_is_clicky = (new_type == BUTTON_TYPE_CLICK || new_type == BUTTON_TYPE_LONG);
			bool existing_is_clicky = (existing_type == BUTTON_TYPE_CLICK || existing_type == BUTTON_TYPE_LONG);
			bool type_is_release = (new_type == BUTTON_TYPE_RELEASE);
			bool existing_is_release = (existing_type == BUTTON_TYPE_RELEASE);

			if (existing_type && type_is_clicky != existing_is_clicky)
			{
				parse_error("CLICK can only be used with LONG");
				ok = 0;
			}
			else if (existing_type && !type_is_clicky &&
					 type_is_release == existing_is_release)
			{
				parse_error("PRESS/REPEAT can only be used with RELEASE");
				ok = 0;
			}

			ref_num = new_type & BUTTON_TYPE_MASK_REF1 ? 1 : 2;

			if (ok)
				ok = getRigToken();			// skip the section identifier
			if (ok)
				ok = skip(RIG_TOKEN_COLON);	// skip the colon

		}
		else if (IS_STATEMENT(rig_token.id))
		{
			display(dbg_parse + 1,"parsing update list(%s)",rigTokenToText(rig_token.id));
			if (*type & BUTTON_TYPE_UPDATE)
			{
				parse_error("There is already an update section defined for this button");
				ok = 0;
			}
			new_type = BUTTON_TYPE_UPDATE;
		}

		if (ok)
		{
			is_parse_button = 1;
			*type |= new_type;
			refs[ref_num] = parse_rig->num_statements + 1;
				// statement refs are 1 based
			ok = handleStatementList();
			is_parse_button = 0;
		}
	}

	if (ok)
		display(dbg_parse + 2,"handleSubsections() finished on %s",rigTokenToText(rig_token.id));
	return ok;
}



static bool handleButton()
	// button update sections and buttons are parsed once
	// and then applied to all buttons in the list
{
	display(dbg_parse + 1,"handleButton()",0);
	proc_entry();

	//-----------------------------------
	// Parse the BUTTON(...) part
	//-----------------------------------
	// to get a mask of buttons

	bool started = 0;
	uint32_t button_mask = 0;
	bool ok = getRigToken();
	ok = ok && skip(RIG_TOKEN_LEFT_PAREN);

	while (ok && rig_token.id == RIG_TOKEN_NUMBER)
	{
		if (!started && rig_token.id != RIG_TOKEN_NUMBER)
		{
			parse_error("at least one BUTTON number expected");
			ok = 0;
		}
		else
		{
			int button_num = rig_token.int_value;
			if (button_num < 0 || button_num >= NUM_BUTTONS)
			{
				parse_error("BUTTON number(%d) must be between 0 and 24",button_num);
				ok = 0;
			}
			else
			{
				display(dbg_parse + 2,"BUTTON_NUM = %d",button_num);
				started = 1;
				uint32_t mask = 1 << button_num;
				display(dbg_parse + 2,"added button_num(0x%02x) to mask(0x%02x)",button_num,mask);
				// PRH still need to check that this button was not previously defined

				if (button_mask & mask)
				{
					parse_error("BUTTON number(%d) listed more than once",button_num);
					ok = 0;
				}
				else
				{
					button_mask |= mask;
				}
			}

			if (ok)
			{
				ok = getRigToken();		// skip the BUTTON number
				if (ok && rig_token.id == RIG_TOKEN_COMMA)
				{
					ok = getRigToken();		// skip the COMMA
					if (ok && rig_token.id != RIG_TOKEN_NUMBER)
					{
						parse_error("BUTTON number expected");
						ok = 0;
					}
				}
			}
		}
	}

	ok = ok && skip(RIG_TOKEN_RIGHT_PAREN);
	ok = ok && skip(RIG_TOKEN_COLON);

	//-----------------------------------
	// Now parse the sections
	//------------------------------------
	// to get the type and three refs

	uint16_t type = 0;
	uint16_t refs[3] = {0,0,0};
	if (ok)
	{
		if (rig_token.id == RIG_TOKEN_INHERIT)
		{
			if (!(parse_rig->rig_type & RIG_TYPE_MODAL))
			{
				parse_error("INHERIT may only be used in ModalRigs");
				ok = 0;
			}
			else
			{
				display(dbg_parse + 1,"    INHERIT",0);
				ok = getRigToken();							// skip the INHERIT
				ok = ok && skip(RIG_TOKEN_SEMICOLON, true);	// may end on this
				type = BUTTON_TYPE_INHERIT;
			}
		}
		else
		{
			ok = handleSubsections(&type,refs);
		}
	}

	// check special case that THE_SYSTEM_BUTTON may
	// only specify BUTTON_TYPE_CLICK or INHERIT

	if (ok &&
		(button_mask & (1 << THE_SYSTEM_BUTTON)) &&
		((type & ~(BUTTON_TYPE_INHERIT | BUTTON_TYPE_UPDATE)) != BUTTON_TYPE_CLICK))
	{
		parse_error("Only CLICK (not 0x%04x) can be specified for THE_SYSTEM_BUTTON(4) in BaseRigs",type);
		ok = 0;
	}

	//----------------------------
	// apply the type and refs
	//----------------------------
	// to all the buttons in the mask


	if (ok)
	{
		int button_num = 0;
		while (button_mask)
		{
			if (button_mask & 1)
			{
				display(dbg_parse + 1,"SETTING BUTTON(%d) type(0x%04x) refs: 0x%04x, 0x%04x, 0x%04x",
					button_num,type,refs[0],refs[1],refs[2]);
				parse_rig->button_type[button_num] = type;
				memcpy(parse_rig->button_refs[button_num],refs,3 * sizeof(uint16_t));
			}
			button_num++;
			button_mask >>= 1;
		}
	}


	proc_leave();
	if (ok)
		display(dbg_parse + 2,"handleButton() returning %d",ok);
	return ok;
}



//--------------------------
// rigStats
//--------------------------

static void rigStats(const char *name, const rig_t *rig)
{
	display(dbg_stats,"Rig(%s) Total Size = %d",name,
		sizeof(rig_t) +
		rig->define_pool_len +
		rig->string_pool_len +
		rig->statement_pool_len +
		rig->expression_pool_len);
	display(dbg_stats + 1,"define_pool     = %d",rig->define_pool_len);
	display(dbg_stats + 1,"string_pool     = %d",rig->string_pool_len);
	display(dbg_stats + 1,"statement_pool  = %d",rig->statement_pool_len);
	display(dbg_stats + 1,"expression_pool = %d",rig->expression_pool_len);
	display(dbg_stats + 1,"num_statements  = %d",rig->num_statements);
	display(dbg_stats + 1,"MAX_RIG_SIZE    = %d",MAX_RIG_SIZE);
	display(dbg_stats + 1,"RIG_POOL_SIZE   = %d",RIG_POOL_SIZE);
}


//---------------------------------------
// loadDefaultRig()
//---------------------------------------

#if WITH_DEFAULT_RIG

	const rig_t *loadDefaultRig(const char *rig_name)
	{
		display(dbg_parse,"loadDefaultRig(%s)",rig_name);

		bool is_base_rig = !strcmp(rig_name,DEFAULT_RIG_TOKEN);
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
		rig->rig_type |= RIG_TYPE_SYSTEM;
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
const rig_t *parseRig(const char *rig_name, uint16_t how)
	// generates the intermediate structure of a
	// syntactically valid set of bytes that are
	// RIG_ID's, and with inline TEXT and int_values;

{
	warning(dbg_parse,"ParseRig(%s.rig,%d)",rig_name,how);

	#if WITH_DEFAULT_RIG
		if (!strcmp(rig_name,DEFAULT_RIG_TOKEN) || (	// we WILL load the default base rig if called with base_only
			!how &&										// but we will NOT load the default_modal rig with any how.
			rig_pool_len &&								// we only load the default modal rig if it is called from the
			(base_rig->rig_type & RIG_TYPE_SYSTEM) &&	// SYSTEM base rig
			!strcmp(rig_name,DEFAULT_MODAL_TOKEN)))
		{
			return loadDefaultRig(rig_name);
		}
	#endif

	bool ok = 0;
	rig_t *new_rig = 0;
	if (openRigFile(rig_name))
	{
		init_parse();
		ok = getRigToken();

		// rig type

		if (ok  && (
			rig_token.id < RIG_TOKEN_BASERIG ||
			rig_token.id > RIG_TOKEN_MODAL))
		{
			parse_error("must start with BaseRig or Overlay");
			ok = 0;
		}
		else if (rig_token.id == RIG_TOKEN_MODAL)
		{
			if (how & PARSE_HOW_BASE_ONLY)
			{
				parse_error("You may only load a BaseRig!!\n'%s.rig' is a ModalRig.",rig_name);
				ok = 0;
			}
			else
				parse_rig->rig_type |= RIG_TYPE_MODAL;
		}

		if (ok)
		{
			ok = getRigToken();
			if (!ok)
			{
				parse_error("unexpected end of program");
				ok = false;
			}
		}

		// init_statements and onUpdate statements
		// note that we SET the num_statements after each try
		// regardless if any were parsed

		if (ok && IS_INIT_STATEMENT(rig_token.id))
			ok = ok && handleStatementList();

		parse_rig->num_statements = 1;
		parse_rig->statements[parse_rig->num_statements] = parse_rig->statement_pool_len;

		if (ok && rig_token.id == RIG_TOKEN_ON_UPDATE)
		{
			ok = getRigToken();	// skip the onUpdate
			ok = ok && skip(RIG_TOKEN_COLON);
			ok = ok && handleStatementList();
		}

		parse_rig->num_statements = 2;
		parse_rig->statements[parse_rig->num_statements] = parse_rig->statement_pool_len;

		// buttons

		if (ok && rig_token.id != RIG_TOKEN_BUTTON)
		{
			if (rig_token.id == RIG_TOKEN_EOF)
			{
				ok = false;
				// this is no longer technically true
				parse_error("At least one BUTTON must be implemented");
			}
			else if (rig_token.id != RIG_TOKEN_BUTTON)
			{
				ok = false;
				parse_error("first BUTTON expected, NOT %d=%s",rig_token.id,rigTokenToString(rig_token.id));
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
				parse_error("BUTTON expected, NOT %d=%s",rig_token.id,rigTokenToString(rig_token.id));
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
			parse_error("A modalRig must contain at least one endModal statement.");
			ok = 0;
		}

		ok = ok && !parse_error_found;
		if (ok)
		{
			warning(dbg_parse,"parseRig(%s.rig) finished OK",rig_name);

			// if any how flags are set, we don't relocate

			new_rig = how ? parse_rig : relocate();
			if (new_rig)
			{
				rigStats(rig_name,new_rig);

				// here we dump H files solely dependent on the actual filenames,
				// this has nothing to do with what shows, or is passed around
				// to accomplish things.

				if (how & PARSE_HOW_DUMP_H_FILE)
					dumpRigHeaderFile(rig_name, new_rig);
			}
			else
			{
				parse_error("There was an error relocating the rig!");
			}

		}
		else
		{
			my_error("There was an error parsing the rig!",0);
		}

	}	// file opened

	return new_rig;
}	// parseRig()


// end of rigParser.cpp
