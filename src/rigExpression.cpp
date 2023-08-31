//-------------------------------------------------------
// rigExpression.cpp
//-------------------------------------------------------
// Contains the implementation of the rig Expression Code
// genearator and interpreter.

#include <myDebug.h>
#include "rigExpression.h"
#include "rigParser.h"

#define dbg_exp 1
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
	if (rig->expression_pool_len >= MAX_EXPRESSION_POOL)
	{
		rig_error("EXPRESSION POOL OVERFLOW");
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

static int factor(rig_t *rig, int tt);
static int term(rig_t *rig, int tt);
static int exp(rig_t *rig, int tt);

static int atom(rig_t *rig, int tt)
{
	display(dbg_exp + 2,"atom(%s)",rigTokenToString(tt));
	proc_entry();

	bool ok = 1;
	uint16_t dbg_ret = 0;
	int type = EXP_TYPE_ILLEGAL;

	if (IS_LED_COLOR(tt))
	{
		display(dbg_exp + 1,"LED_COLOR = %s",rigTokenToString(tt));
		type = EXP_TYPE_LED_COLOR;
		uint8_t byte = EXP_INLINE | EXP_LED_COLOR;
		ok = addExpByte(rig, byte);
		ok = ok && addExpByte(rig, LED_COLOR(tt));
		ok = ok && getRigToken();
		dbg_ret = (byte << 8) | LED_COLOR(tt);
	}
	else if (IS_DISPLAY_COLOR(tt))
	{
		display(dbg_exp + 1,"DISPLAY_COLOR = %s",rigTokenToString(tt));
		type = EXP_TYPE_DISPLAY_COLOR;
		uint8_t byte = EXP_INLINE | EXP_DISPLAY_COLOR;
		ok = addExpByte(rig, byte);
		ok = ok && addExpByte(rig, DISPLAY_COLOR(tt));
		ok = ok && getRigToken();
		dbg_ret = (byte << 8) | DISPLAY_COLOR(tt);
	}
	else if (tt == RIG_TOKEN_NUMBER)
	{
		display(dbg_exp + 1,"NUMBER = %d",rig_token.int_value);
		if (rig_token.int_value > 255)
		{
			rig_error("EXPRESSION NUMBERS must be in range of 0..255");
			ok = 0;;
		}
		type = EXP_TYPE_NUMBER;
		uint8_t num = rig_token.int_value;
		uint8_t byte = EXP_INLINE | EXP_NUMBER;
		ok = ok && addExpByte(rig, byte);
		ok = ok && addExpByte(rig, num);
		ok = ok && getRigToken();
		dbg_ret = (byte << 8) | num;
	}
	else if (tt == RIG_TOKEN_IDENTIFIER)
	{
		display(dbg_exp + 1,"IDENTIFIER = %s",rig_token.text);
		uint16_t idxP1 = findIdentifier(rig, rig_token.text);
		if (!idxP1)
		{
			rig_error("DEFINE(%s) not found",rig_token.text);
			ok = 0;;
		}
		type = EXP_TYPE_NUMBER;
		uint8_t num = idxP1 - 1;
		uint8_t byte = EXP_INLINE | EXP_INLINE_ID | EXP_DEFINE;
		ok = ok && addExpByte(rig, byte);
		ok = ok && addExpByte(rig, num);
		ok = ok && getRigToken();
		dbg_ret = (byte << 8) | num;
	}
	else if (tt == RIG_TOKEN_TEXT)
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
		tt == RIG_TOKEN_VALUE ||
		tt == RIG_TOKEN_STRING)
	{
		int what = tt;
		int OP_TYPE = tt == RIG_TOKEN_VALUE ? EXP_VALUE : EXP_STRING;
		type = tt == RIG_TOKEN_VALUE ? EXP_TYPE_NUMBER : EXP_TYPE_STRING;
		dbg_ret = type;

		tt = getRigToken();
		if (tt != RIG_TOKEN_LEFT_BRACKET)
		{
			ok = false;
			rig_error("LEFT_BRACKET expected");
		}
		tt = getRigToken();
		ok = ok && tt;
		ok = addExpByte(rig, OP_TYPE);

		if (ok)
		{
			display(dbg_exp + 1,"%s[exp]",rigTokenToString(what));

			uint16_t where = rig->expression_pool_len;
			int sub_type = exp(rig, tt);		// recurse

			if (sub_type != EXP_TYPE_NUMBER)
			{
				rig_error("INDEXES must be a numeric expression");
				ok = 0;
			}
			else	// successful sub expression
			{
				bool exp_inline = 0;
				uint8_t byte0 = rig->expression_pool[where];
				uint8_t byte1 = rig->expression_pool[where + 1];

				uint8_t bits_inline = byte0 & (EXP_INLINE | EXP_INLINE_ID);
				uint8_t sub_op = byte0 & ~(EXP_INLINE | EXP_INLINE_ID);

				if (rig->expression_pool_len - where == 2 && 	// sub expression was completely inline
					bits_inline && (								// just to be sure
					sub_op == EXP_NUMBER ||							// and it's a number or identifier
					sub_op == EXP_DEFINE))
				{
					exp_inline = 1;			// we will promote it to the previous op
					display(dbg_exp + 1,"--- promoting 0x%02x%02x back into 0x%02x", byte0,byte1,OP_TYPE);

					// note that the EXP_NUMBER or EXP_DEEFINE gets lost here,
					// because the result must still be EXP_VALUE or EXP_STRING.
					// thus the client (dumpOp) must look at the high order bits
					// to determine how to evaulate the complicated op code

					uint8_t *pool = (uint8_t *) rig->expression_pool;
					pool[where-1] = OP_TYPE | bits_inline;
					pool[where] = byte1;
					rig->expression_pool_len = where + 1;
					dbg_ret = ((OP_TYPE | bits_inline) << 8) | byte1;;
				}

				tt = rig_token.id;
				ok = ok && tt;
				if (ok && tt != RIG_TOKEN_RIGHT_BRACKET)
				{
					ok = false;
					rig_error("RIGHT_BRACKET expected");
				}
				if (!exp_inline)
					ok = ok && addExpByte(rig, EXP_RIGHT_BRACKET);
					// needed for linear dump()

				ok = ok && getRigToken();
			}
		}
	}	// VALUE[] or STRING[]

	if (!ok)
		type = EXP_TYPE_ILLEGAL;

	proc_leave();
	display(dbg_exp + 2,"atom() returning type(%d) value(0x%04x) on %s",type,dbg_ret,rigTokenToString(rig_token.id));
	return type;
}



static int factor(rig_t *rig, int tt)
{
	display(dbg_exp + 2,"factor(%s)",rigTokenToString(tt));
	proc_entry();

	bool ok = 1;
	int type = EXP_TYPE_ILLEGAL;

	if (tt == RIG_TOKEN_NOT)
	{
		display(dbg_exp + 1,"NOT exp",0);
		ok = addExpByte(rig, EXP_NOT);
		tt = getRigToken();
		ok = ok && tt;
		if (ok)
			type = factor(rig, tt);
		if (type != EXP_TYPE_NUMBER)
		{
			rig_error("NOT(!) may only be used on NUMERIC expressions");
			ok = false;
		}
	}
	else if (tt == RIG_TOKEN_LEFT_PAREN)
	{
		display(dbg_exp + 1,"( exp )",0);
		ok = addExpByte(rig, EXP_LEFT_PAREN);
		tt = getRigToken();
		ok = ok && tt;
		if (ok)
			type = exp(rig, tt);
		ok = ok && type != EXP_TYPE_ILLEGAL;
		if (ok && rig_token.id != RIG_TOKEN_RIGHT_PAREN)
		{
			rig_error("RIGHT_PAREN expected");
			ok = false;
		}
		else
		{
			ok = addExpByte(rig, EXP_RIGHT_PAREN);
			tt = getRigToken();
			ok = ok && tt;
		}
	}
	else if (
		tt == RIG_TOKEN_NUMBER ||
		tt == RIG_TOKEN_IDENTIFIER ||
		tt == RIG_TOKEN_TEXT ||
		tt == RIG_TOKEN_VALUE ||
		tt == RIG_TOKEN_STRING ||
		IS_LED_COLOR(tt) ||
		IS_DISPLAY_COLOR(tt))
	{
		type = atom(rig, tt);
	}

	if (!ok)
		type = EXP_TYPE_ILLEGAL;

	proc_leave();
	display(dbg_exp + 2,"factor() returning %d on %s",type,rigTokenToString(rig_token.id));
	return type;
}


static int term(rig_t *rig, int tt)
{
	display(dbg_exp + 2,"term(%s)",rigTokenToString(tt));
	proc_entry();

	bool ok = 1;
	int type = factor(rig, tt);
	tt = rig_token.id;

	if (type == EXP_TYPE_NUMBER)
	{
		while (ok && IS_BIN_OP(tt))
		{
			display(dbg_exp + 1,"%s factor",rigTokenToString(tt));
			ok = addExpByte(rig, EXP_PLUS + BIN_OP(tt));
			tt = getRigToken();
			ok = ok && tt;
			if (ok)
			{
				int sub_type = factor(rig, tt);
				tt = rig_token.id;
				if (sub_type != EXP_TYPE_NUMBER)
				{
					rig_error("BIN_OP expects numeric term");
					ok = 0;
				}
			}
		}
	}

	if (!ok)
		type  = EXP_TYPE_ILLEGAL;

	proc_leave();
	display(dbg_exp + 2,"term() returning %d on %s",type,rigTokenToString(rig_token.id));
	return type;
}



static int exp(rig_t *rig, int tt)
{
	display(dbg_exp + 1,"exp(%s)",rigTokenToString(tt));
	proc_entry();

	bool ok = 1;
	int type = term(rig, tt);
	if (type == EXP_TYPE_NUMBER)
	{
		tt = rig_token.id;
		if (tt == RIG_TOKEN_QUESTION)
		{
			display(dbg_exp + 1,"? exp",rigTokenToString(tt));
			ok = addExpByte(rig, EXP_QUESTION);
			tt = getRigToken();
			ok = ok && tt;

			type = exp(rig, tt);
			if (type != EXP_TYPE_ILLEGAL)
			{
				tt = rig_token.id;
				if (tt != RIG_TOKEN_COLON)
				{
					rig_error("COLON expected");
					ok = 0;
				}
				display(dbg_exp + 1,": exp",rigTokenToString(tt));
				ok = addExpByte(rig, EXP_COLON);
				tt = getRigToken();
				ok = ok && tt;

				if (ok)
				{
					int type2 = exp(rig, tt);
					if (type2 != type)
					{
						rig_error("Conditional terminals must evaluate to same type");
						ok = 0;
					}
				}
			}
		}
	}

	if (!ok)
		type = EXP_TYPE_ILLEGAL;

	proc_leave();
	display(dbg_exp + 1,"exp() returning %d on %s",type,rigTokenToString(rig_token.id));
	return type;
}


//-------------------------------------------
// expression helpers
//-------------------------------------------

static uint16_t inlineExp(rig_t *rig, const char *what, uint16_t offset, uint8_t max_value, uint8_t min_value)
{
	// inline expressions are identified if the length == 3
	// 2 for the inline expression and one for the EXP_END token

	if (rig->expression_pool_len - offset == 3)
	{
		// assuming the previous two bytes are a single inline expression
		// and we know it's a number since max was passed in

		uint8_t byte0 = rig->expression_pool[offset];
		uint8_t byte1 = rig->expression_pool[offset + 1];
		display(dbg_exp + 1,"---> INLINE 0x%02x%02x  max=%d",byte0,byte1,max_value);

		// get numeric value of inline define

		uint8_t value = byte1;
		display(dbg_exp + 2,"inline value = %d",value);

		if (byte0 & EXP_INLINE_ID)
		{
			value = rig->define_values[byte1];
			display(dbg_exp + 2,"got defined value = %d", value);
		}

		// check for numbers in range

		if ((max_value && value > max_value) ||
			(min_value && value < min_value))
		{
			rig_error("%s(%d) must be between %d and %d",what,value,min_value,max_value);
			// by commenting this out:
			// 		offset = 0;
			// we will show all the errors and then
			// fail parse due to rig-error egetting called
		}

		rig->expression_pool_len = offset;	// roll back the ppol
		offset = (byte0 << 8) | byte1;
	}
	else
	{
		offset += 1;	// make real offsets one based
	}
	return offset;
}


static uint16_t genericExpression(rig_t *rig, const char *what, int expected, int tt, uint8_t max_value=0, uint8_t min_value=0)
{
	display(dbg_exp + 1,"%s expression",what);
	uint16_t offset = rig->expression_pool_len;
	int save_proc_level = proc_level;
	proc_level = 0;

	int type = exp(rig, tt);
	if (type != expected)
	{
		rig_error("%s expression expected",what);
		proc_level = save_proc_level;
		return 0;
	}
	if (!addExpByte(rig, EXP_END))
	{
		proc_level = save_proc_level;
		return 0;
	}

	proc_level = save_proc_level;
	offset = inlineExp(rig, what,offset,max_value,min_value);
	display(dbg_exp,"%s expression returning 0x%04x",what,offset);
	return offset;

}


//-------------------------------------------
// specific expression types
//-------------------------------------------


uint16_t rigNumericExpression(rig_t *rig, int tt)
{
	return genericExpression(rig, "NUMERIC", EXP_TYPE_NUMBER,tt);
}


uint16_t rigPedalNumExpression(rig_t *rig, int tt)
{
	return genericExpression(rig, "PEDAL_NUM", EXP_TYPE_NUMBER,tt,NUM_PEDALS - 1);
}
uint16_t rigRotaryNumExpression(rig_t *rig, int tt)
{
	return genericExpression(rig, "PEDAL_NUM", EXP_TYPE_NUMBER,tt,NUM_ROTARY - 1);
}
uint16_t rigAreaNumExpression(rig_t *rig, int tt)
{
	return genericExpression(rig, "AREA_NUM", EXP_TYPE_NUMBER,tt,RIG_NUM_AREAS - 1);
}
uint16_t rigListenNumExpression(rig_t *rig, int tt)
{
	return genericExpression(rig, "LISTEN_NUM", EXP_TYPE_NUMBER,tt,RIG_NUM_LISTENS - 1);
}
uint16_t rigValueNumExpression(rig_t *rig, int tt)
{
	return genericExpression(rig, "VALUE_NUM",EXP_TYPE_NUMBER,tt,RIG_NUM_VALUES-1);
}


uint16_t rigValueExpression(rig_t *rig, int tt)
{
	return genericExpression(rig, "VALUE_NUM",EXP_TYPE_NUMBER,tt,MAX_RIG_VALUE);
}
uint16_t rigMidiChannelExpression(rig_t *rig, int tt)
{
	return genericExpression(rig, "MIDI_CHANNEL",EXP_TYPE_NUMBER,tt,MIDI_MAX_CHANNEL,MIDI_MIN_CHANNEL);
}
uint16_t rigListenChannelExpression(rig_t *rig, int tt)
{
	return genericExpression(rig, "LISTEN_CHANNEL",EXP_TYPE_NUMBER,tt,MIDI_MAX_CHANNEL);
}
uint16_t rigMidiValueExpression(rig_t *rig, int tt)
{
	return genericExpression(rig, "MIDI_VALUE",EXP_TYPE_NUMBER,tt,MIDI_MAX_VALUE);
}


uint16_t rigStringExpression(rig_t *rig, int tt)
{
	return genericExpression(rig, "STRING", EXP_TYPE_STRING,tt);
}
uint16_t rigLedColorExpression(rig_t *rig, int tt)
{
	return genericExpression(rig, "LED_COLOR", EXP_TYPE_LED_COLOR,tt);
}
uint16_t rigDisplayColorExpression(rig_t *rig, int tt)
{
	return genericExpression(rig, "DISPLAY_COLOR", EXP_TYPE_DISPLAY_COLOR,tt);
}
