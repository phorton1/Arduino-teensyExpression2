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

static int exp_len;
static int exp_level;

static bool addExpByte(uint8_t byte)
{
	if (rig_header.expression_pool_len >= MAX_EXPRESSION_POOL)
	{
		rig_error("EXPRESSION POOL OVERFLOW");
		return false;
	}

	#if SHOW_OUTPUT_BYTES
		dbgSerial->print("{");
		dbgSerial->print(byte,HEX);
		dbgSerial->print("}");
	#endif

	rig_code.expression_pool[rig_header.expression_pool_len++] = byte;
	return true;
}


static void init_exp()
{
	exp_len = 0;
	exp_level = 0;
}


//-------------------------------------------
// expression
//-------------------------------------------

static int factor(int tt);
static int term(int tt);
static int exp(int tt);

static int atom(int tt)
{
	display(dbg_exp + 2,"atom(%s)",rigTokenToString(tt));
	proc_entry();

	bool ok = 1;
	int type = EXP_TYPE_ILLEGAL;

	if (IS_LED_COLOR(tt))
	{
		display(dbg_exp + 1,"LED_COLOR = %s",rigTokenToString(tt));
		type = EXP_TYPE_LED_COLOR;
		uint8_t byte = ((uint8_t) EXP_INLINE) | ((uint8_t) EXP_TYPE_LED_COLOR);
		ok = addExpByte(byte);
		ok = ok && addExpByte(LED_COLOR(tt));
		ok = ok && getRigToken();
	}
	else if (IS_DISPLAY_COLOR(tt))
	{
		display(dbg_exp + 1,"DISPLAY_COLOR = %s",rigTokenToString(tt));
		type = EXP_TYPE_DISPLAY_COLOR;
		uint8_t byte = ((uint8_t) EXP_INLINE) | ((uint8_t) EXP_TYPE_DISPLAY_COLOR);
		ok = addExpByte(byte);
		ok = ok && addExpByte(DISPLAY_COLOR(tt));
		ok = ok && getRigToken();
	}
	else if (tt == RIG_TOKEN_NUMBER)
	{
		display(dbg_exp + 1,"NUMBER = %d",rig_token.int_value);
		type = EXP_TYPE_NUMBER;
		if (rig_token.int_value > 255)
		{
			rig_error("EXPRESSION NUMBERS must be in range of 0..255");
			ok = 0;;
		}
		uint8_t num = rig_token.int_value;
		uint8_t byte = ((uint8_t) EXP_INLINE) | ((uint8_t) EXP_NUMBER);
		ok = ok && addExpByte(byte);
		ok = ok && addExpByte(num);
		ok = ok && getRigToken();
	}
	else if (tt == RIG_TOKEN_TEXT)
	{
		display(dbg_exp + 1,"TEXT = %s",rig_token.text);
		type = EXP_TYPE_STRING;
		ok = ok && addExpByte(EXP_TEXT);
		for (uint16_t i=0; ok && i<strlen(rig_token.text); i++)
		{
			ok = addExpByte(rig_token.text[i]);
		}
		ok = ok && addExpByte(0);	// NULL Terminator
		ok = ok && getRigToken();
	}
	else if (
		tt == RIG_TOKEN_VALUE ||
		tt == RIG_TOKEN_STRING)
	{
		int what = tt;
		int OP_TYPE = tt == RIG_TOKEN_VALUE ? EXP_VALUE : EXP_STRING;
		type = tt == RIG_TOKEN_VALUE ? EXP_TYPE_NUMBER : EXP_TYPE_STRING;

		tt = getRigToken();
		if (tt != RIG_TOKEN_LEFT_BRACKET)
		{
			ok = false;
			rig_error("LEFT_BRACKET expected");
		}
		tt = getRigToken();
		ok = ok && tt;

		bool exp_inline = 0;
		if (ok)
		{
			display(dbg_exp + 1,"%s[exp]",rigTokenToString(what));
			ok = addExpByte(OP_TYPE);
			uint16_t where = rig_header.expression_pool_len;
			int sub_type = exp(tt);
			if (sub_type != EXP_TYPE_NUMBER)
			{
				rig_error("INDEXES must be a numeric expression");
				ok = 0;
			}

			// demote solo following inline number to this opcode

			else if (rig_code.expression_pool[where] == (EXP_NUMBER | EXP_INLINE))
			{
				int len = rig_header.expression_pool_len - where;
				if (len == 2)
				{
					exp_inline = 1;
					rig_code.expression_pool[where-1] |= EXP_INLINE;
					rig_code.expression_pool[where] = rig_code.expression_pool[where+1];
					rig_header.expression_pool_len = where + 1;
				}
			}
			tt = rig_token.id;
			ok = ok && tt;
		}
		if (ok)
		{
			if (tt != RIG_TOKEN_RIGHT_BRACKET)
			{
				ok = false;
				rig_error("RIGHT_BRACKET expected");
			}
			if (!exp_inline)
				ok = ok && addExpByte(EXP_RIGHT_BRACKET);
			ok = ok && getRigToken();
		}
	}	// VALUE[] or STRING[]

	if (!ok)
		type = EXP_TYPE_ILLEGAL;

	proc_leave();
	display(dbg_exp + 2,"atom() returning %d on %s",type,rigTokenToString(rig_token.id));
	return type;
}



static int factor(int tt)
{
	display(dbg_exp + 2,"factor(%s)",rigTokenToString(tt));
	proc_entry();

	bool ok = 1;
	int type = EXP_TYPE_ILLEGAL;

	if (tt == RIG_TOKEN_NOT)
	{
		display(dbg_exp + 1,"NOT exp",0);
		ok = addExpByte(EXP_LEFT_PAREN);
		tt = getRigToken();
		ok = ok && tt;
		if (ok)
			type = factor(tt);
		if (type != EXP_TYPE_NUMBER)
		{
			rig_error("NOT(!) may only be used on NUMERIC expressions");
			ok = false;
		}
	}
	else if (tt == RIG_TOKEN_LEFT_PAREN)
	{
		display(dbg_exp + 1,"( exp )",0);
		ok = addExpByte(EXP_LEFT_PAREN);
		tt = getRigToken();
		ok = ok && tt;
		if (ok)
			type = exp(tt);
		ok = ok && type != EXP_TYPE_ILLEGAL;
		if (ok && rig_token.id != RIG_TOKEN_RIGHT_PAREN)
		{
			ok = addExpByte(EXP_RIGHT_PAREN);
			tt = getRigToken();
			ok = ok && tt;
		}
	}
	else if (
		tt == RIG_TOKEN_NUMBER ||
		tt == RIG_TOKEN_TEXT ||
		tt == RIG_TOKEN_VALUE ||
		tt == RIG_TOKEN_STRING ||
		IS_LED_COLOR(tt) ||
		IS_DISPLAY_COLOR(tt))
	{
		type = atom(tt);
	}

	if (!ok)
		type = EXP_TYPE_ILLEGAL;

	proc_leave();
	display(dbg_exp + 2,"factor() returning %d on %s",type,rigTokenToString(rig_token.id));
	return type;
}


static int term(int tt)
{
	display(dbg_exp + 2,"term(%s)",rigTokenToString(tt));
	proc_entry();

	bool ok = 1;
	int type = factor(tt);
	tt = rig_token.id;

	if (type == EXP_TYPE_NUMBER)
	{
		while (ok && IS_BIN_OP(tt))
		{
			display(dbg_exp + 1,"%s factor",rigTokenToString(tt));
			ok = addExpByte(EXP_PLUS + BIN_OP(tt));
			tt = getRigToken();
			ok = ok && tt;
			if (ok)
			{
				int sub_type = factor(tt);
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



static int exp(int tt)
{
	display(dbg_exp + 1 + (exp_level ? 1 : 0),"exp(%s)",rigTokenToString(tt));
	proc_entry();
	exp_level++;

	bool ok = 1;
	int type = term(tt);
	if (type == EXP_TYPE_NUMBER)
	{
		tt = rig_token.id;
		if (tt == RIG_TOKEN_QUESTION)
		{
			display(dbg_exp + 1,"? exp",rigTokenToString(tt));
			ok = addExpByte(EXP_QUESTION);
			tt = getRigToken();
			ok = ok && tt;

			type = exp(tt);
			if (type != EXP_TYPE_ILLEGAL)
			{
				tt = rig_token.id;
				if (tt != RIG_TOKEN_COLON)
				{
					rig_error("COLON expected");
					ok = 0;
				}
				display(dbg_exp + 1,": exp",rigTokenToString(tt));
				ok = addExpByte(EXP_COLON);
				tt = getRigToken();
				ok = ok && tt;

				if (ok)
				{
					int type2 = exp(tt);
					if (type2 != type)
					{
						rig_error("Conditional terminals must evaluate to same type");
						ok = 0;
					}
				}
			}
		}
	}

	exp_level--;
	if (ok && type != EXP_TYPE_ILLEGAL && !exp_level)
		ok = addExpByte(EXP_END);
	if (!ok)
		type = EXP_TYPE_ILLEGAL;

	proc_leave();
	display(dbg_exp + 1 + (exp_level ? 1 : 0),"exp() returning %d on %s",type,rigTokenToString(rig_token.id));
	return type;
}


//-------------------------------------------
// specific expression types
//-------------------------------------------

uint16_t inlineExp(uint16_t offset)
{
	if (rig_header.expression_pool_len - offset == 2)
	{
		uint8_t byte0 = rig_code.expression_pool[offset];
		uint8_t byte1 = rig_code.expression_pool[offset + 1];
		byte0 |= EXP_INLINE;
		display(dbg_exp + 1,"returning INLINE expression(0x%02x,0x%02x)",byte0,byte1);
		rig_header.expression_pool_len = offset;	// roll back the ppol
		offset = byte0 << 8 | byte1;
	}
	else
	{
		offset += 1;	// make real offsets one based
	}
	return offset;
}


// extern
uint16_t rigNumericExpression(int tt)
{
	uint16_t offset = rig_header.expression_pool_len;
	display(dbg_exp + 1,"NUM_EXPRESSION",0);
	int save_proc_level = proc_level;
	proc_level = 0;

	init_exp();
	int type = exp(tt);
	if (type != EXP_TYPE_NUMBER)
	{
		rig_error("NUMERIC expression expected");
		proc_level = save_proc_level;
		return false;
	}

	proc_level = save_proc_level;
	offset = inlineExp(offset);
	display(dbg_exp,"NUM_EXPRESSION() returning 0x%04x",offset);
	return offset;
}


// extern
uint16_t rigStringExpression(int tt)
{
	uint16_t offset = rig_header.expression_pool_len;
	display(dbg_exp + 1,"STRING_EXPRESSION",0);
	int save_proc_level = proc_level;
	proc_level = 0;

	init_exp();
	int type = exp(tt);
	if (type != EXP_TYPE_STRING)
	{
		rig_error("STRING expression expected");
		proc_level = save_proc_level;
		return false;
	}


	proc_level = save_proc_level;
	offset = inlineExp(offset);
	display(dbg_exp,"STRING_EXPRESSION() returning 0x%04x",offset);
	return offset;
}


// extern
uint16_t rigLedColorExpression(int tt)
{
	uint16_t offset = rig_header.expression_pool_len;
	display(dbg_exp + 1,"LED_EXPRESSION",0);
	int save_proc_level = proc_level;
	proc_level = 0;

	init_exp();
	int type = exp(tt);
	if (type != EXP_TYPE_LED_COLOR)
	{
		rig_error("DISPLAY_COLOR expression expected");
		proc_level = save_proc_level;
		return false;
	}

	proc_level = save_proc_level;
	offset = inlineExp(offset);
	display(dbg_exp,"LED_EXPRESSION() returning 0x%04x",offset);
	return offset;
}


// extern
uint16_t rigDisplayColorExpression(int tt)
{
	uint16_t offset = rig_header.expression_pool_len;
	display(dbg_exp + 1,"COLOR_EXPRESSION",0);
	int save_proc_level = proc_level;
	proc_level = 0;

	init_exp();
	int type = exp(tt);
	if (type != EXP_TYPE_DISPLAY_COLOR)
	{
		rig_error("DISPLAY_COLOR expression expected");
		proc_level = save_proc_level;
		return false;
	}

	proc_level = save_proc_level;
	offset = inlineExp(offset);
	display(dbg_exp,"COLOR_EXPRESSION() returning 0x%04x",offset);
	return offset;
}
