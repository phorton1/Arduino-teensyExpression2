//-------------------------------------------------------
// rigEval.cpp
//-------------------------------------------------------
// The rigMachine expression evaluator.
//
// Errors in evaluation are problematic to report, especially
// if they occur in the button led code which happens 30 times
// a second and never stops.
//
// Therefore, the rig_error method itself knows if we are
// parsing, evaulating a statement, or evaluating a
// color/blink section, and can appropriately try
// to report things if it can.

#include <myDebug.h>
#include "rigMachine.h"
#include "rigExpression.h"
#include "winDialogs.h"


static int debug_eval_offset = 0x00bd;	// 208;


static int dbg_ops		= 1;
static int dbg_stack   = 1;
static int dbg_eval 	= 1;
	// 0 = just the final return value
	// -1 = all return values
	// -2 = all mainline functionality
	// -3 = everything

#define MAX_OP_STACK	32
#define MAX_VAL_STACK   32

#define IS_EXP_OP(op)	(op >= EXP_NOT && op <= EXP_LOGICAL_AND)
#define OP_TOKEN(op)	(rigTokenToString(op + RIG_TOKEN_LEFT_PAREN - EXP_LEFT_PAREN))


int op_top;
int val_top;
// int cond_top;

uint8_t 	 op_stack[MAX_OP_STACK];
evalResult_t val_stack[MAX_VAL_STACK];


//-----------------------------------------------
// rig_error() and VAL_DISPLAY()
//-----------------------------------------------

#define ERROR_COLOR_STRING      "\033[91m"       // red

// extern
void rig_error(int type, uint16_t offset, const char *format, ...)
{
	char error_buffer[255];
	sprintf(error_buffer,"%d:%d ",type,offset);
	char *text = &error_buffer[strlen(error_buffer)];

	va_list var;
	va_start(var, format);
	vsprintf(text,format,var);

	if (dbgSerial)
	{
		dbgSerial->print(ERROR_COLOR_STRING);
		dbgSerial->print("RIG_ERROR - ");
		dbgSerial->println(error_buffer);
	}

	error_dlg.setMessage("Rig Error",error_buffer);
	the_system.startWindow(&error_dlg);

}


const char *VAL_DISPLAY(evalResult_t *rslt)
{
	static char local_buf[2][128];
	static int which = 0;
	which = which ? 0 : 1;
	if (!rslt->is_string)
		sprintf(local_buf[which],"%d",rslt->value);
	else
		sprintf(local_buf[which],"\"%s\"",rslt->text);
	return local_buf[which];
}



//-----------------------------------------------
// utilities
//-----------------------------------------------


static void dumpOpStack()
{
	if (dbg_eval > -3)
		return;
	if (!op_top)
		Serial.print("{empty}");
	for (int i=0; i<op_top; i++)
	{
		Serial.print("  ");
		Serial.print(OP_TOKEN(op_stack[i]));
	}
	Serial.println();
}



static bool pushOp(uint8_t op)
{
	display(dbg_stack,"pushOp(%d) 0x%02x=%s",op_top,op,OP_TOKEN(op));
	if (op_top >= MAX_OP_STACK)
	{
		rig_error(0,0,"OP_STACK OVERFLOW");
		return false;
	}
	op_stack[op_top++] = op;
	dumpOpStack();
	return true;
}

static bool popOp(uint8_t *op)
{
	if (!op_top)
	{
		rig_error(0,0,"OP_STACK UNDERFLOW");
		return false;
	}
	*op = op_stack[--op_top];
	display(dbg_stack,"popOp(%d) 0x%02x=%s",op_top,*op,OP_TOKEN(*op));
	dumpOpStack();
	return true;
}

static bool pushVal(evalResult_t *val)
{
	display(dbg_stack,"pushVal(%d) %s",val_top,VAL_DISPLAY(val));
	if (val_top >= MAX_VAL_STACK)
	{
		rig_error(0,0,"VAL_STACK OVERFLOW");
		return false;
	}
	memcpy(&val_stack[val_top++],val,sizeof(evalResult_t));
	return true;
}

static bool pushValInt(uint16_t value)
{
	display(dbg_stack,"pushValInt(%d) %d",val_top,value);
	if (val_top >= MAX_VAL_STACK)
	{
		rig_error(0,0,"VAL_STACK(TEXT) OVERFLOW");
		return false;
	}
	val_stack[val_top].is_string = 0;
	val_stack[val_top].value = value;
	val_top++;
	return true;
}

static bool pushValText(const char *text)
{
	display(dbg_stack,"pushValText(%d) \"%s\"",val_top,text);
	if (val_top >= MAX_VAL_STACK)
	{
		rig_error(0,0,"VAL_STACK(TEXT) OVERFLOW");
		return false;
	}
	val_stack[val_top].is_string = 1;
	val_stack[val_top].text = text;
	val_top++;
	return true;
}


static bool popVal(evalResult_t *val)
{
	if (!val_top)
	{
		rig_error(0,0,"VAL_STACK UNDERFLOW");
		return false;
	}
	memcpy(val,&val_stack[--val_top],sizeof(evalResult_t));
	display(dbg_stack,"popVal(%d) %s",val_top,VAL_DISPLAY(&val_stack[val_top]));
	return true;
}


static const int precedence(uint8_t op)
	// Mine are in reverse order from C++, which places them like this:
	//  4   !					logical and binary NOT	right-to-left
	//	5 	a*b   a/b   a%b 	Multiplication, division, and remainder
	//	6 	a+b   a-b 			Addition and subtraction
	//	7 	<<   >> 			Bitwise left shift and right shift
	//	8 	<=> 				Three-way comparison operator (since C++20)
	//	9 	<   <=   >   >= 	For relational operators < and <= and > and >= respectively
	//	10 	==   != 			For equality operators = and != respectively
	//	11 	a&b 				Bitwise AND
	//	12 	^ 					Bitwise XOR (exclusive or)
	//	13 	| 					Bitwise OR (inclusive or)
	//	14 	&& 					Logical AND
	//	15 	|| 					Logical OR
	// Added by Me
	//  16  ? :					Ternary IF and COLON operators
	//  17  )					RIGHT_PAREN
	//  19  ( 					LEFT_PAREN
{
	// display(0,"precedence(0x%02x)",op);
	switch(op)
	{
		case EXP_NOT :
			return 1;
		case EXP_TIMES :
		case EXP_DIVIDE :
			return 2;
		case EXP_PLUS :
		case EXP_MINUS :
			return 3;
		case EXP_GT :
		case EXP_GE :
		case EXP_LT :
		case EXP_LE :
			return 4;
		case EXP_EQ :
		case EXP_NE :
			return 5;
		case EXP_BITWISE_AND :
			return 6;
		case EXP_BITWISE_OR :
			return 7;
		case EXP_LOGICAL_AND :
			return 8;
		case EXP_LOGICAL_OR :
			return 9;
		//case EXP_QUESTION :
		//case EXP_COLON :
		//	return 10;
		//case OP_CONDITION :
		//	return 11;
		//case EXP_RIGHT_PAREN :
		//	return 12;
		case EXP_LEFT_PAREN :
			return 99;
	}
	rig_error(0,0,"unknown op(0x%02x) in precedence",op);
	return 0;
}


//----------------------------------------------------------------------------------
// doOp()
//----------------------------------------------------------------------------------

static bool doOp(int op_start, uint8_t op)
{
	display(dbg_eval+2,"doOp(0x%02x=%s)",op,OP_TOKEN(op));
	proc_entry();

	bool ok = 1;
	evalResult_t val1;
	evalResult_t val2;

	if (ok && op == EXP_NOT)
	{
		ok = popVal(&val2);
		val1.is_string = 0;
		display(dbg_ops,"NOT %d",val2.value);
		val1.value = !val2.value;
	}
	else
	{
		ok = popVal(&val2);
		ok = ok && popVal(&val1);
		if (ok)
		{
			display(dbg_ops,"%s %s %s",VAL_DISPLAY(&val1),OP_TOKEN(op),VAL_DISPLAY(&val2));
			switch (op)
			{
				case EXP_PLUS :
					val1.value += val2.value;
					break;
				case EXP_MINUS :
					val1.value -= val2.value;
					break;
				case EXP_TIMES :
					val1.value *= val2.value;
					break;
				case EXP_DIVIDE :
					if (val2.value == 0)
					{
						rig_error(0,0,"rigEval - Divide by zero!!");
						ok = 0;
					}
					val1.value /= val2.value;
					break;
				case EXP_EQ :
					val1.value = val1.value == val2.value;
					break;
				case EXP_NE :
					val1.value = val1.value != val2.value;
					break;
				case EXP_GT :
					val1.value = val1.value > val2.value;
					break;
				case EXP_GE :
					val1.value = val1.value >= val2.value;
					break;
				case EXP_LT :
					val1.value = val1.value < val2.value;
					break;
				case EXP_LE :
					val1.value = val1.value <= val2.value;
					break;
				case EXP_BITWISE_OR :
					val1.value |= val2.value;
					break;
				case EXP_BITWISE_AND :
					val1.value &= val2.value;
					break;
				case EXP_LOGICAL_OR :
					val1.value = val1.value || val2.value;
					break;
				case EXP_LOGICAL_AND :
					val1.value = val1.value && val2.value;
					break;
			}	// switch
		}	// ok
	}	// bin_ops


	ok = ok && pushVal(&val1);
	proc_leave();
	return ok;
}



//---------------------------------------------
// getAtom()
//---------------------------------------------

bool rigMachine::getAtom(const rig_t *rig, const uint8_t *code, uint16_t *offset)
	// it's either an inline number, or a
{

	display(dbg_eval + 2,"getAtom(0x%02x) at %d  val_start=%d",code[*offset],*offset,val_top);
	proc_entry();

	int val_start = val_top;
	uint8_t byte = code[(*offset)++];
	uint8_t inline_op = byte & 0x3f;

	bool ok = 1;
	if (byte == EXP_TEXT)
	{
		const char *s = (const char *) &code[*offset];
		display(dbg_eval + 2,"TEXT = %s",s);
		ok = pushValText(s);
		(*offset) += strlen(s) + 1;
	}
	else if (byte == EXP_STRING)
	{
		display(dbg_eval + 2,"STRING[sub_exp]",0);
		ok = evaluate(rig, code, offset);				// RECURSE
		if (ok)
		{
			if (code[*offset] == EXP_RIGHT_BRACKET)
			{
				display(dbg_eval+2,"STRING[sub_exp] skipping RIGHT_BRACKET",0);
				(*offset)++;
			}
			uint8_t num = val_stack[--val_top].value;
			if (num > RIG_NUM_STRINGS-1)
			{
				rig_error(0,*offset,"rigEval - STRING INDEX(%d) out of range (0..%d)",num,RIG_NUM_STRINGS-1);
				ok = 0;
			}
			else
			{
				uint16_t off = rig->strings[num];  // one based offset
				if (!off)
				{
					warning(0,"STRING[%d] is not defined; returning empty string",num);
					ok = pushValText("");
				}
				else
				{
					off--;
					const char *s = &rig->string_pool[off];
					display(dbg_eval + 2,"STRING[%d] off(%d) = %s",num,off,s);
					ok = pushValText(s);
				}
			}
		}
	}
	else if (byte == EXP_VALUE)
	{
		display(dbg_eval + 2,"VALUE[sub_exp]",0);
		ok = evaluate(rig, code, offset);				// RECURSE
		if (ok)
		{
			if (code[*offset] == EXP_RIGHT_BRACKET)
			{
				display(dbg_eval+2,"VALUE[sub_exp] skipping RIGHT_BRACKET",0);
				(*offset)++;
			}
			uint8_t num = val_stack[--val_top].value;
			if (num > RIG_NUM_VALUES-1)
			{
				rig_error(0,*offset,"rigEval - VALUE INDEX(%d) out of range (0..%d)",num,RIG_NUM_VALUES-1);
				ok = 0;
			}
			else
			{
				uint8_t value = m_rig_state.values[num];
				display(dbg_eval + 2,"VALUE[num=%d] value=%d",num,value);
				ok = pushValInt(value);
			}
		}
	}

	// otherwise everything *should* be an inline

	else if (!(byte & EXP_INLINE))
	{
		rig_error(0,*offset-1,"rigEval - INLINE op expected, not (0x%02x)",byte);
		ok = 0;
	}
	else
	{
		uint8_t value = code[(*offset)++];

		if (inline_op == EXP_LED_COLOR)
		{
			display(dbg_eval + 2,"INLINE LED_COLOR(%d) = %s",value,rigTokenToText(RIG_TOKEN_LED_BLACK + value));
			ok = pushValInt(value);
		}
		else if (inline_op == EXP_DISPLAY_COLOR)
		{
			display(dbg_eval + 2,"INLINE DISPLAY_COLOR(%d) = %s",value,rigTokenToText(RIG_TOKEN_DISPLAY_BLACK + value));
			ok = pushValInt(value);
		}
		else if (inline_op == EXP_TEXT)
		{
			if (!value)
				ok = pushValText("");
			else
			{
				rig_error(0,*offset-2,"rigEval - Unexpected NON-NULL inline EXP_TEXT op!!");
				ok = 0;
			}
		}
		else if (inline_op == EXP_STRING)
		{
			// string_offsets are one based in the rig header

			uint16_t off = rig->strings[value];
			const char *s = &rig->string_pool[off-1];
			display(dbg_eval + 2,"INLINE_STRING[%d] at %d = %s",value,off,s);
			ok = pushValText(s);
		}
		else if (inline_op == EXP_VALUE)
		{
			display(dbg_eval + 2,"INLINE VALUE[%d]",value);
			value = m_rig_state.values[value];
			ok = pushValInt(value);
		}
		else if (inline_op == EXP_NUMBER)
		{
			display(dbg_eval + 2,"INLINE NUMBER(%d)",value);
			ok = pushValInt(value);
		}
		else
		{
			rig_error(0,*offset-2,"rigEval - Unexpected inline_op(0x%02x) value=0x%02x",inline_op,value);
			ok = 0;
		}
	}

	proc_leave();
	if (ok)
		display(dbg_eval + 1,"getAtom() returning(%s) at %d   val_top=%d   op_top=%d",
				VAL_DISPLAY(&val_stack[val_start]),*offset,val_top,op_top);
	return ok;
}



//============================================================
// evaluate()
//============================================================



bool rigMachine::evaluate(const rig_t *rig, const uint8_t *code, uint16_t *offset)
{
	int op_start = op_top;
	int val_start = val_top;

	display(dbg_eval + 2,"evaluate(0x%02x) at %d  op_start=%d   val_start=%d",code[*offset],*offset,op_start,val_start);
	proc_entry();

	bool ok = 1;
	bool done = 0;
	while (ok && !done &&
		   code[*offset] != EXP_END &&
		   code[*offset] != EXP_COLON &&
		   code[*offset] != EXP_RIGHT_BRACKET)
	{
		if (code[*offset] == EXP_QUESTION)
		{
			(*offset)++;
			display(dbg_eval+2,"EXP_QUESTION at %d ",*offset-1);
			proc_entry();

			while (ok &&
				   op_top > op_start &&
				   op_stack[op_top-1] != EXP_LEFT_PAREN)
			{
				uint8_t op;
				ok = popOp(&op);
				ok = ok && doOp(op_start,op);
			}

			// save the result and recurse for the two sub expressions

			evalResult_t cond;
			evalResult_t val1;
			evalResult_t val2;

			ok = ok && popVal(&cond);

			if (ok)
			{
				display(dbg_eval+2,"? exp",0);
				ok = evaluate(rig,code,offset);
				ok = ok && popVal(&val1);
				if (ok) display(dbg_eval+2,"? exp part = %s",VAL_DISPLAY(&val1));
			}
			if (ok)
			{
				display(dbg_eval+2,": exp",0);
				if (code[*offset] == EXP_COLON)
				{
					display(dbg_eval+2,": exp part skipping EXP_COLON",0);
					(*offset)++;
				}
				ok = evaluate(rig,code,offset);
				ok = ok && popVal(&val2);
				if (ok) display(dbg_eval+2,": exp part = %s",VAL_DISPLAY(&val2));
			}
			if (ok)
			{
				display(dbg_eval+2,"%d ? %s : %s",cond.value,VAL_DISPLAY(&val1),VAL_DISPLAY(&val2));
				ok = pushVal(cond.value ? &val1 : &val2);
			}

			proc_leave();
		}
		else if (code[*offset] == EXP_LEFT_PAREN)
 		{
			(*offset)++;
 			display(dbg_eval + 2,"LEFT_PAREN at %d",*offset-1);
 			ok = ok && pushOp(EXP_LEFT_PAREN);
 		}
		else if (code[*offset] == EXP_RIGHT_PAREN)
		{
			(*offset)++;
 			display(dbg_eval + 2,"RIGHT_PAREN at %d",*offset-1);
			proc_entry();

			uint8_t op;
			ok = popOp(&op);
			while (op != EXP_LEFT_PAREN)
			{
				ok = ok && doOp(op_start,op);
				ok = popOp(&op);
			}

			proc_leave();
		}

		else if (code[*offset] == EXP_NOT)
		{
			(*offset)++;
 			ok = pushOp(EXP_NOT);
		}
		else if (IS_EXP_OP(code[*offset]))
		{
			display(dbg_eval+2,"IS_EX_OP(0x%02x=%s) at %d  op_top=%d op_start=%d",code[*offset],OP_TOKEN(code[*offset]),*offset,op_top,op_start);
			proc_entry();

			uint8_t byte = code[(*offset)++];
			while (ok && op_top > op_start &&
				   precedence(op_stack[op_top-1]) < precedence(byte))
			{
				uint8_t op;
				ok = popOp(&op);
				ok = ok && doOp(op_start,op);
			}

			ok = ok && pushOp(byte);
			proc_leave();
		}

		// otherwise, it's an atom with a possible nested expression

		else
		{
			ok = getAtom(rig,code,offset);	// pushes it on the stack
			// byte = code[(*offset)++];
		}
	}

	if (ok && op_top > op_start)
	{
		display(dbg_eval+2,"LEFTOVER_OPS top=%d start=%d",op_top,op_start);
		proc_entry();
		while (ok && op_top > op_start)
		{
			uint8_t op;
			ok = popOp(&op);
			ok = ok && doOp(op_start,op);
		}
		proc_leave();
	}

	proc_leave();
	if (ok)
		display(dbg_eval+1,"evaluate() returning %s at %d   op_top=%d  val_top=%d",VAL_DISPLAY(&val_stack[val_start]),*offset,op_top,val_top);
	return ok;
}




//-------------------------
// entry point
//-------------------------

bool rigMachine::expression(const rig_t *rig, evalResult_t *rslt, const uint8_t *code, uint16_t *offset)
{
	// debugging of certain expressions

	bool debug_this = 0;
	int save_ops = dbg_ops;
	int save_stack = dbg_stack;
	int save_eval = dbg_eval;

	if (*offset == debug_eval_offset)
	{
		debug_this 	= 1;
		dbg_ops		= 0;
		dbg_stack   = 0;
		dbg_eval 	= -3;
	}

	display(dbg_eval,"expression at offset %d",*offset);

	op_top = 0;
	val_top = 0;

	bool ok = evaluate(rig,code,offset);
	if (ok)
		memcpy(rslt,&val_stack[0],sizeof(evalResult_t));
	if (ok)
		display(dbg_eval,"expression() returning %s at %d",VAL_DISPLAY(rslt),*offset);

	if (debug_this)
	{
		dbg_ops	  = save_ops;
		dbg_stack = save_stack;
		dbg_eval  = save_eval;
	}

	return ok;
}
