//-------------------------------------------------------
// rigExpression.cpp
//-------------------------------------------------------
// Contains the implementation of the rig Expression
// Code Generator

#include <myDebug.h>
#include "rigExpression.h"
#include "rigParser.h"

static int debug_exp_offset = -1;	// 208;
	// set this to the offset of a known problem expression
	// and it will be displayed at dbg_exp = -2 level

static int dbg_exp = 1;
	// 0 = outer level expressions
	// -1 = expressions
	// -2 = everyting

#define SHOW_OUTPUT_BYTES  0

#define EXP_TYPE_ILLEGAL		-1
#define EXP_TYPE_NUMBER 		0
#define EXP_TYPE_STRING 		1
#define EXP_TYPE_LED_COLOR 		2
#define EXP_TYPE_DISPLAY_COLOR 	3


static bool addExpByte(rig_t *rig, uint8_t byte)
{
	display(dbg_exp+2,"addExpByte(%d,0x%02x)",rig->expression_pool_len,byte);
	if (rig->expression_pool_len >= MAX_EXPRESSION_POOL)
	{
		parse_error("EXPRESSION POOL OVERFLOW");
		return false;
	}

	#if SHOW_OUTPUT_BYTES
	if (dbgSerial)
	{
		dbgSerial->print("{");
		dbgSerial->print(byte,HEX);
		dbgSerial->print("}");
	}
	#endif

	uint8_t *pool = (uint8_t *) rig->expression_pool;
	pool[rig->expression_pool_len++] = byte;
	return true;
}


static uint16_t findIdentifier(rig_t *rig, const char *id)
	// returns 1 based index to define
{
	for (uint16_t idx=0; idx<RIG_NUM_DEFINES; idx++)
	{
		uint16_t offset = rig->define_ids[idx];
		if (offset)
		{
			const char *s = &rig->define_pool[offset-1];
			if (!strcmp(id,s))
				return idx + 1;
		}
	}
	return  0;
}


//-------------------------------------------
// expression
//-------------------------------------------

static int factor(rig_t *rig);
static int term(rig_t *rig);
static int exp(rig_t *rig);


static int atom(rig_t *rig)
{
	display(dbg_exp + 2,"atom(%s)",rigTokenToString(rig_token.id));
	proc_entry();

	bool ok = 1;
	uint16_t dbg_ret = 0;
	int type = EXP_TYPE_ILLEGAL;

	if (rig_token.id == RIG_TOKEN_BUTTON_NUM ||
		rig_token.id == RIG_TOKEN_BUTTON_ROW ||
		rig_token.id == RIG_TOKEN_BUTTON_COL )
	{
		extern int is_parse_button;
		display(dbg_exp+1,"%s  is_parse_button=%d",rigTokenToString(rig_token.id),is_parse_button);
		if (!is_parse_button)
		{
			parse_error("%s may only used within button statements",rigTokenToString(rig_token.id));
		}
		else
		{
			type = EXP_TYPE_NUMBER;
			uint8_t byte1 =
				rig_token.id == RIG_TOKEN_BUTTON_ROW ? EXP_BUTTON_ROW :
				rig_token.id == RIG_TOKEN_BUTTON_COL ? EXP_BUTTON_COL :
				EXP_BUTTON_NUM;
			uint8_t byte0 = EXP_INLINE | EXP_INLINE_BUTTON | EXP_NUMBER;
			ok = addExpByte(rig,byte0);
			ok = ok && addExpByte(rig,byte1);
			ok = ok && getRigToken();
			dbg_ret = (byte0 << 8) | byte1;
		}
	}
	else if (IS_LED_COLOR(rig_token.id))
	{
		display(dbg_exp + 1,"LED_COLOR = %s",rigTokenToString(rig_token.id));
		type = EXP_TYPE_LED_COLOR;
		uint8_t byte0 = EXP_INLINE | EXP_LED_COLOR;
		uint8_t byte1 = RIG_LED_COLOR(rig_token.id);
		ok = addExpByte(rig, byte0);
		ok = ok && addExpByte(rig, byte1);
		ok = ok && getRigToken();
		dbg_ret = (byte0 << 8) | byte1;
	}
	else if (IS_DISPLAY_COLOR(rig_token.id))
	{
		display(dbg_exp + 1,"DISPLAY_COLOR = %s",rigTokenToString(rig_token.id));
		type = EXP_TYPE_DISPLAY_COLOR;
		uint8_t byte0 = EXP_INLINE | EXP_DISPLAY_COLOR;
		uint8_t byte1 = RIG_DISPLAY_COLOR(rig_token.id);
		ok = addExpByte(rig, byte0);
		ok = ok && addExpByte(rig, byte1);
		ok = ok && getRigToken();
		dbg_ret = (byte0 << 8) | byte1;
	}
	else if (rig_token.id == RIG_TOKEN_NUMBER)
	{
		display(dbg_exp + 1,"NUMBER = %d",rig_token.int_value);
		if (rig_token.int_value > 255)
		{
			parse_error("EXPRESSION NUMBERS must be in range of 0..255");
			ok = 0;;
		}
		else
		{
			type = EXP_TYPE_NUMBER;
			uint8_t byte0 = EXP_INLINE | EXP_NUMBER;
			uint8_t byte1 = rig_token.int_value;
			ok = addExpByte(rig, byte0);
			ok = ok && addExpByte(rig, byte1);
			ok = ok && getRigToken();
			dbg_ret = (byte0 << 8) | byte1;
		}
	}
	else if (rig_token.id == RIG_TOKEN_IDENTIFIER)
	{
		display(dbg_exp + 1,"IDENTIFIER = %s",rig_token.text);
		uint16_t idxP1 = findIdentifier(rig, rig_token.text);
		if (!idxP1)
		{
			parse_error("DEFINE(%s) not found",rig_token.text);
			ok = 0;;
		}
		else
		{
			idxP1--;	// findIdentifier90 returns one bawed
			type = EXP_TYPE_NUMBER;
			uint8_t byte0 = EXP_INLINE | EXP_NUMBER;
			uint8_t byte1 = rig->define_values[idxP1];
			ok = ok && addExpByte(rig, byte0);
			ok = ok && addExpByte(rig, byte1);
			ok = ok && getRigToken();
			dbg_ret = (byte0 << 8) | byte1;
		}
	}
	else if (rig_token.id == RIG_TOKEN_TEXT)
	{
		display(dbg_exp + 1,"TEXT = %s",rig_token.text);
		type = EXP_TYPE_STRING;
		ok = ok && addExpByte(rig, EXP_TEXT);
		for (uint16_t i=0; ok && i<strlen(rig_token.text); i++)
		{
			ok = addExpByte(rig, rig_token.text[i]);
		}
		ok = ok && addExpByte(rig, 0);	// NULL Terminator
		ok = ok && getRigToken();
		dbg_ret = type;
	}
	else if (
		rig_token.id == RIG_TOKEN_VALUE ||
		rig_token.id == RIG_TOKEN_STRING)
	{
		int save_tt = rig_token.id;
		int opcode = rig_token.id == RIG_TOKEN_VALUE ? EXP_VALUE : EXP_STRING;

		// dbg_ret = type;

		ok = getRigToken();
		if (ok && rig_token.id != RIG_TOKEN_LEFT_BRACKET)
		{
			parse_error("LEFT_BRACKET expected");
			ok = false;
		}

		ok = ok && addExpByte(rig, opcode);
		ok = ok && getRigToken();

		if (ok)
		{
			display(dbg_exp + 1,"%s[exp]",rigTokenToString(rig_token.id));

			uint16_t where = rig->expression_pool_len;		// save offset in case inline
			int sub_type = exp(rig);						// recurse
			ok = ok && sub_type != EXP_TYPE_ILLEGAL;
			if (ok && sub_type != EXP_TYPE_NUMBER)
			{
				parse_error("INDEXES must be a numeric expression");
				ok = false;
			}
			if (ok)
			{
				bool exp_inline = 0;

				uint8_t byte0 = rig->expression_pool[where];
				uint8_t byte1 = rig->expression_pool[where + 1];
				uint8_t inline_bits = byte0 & (EXP_INLINE | EXP_INLINE_BUTTON);
				uint8_t inline_op = byte0 & ~(EXP_INLINE | EXP_INLINE_BUTTON);

				// note that INLINE_BUTTON is retained when promoting, so,
				// for exmple, it is possible to have VALUE[_BUTTON_NUM] be
				// represented by a single inline expression, i.e.
				//
				//    byte0 = EXP_INLINE | EXP_INLINE_BUTTON | EXP_VALUE
				//    byte1 = EXP_BUTTON_NUM, ROW, or COL

				if (rig->expression_pool_len - where == 2)
				{
					exp_inline = 1;			// don't write the EXP_RIGHT_BRACKET

					display(dbg_exp + 1,"--- promoting 0x%02x%02x back into 0x%02x%02x",
						byte0,byte1,
						opcode | EXP_INLINE,
						byte1);

					if (!(byte0 & EXP_INLINE))
					{
						parse_error("Expected EXP_INLINE with expression length of 2, not 0x%02x,0x%02x",byte0,byte1);
						ok = false;
					}
					else if (inline_op != EXP_NUMBER)
					{
						parse_error("Expected INLINE EXP_NUMBER with expression length of 2, not 0x%02x,0x%02x",byte0,byte1);
						ok = false;
					}

					// note that the EXP_NUMBER gets lost here,
					// because the result is EXP_VALUE or EXP_STRING.
					// but that EXP_INLINE_BUTTON, if present, is retained

					if (ok)
					{
						byte0 = opcode | inline_bits;
						uint8_t *pool = (uint8_t *) rig->expression_pool;
						pool[where-1] = byte0;
						pool[where]   = byte1;
						rig->expression_pool_len = where + 1;
					}
				}

				if (ok)
				{
					if (rig_token.id != RIG_TOKEN_RIGHT_BRACKET)
					{
						parse_error("RIGHT_BRACKET expected");
						ok = false;
					}
					else if (!exp_inline)
					{
						ok = addExpByte(rig, EXP_RIGHT_BRACKET);
					}
				}

				ok = ok && getRigToken();
				if (ok)
				{
					dbg_ret = (byte0 << 8) | byte1;;
					type = (save_tt == RIG_TOKEN_VALUE) ? EXP_TYPE_NUMBER : EXP_TYPE_STRING;

				}
			}
		}
	}	// VALUE[] or STRING[]


	if (!ok)
		type = EXP_TYPE_ILLEGAL;

	proc_leave();
	display(dbg_exp + 2,"atom() returning type(%d) value(0x%04x) on %s",type,dbg_ret,rigTokenToString(rig_token.id));
	return type;
}



static int factor(rig_t *rig)
{
	display(dbg_exp + 2,"factor(%s)",rigTokenToString(rig_token.id));
	proc_entry();

	bool ok = 1;
	int type = EXP_TYPE_ILLEGAL;

	if (rig_token.id == RIG_TOKEN_NOT)
	{
		display(dbg_exp + 1,"NOT exp",0);
		ok = addExpByte(rig, EXP_NOT);
		ok = ok && getRigToken();
		if (ok)
			type = factor(rig);	// recurse
		ok = ok && type != EXP_TYPE_ILLEGAL;
		if (ok && type != EXP_TYPE_NUMBER)
		{
			parse_error("NOT(!) may only be used on NUMERIC expressions");
			ok = false;
		}
	}
	else if (rig_token.id == RIG_TOKEN_LEFT_PAREN)
	{
		display(dbg_exp + 1,"( exp )",0);
		ok = addExpByte(rig, EXP_LEFT_PAREN);
		ok = ok && getRigToken();
		if (ok)
		{
			type = exp(rig);	// recurse
			ok = ok && type != EXP_TYPE_ILLEGAL;
			if (ok)
			{
				if (rig_token.id != RIG_TOKEN_RIGHT_PAREN)
				{
					parse_error("RIGHT_PAREN expected");
					ok = false;
				}
				else
				{
					ok = ok && addExpByte(rig, EXP_RIGHT_PAREN);
					ok = ok && getRigToken();
				}
			}
		}
	}
	else if (
		rig_token.id == RIG_TOKEN_NUMBER ||
		rig_token.id == RIG_TOKEN_IDENTIFIER ||
		rig_token.id == RIG_TOKEN_BUTTON_NUM ||
		rig_token.id == RIG_TOKEN_BUTTON_ROW ||
		rig_token.id == RIG_TOKEN_BUTTON_COL ||
		rig_token.id == RIG_TOKEN_TEXT ||
		rig_token.id == RIG_TOKEN_VALUE ||
		rig_token.id == RIG_TOKEN_STRING ||
		IS_LED_COLOR(rig_token.id) ||
		IS_DISPLAY_COLOR(rig_token.id))
	{
		type = atom(rig);
	}

	if (!ok)
		type = EXP_TYPE_ILLEGAL;

	proc_leave();
	display(dbg_exp + 2,"factor() returning type(%d) on %s",type,rigTokenToString(rig_token.id));
	return type;
}


static int term(rig_t *rig)
{
	display(dbg_exp + 2,"term(%s)",rigTokenToString(rig_token.id));
	proc_entry();

	bool ok = 1;
	int type = factor(rig);

	if (type == EXP_TYPE_NUMBER)
	{
		while (ok && IS_BIN_OP(rig_token.id))
		{
			display(dbg_exp + 1,"%s factor",rigTokenToString(rig_token.id));
			ok = addExpByte(rig, EXP_PLUS + BIN_OP(rig_token.id));
			ok = ok && getRigToken();
			if (ok)
			{
				int sub_type = factor(rig);	// recurse
				ok = ok && (sub_type != EXP_TYPE_ILLEGAL);
				if (ok && sub_type != EXP_TYPE_NUMBER)
				{
					parse_error("BIN_OP expects numeric term");
					ok = false;
				}
			}
		}
	}

	if (!ok)
		type  = EXP_TYPE_ILLEGAL;

	proc_leave();
	display(dbg_exp + 2,"term() returning type(%d) on %s",type,rigTokenToString(rig_token.id));
	return type;
}



static int exp(rig_t *rig)
{
	display(dbg_exp + 1,"exp(%s)",rigTokenToString(rig_token.id));
	proc_entry();

	bool ok = 1;
	int type = term(rig);
	if (type == EXP_TYPE_NUMBER)
	{
		if (rig_token.id == RIG_TOKEN_QUESTION)
		{
			display(dbg_exp + 1,"? exp",rigTokenToString(rig_token.id));
			ok = addExpByte(rig, EXP_QUESTION);
			ok = ok && getRigToken();

			type = exp(rig);	// recurse
			ok = ok && type != EXP_TYPE_ILLEGAL;
			if (ok)
			{
				if (rig_token.id != RIG_TOKEN_COLON)
				{
					parse_error("COLON expected");
					ok = false;
				}
				else
				{
					display(dbg_exp + 1,": exp",rigTokenToString(rig_token.id));
					ok = addExpByte(rig, EXP_COLON);
					ok = ok && getRigToken();
				}
				if (ok)
				{
					int type2 = exp(rig);	// recurse
					ok = ok && type2 != EXP_TYPE_ILLEGAL;
					if (ok && type2 != type)
					{
						parse_error("Conditional terminals must evaluate to same type");
						ok = false;
					}
				}
			}
		}
	}

	if (!ok)
		type = EXP_TYPE_ILLEGAL;

	proc_leave();
	display(dbg_exp + 1,"exp() returning type(%d) on %s",type,rigTokenToString(rig_token.id));
	return type;
}


//-------------------------------------------
// expression helpers
//-------------------------------------------

static uint16_t inlineExp(rig_t *rig, const char *what, uint16_t offset, uint8_t max_value, uint8_t min_value)
{
	// we are reading real uint16_t's out of the expression pool
	// which are LSB first

	uint8_t byte0 = rig->expression_pool[offset];
	uint8_t byte1 = rig->expression_pool[offset + 1];

	// OK, so null strings come in as 0x08 0x00
	// which *can* be considered an inline expression and removed from the expression pool,
	// BUT I have to make sure that evaulation understands this.

	if (rig->expression_pool_len - offset == 3)		// if (byte0 & EXP_INLINE)
	{
		display(dbg_exp+1,"---> INLINE(0x%02) value(0x%02x)  max=%d",byte1,byte0,max_value);

		if (byte0 == EXP_TEXT &&
			byte1 == 0)
		{
			display(dbg_exp,"    promoting inline null EXP_TEXT",0);
			byte0 |= EXP_INLINE;
		}

		// sanity check

		if (!(byte0 & EXP_INLINE))
		{
			parse_error("Expected an INLINE expression, not 0x%02x, 0x%02x", byte0,byte1);
			offset = 0;
		}

		// check for numbers in range

		else if ((max_value && byte1 > max_value) ||
				 (min_value && byte1 < min_value))
		{
			parse_error("%s(%d) must be between %d and %d",what,byte1,min_value,max_value);
			offset = 0;
		}

		// it's ok, rewind the pool and return it

		else
		{
			rig->expression_pool_len = offset;	// roll back the pool
			offset = (byte0 << 8) | byte1;		// return the inline word
		}
	}
	else
	{
		offset += 1;	// make real offsets one based
	}
	return offset;
}


static uint16_t genericExpression(rig_t *rig, const char *what, int expected, uint8_t max_value=0, uint8_t min_value=0)
{
	uint16_t offset = rig->expression_pool_len;

	bool debug_this = 0;
	int save_exp = dbg_exp;
	if (offset == debug_exp_offset)
	{
		debug_this 	= 1;
		dbg_exp     = -3;
	}

	display(dbg_exp + 1,"%s expression",what);
	int save_proc_level = proc_level;
	proc_level = 0;

	int type = exp(rig);
	if (type != expected)
	{
		if (type != EXP_TYPE_ILLEGAL)	// already reported
			parse_error("%s expression expected",what);
		proc_level = save_proc_level;
		return 0;
	}
	if (!addExpByte(rig, EXP_END))
	{
		proc_level = save_proc_level;
		return 0;
	}

	proc_level = save_proc_level;
	offset = inlineExp(rig, what, offset, max_value, min_value);
	display(dbg_exp,"%s expression returning 0x%04x",what,offset);

	if (debug_this)
	{
		dbg_exp = save_exp;
	}

	return offset;
}


//-------------------------------------------
// specific expression types
//-------------------------------------------

uint16_t rigNumericExpression(rig_t *rig)
{
	return genericExpression(rig, "NUMERIC", EXP_TYPE_NUMBER);
}


uint16_t rigPedalNumExpression(rig_t *rig)
{
	return genericExpression(rig, "PEDAL_NUM", EXP_TYPE_NUMBER,NUM_PEDALS - 1);
}
uint16_t rigRotaryNumExpression(rig_t *rig)
{
	return genericExpression(rig, "PEDAL_NUM", EXP_TYPE_NUMBER,NUM_ROTARY - 1);
}
uint16_t rigAreaNumExpression(rig_t *rig)
{
	return genericExpression(rig, "AREA_NUM", EXP_TYPE_NUMBER,RIG_NUM_AREAS - 1);
}
uint16_t rigListenNumExpression(rig_t *rig)
{
	return genericExpression(rig, "LISTEN_NUM", EXP_TYPE_NUMBER,RIG_NUM_LISTENS - 1);
}
uint16_t rigValueNumExpression(rig_t *rig)
{
	return genericExpression(rig, "VALUE_NUM",EXP_TYPE_NUMBER,RIG_NUM_VALUES-1);
}


uint16_t rigValueExpression(rig_t *rig)
{
	return genericExpression(rig, "VALUE_NUM",EXP_TYPE_NUMBER,MAX_RIG_VALUE);
}
uint16_t rigMidiChannelExpression(rig_t *rig)
{
	return genericExpression(rig, "MIDI_CHANNEL",EXP_TYPE_NUMBER,MIDI_MAX_CHANNEL,MIDI_MIN_CHANNEL);
}
uint16_t rigListenChannelExpression(rig_t *rig)
{
	return genericExpression(rig, "LISTEN_CHANNEL",EXP_TYPE_NUMBER,MIDI_MAX_CHANNEL);
}
uint16_t rigMidiValueExpression(rig_t *rig)
{
	return genericExpression(rig, "MIDI_VALUE",EXP_TYPE_NUMBER,MIDI_MAX_VALUE);
}


uint16_t rigStringExpression(rig_t *rig)
{
	return genericExpression(rig, "STRING", EXP_TYPE_STRING);
}
uint16_t rigLedColorExpression(rig_t *rig)
{
	return genericExpression(rig, "LED_COLOR", EXP_TYPE_LED_COLOR);
}
uint16_t rigDisplayColorExpression(rig_t *rig)
{
	return genericExpression(rig, "DISPLAY_COLOR", EXP_TYPE_DISPLAY_COLOR);
}
