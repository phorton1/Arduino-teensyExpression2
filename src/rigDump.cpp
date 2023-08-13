//-------------------------------------------------------
// rigDump.cpp
//-------------------------------------------------------

#include "rigDump.h"		// by convention
#include <myDebug.h>
#include "rigParser.h"
#include "rigExpression.h"


#define dbg_dump 0
	// 0 = basic stats
	// -1 = details
	// -2 = code details
	// -3 = opcodes

// a buffer to build output text

#define MAX_DUMP_BUF 255

int 	dump_buf_ptr = 0;
char 	dump_buf[MAX_DUMP_BUF + 1];


//------------------------------------
// methods
//------------------------------------

static bool dumpInline(const uint8_t *code, uint16_t *offset)
{
	uint8_t value;
	uint8_t op = code[(*offset)++];
	op &= ~EXP_INLINE;
	switch (op)
	{
		case EXP_NUMBER :
			value = code[(*offset)++];
			sprintf(&dump_buf[strlen(dump_buf)],"%d",value);
			break;
		case EXP_LED_COLOR :
			value = code[(*offset)++];
			sprintf(&dump_buf[strlen(dump_buf)],"%s",rigTokenToText(RIG_TOKEN_LED_BLACK + value));
			break;
		case EXP_DISPLAY_COLOR :
			value = code[(*offset)++];
			sprintf(&dump_buf[strlen(dump_buf)],"%s",rigTokenToText(RIG_TOKEN_DISPLAY_BLACK + value));
			break;
		case EXP_VALUE :
			value = code[(*offset)++];
			sprintf(&dump_buf[strlen(dump_buf)],"VALUE[%d]",value);
			break;
		case EXP_STRING :
			value = code[(*offset)++];
			sprintf(&dump_buf[strlen(dump_buf)],"STRING[%d]",value);
			break;
		default :
			rig_error("unknown inline op(%d)",op);
			return false;
			break;
	}
	return true;
}



static bool dumpOp(const uint8_t *code, uint16_t *offset)
{
	uint8_t op = code[(*offset)++];

	switch(op)
	{
		case EXP_VALUE :
			sprintf(&dump_buf[strlen(dump_buf)],"VALUE[");
			break;
		case EXP_STRING :
			sprintf(&dump_buf[strlen(dump_buf)],"STRING[");
			break;
		case EXP_TEXT :
			sprintf(&dump_buf[strlen(dump_buf)],"\"%s\"",&code[*offset]);
			*offset += strlen((const char *) &code[*offset]) + 1;
			break;
		case EXP_RIGHT_BRACKET	:
			sprintf(&dump_buf[strlen(dump_buf)],"]");
			break;
		case EXP_LEFT_PAREN	:
			sprintf(&dump_buf[strlen(dump_buf)],"(");
			break;
		case EXP_RIGHT_PAREN :
			sprintf(&dump_buf[strlen(dump_buf)],"]");
			break;
		case EXP_QUESTION :
			sprintf(&dump_buf[strlen(dump_buf)]," ? ");
			break;
		case EXP_COLON :
			sprintf(&dump_buf[strlen(dump_buf)]," : ");
			break;

		case EXP_PLUS :
		case EXP_MINUS :
		case EXP_TIMES :
		case EXP_DIVIDE :
		case EXP_EQ :
		case EXP_NOT :
		case EXP_NE :
		case EXP_GT :
		case EXP_GE :
		case EXP_LT :
		case EXP_LE :
		case EXP_BITWISE_OR :
		case EXP_BITWISE_AND :
		case EXP_LOGICAL_OR :
		case EXP_LOGICAL_AND :
			sprintf(&dump_buf[strlen(dump_buf)]," %s ",rigTokenToText(op+RIG_TOKEN_PLUS-EXP_PLUS));
			break;
		default :
			rig_error("unknown expression op(%d)",op);
			return false;
			break;
	}

	return true;
}



static bool dumpExpression(const uint8_t *code, uint16_t *offset)
{
	// exp_level++;

	display(dbg_dump + 1,"# dumpExpression at offset(%d)",*offset);

	bool ok = 1;
	while (ok && code[*offset] != EXP_END)
	{
		display(dbg_dump + 3," op at %d = 0x%02x",*offset,code[*offset]);

		if (code[*offset] & EXP_INLINE)
			ok = dumpInline(code,offset);
		else
			ok = dumpOp(code,offset);
	}

	return ok;

	// exp_level--;
    //
	// if (/*!exp_level && */ code[*offset] != EXP_END)
	// 	rig_error("Expression did not end on EXP_END");
}



static bool dumpExpressionAny(uint16_t offset)
{
	if (offset & (EXP_INLINE << 8))
	{
		uint8_t byte0 = (offset >> 8) & 0x7f;
		uint8_t byte1 = offset & 0xff;
		uint8_t code[] = {byte0, byte1, EXP_END};

		offset = 0;
		if (!dumpOp(code,&offset))
			return false;
	}
	else	// dumpExpression for real
	{
		offset -= 1;		// real expression offset are one based
		const uint8_t *code = cur_rig_code->expression_pool;
		if (!dumpExpression(code,&offset))
			return false;
	}
	return true;
}


static bool dumpParam(bool button_section, int arg_type, bool last, const uint8_t *code, uint16_t *offset)
{
	switch (arg_type)
	{
		case PARAM_VALUE_NUM 	:
		case PARAM_AREA_NUM     :
		case PARAM_MIDI_CHANNEL :
		case PARAM_MIDI_CC      :
			sprintf(&dump_buf[strlen(dump_buf)],"%d",code[(*offset)++]);
			break;
		case PARAM_MIDI_PORT    :
			sprintf(&dump_buf[strlen(dump_buf)],"%s",
				rigTokenToText(code[(*offset)++] + RIG_TOKEN_MIDI));
			break;
		case PARAM_NUM_EXPRESSION				:
		case PARAM_STRING_EXPRESSION			:
		case PARAM_LED_COLOR_EXPRESSION			:
		case PARAM_DISPLAY_COLOR_EXPRESSION		:
			uint16_t *ptr16 = (uint16_t *) &code[*offset];
			if (!dumpExpressionAny(*ptr16))
				return false;
			*offset += 2;
			break;
	}
	sprintf(&dump_buf[strlen(dump_buf)],"%s",last?");":", ");
	return true;
}


static bool dumpStatement(bool button_section, uint16_t *offset, uint16_t last_offset)
{
	dump_buf[0] = 0;

	const uint8_t *code = cur_rig_code->statement_pool;

	proc_level = button_section ? 2 : 0;
	display(dbg_dump+2,"# dumping statement offset %d",*offset);

	uint8_t tt = code[(*offset)++];

	sprintf(dump_buf,"%s(",rigTokenToText(tt));
	const statement_param_t *params = findParams(tt);

	if (params) // opposite should never happen
	{
		const int *arg = params->args;
		while (*arg)
		{
			const int *args = arg++;
			if (!dumpParam(button_section,*args,!*arg,code,offset))
			{
				proc_level = 0;
				return false;
			}
		}
	}

	display(0,dump_buf,0);
	proc_level = 0;
	return true;
}


static bool dumpStatementList(bool button_section, int statement_num)
{
	uint16_t offset = cur_rig_header->statements[statement_num];
	uint16_t last_offset  = cur_rig_header->statements[statement_num+1];

	proc_level = button_section ? 2 : 0;
	display(dbg_dump+1,"# dumping statments(%d) from %d to %d (%d bytes)",
		statement_num,
		offset,
		last_offset,
		last_offset-offset);

	while (offset < last_offset)
	{
		if (!dumpStatement(button_section,&offset,last_offset))
			return false;
	}
	return true;
}


//------------------------------------------------
// main
//-------------------------------------------------

// extern
void dumpRig()
{
	int save_proc_level = proc_level;
	proc_level = 0;

	delay(500);

	// display interesting numbers]

	display(dbg_dump,"Dumping Rig",0);
	display(dbg_dump,"string_pool_len=%d",cur_rig_header->string_pool_len);
	display(dbg_dump,"statement_pool_len=%d",cur_rig_header->statement_pool_len);
	display(dbg_dump,"expression_pool_len=%d",cur_rig_header->expression_pool_len);
	display(dbg_dump,"num_statements=%d",cur_rig_header->num_statements);
	for (int i=0; i<=cur_rig_header->num_statements; i++)
		display(dbg_dump + 1,"statement[%d] = %d",i,cur_rig_header->statements[i]);

    display_bytes(dbg_dump+2,"areas",(const uint8_t *) cur_rig_header->areas, 3 * sizeof(rigArea_t));
	display_bytes(dbg_dump+2,"statements",cur_rig_code->statement_pool,cur_rig_header->statement_pool_len);
	display_bytes_long(dbg_dump+2,0,cur_rig_code->expression_pool,cur_rig_header->expression_pool_len);
	// dump the program

	display(0,"------------------------------------------------",0);
	display(0,"",0);
	display(0,"%s",cur_rig_header->overlay_type?"Overlay":"BaseRig");
	display(0,"",0);

	bool ok = 1;

	for (int i=0; i<NUM_PEDALS; i++)
	{
		if (cur_rig_header->pedals[i].name[0])	// definition indicator - they must name em'
		{
			display(0,"PEDAL(%d, \"%s\", %s, %d, %d);",
				i,
				cur_rig_header->pedals[i].name,
				rigTokenToText(cur_rig_header->pedals[i].port + RIG_TOKEN_MIDI),
				cur_rig_header->pedals[i].channel,
				cur_rig_header->pedals[i].cc);
		}
	}
	for (int i=0; i<RIG_NUM_AREAS; i++)
	{
		if (cur_rig_header->areas[i].font_size)	// definition indicator
		{
			display(0,"AREA(%d, %d, %s, %s, %d, %d, %d, %d);",
				i,
				cur_rig_header->areas[i].font_size,
				rigTokenToText(cur_rig_header->areas[i].font_type + RIG_TOKEN_NORMAL),
				rigTokenToText(cur_rig_header->areas[i].font_just + RIG_TOKEN_LEFT),
				cur_rig_header->areas[i].xs,
				cur_rig_header->areas[i].ys,
				cur_rig_header->areas[i].xe,
				cur_rig_header->areas[i].ye);
		}
	}
	for (int i=0; i<RIG_NUM_LISTENS; i++)
	{
		if (cur_rig_header->listens[i].active)	// definition indicator
		{
			display(0,"LISTEN(%d, %s, %d, %d);",
				i,
				rigTokenToText(cur_rig_header->listens[i].port ? RIG_TOKEN_SERIAL : RIG_TOKEN_MIDI),
				cur_rig_header->listens[i].channel,
				cur_rig_header->listens[i].cc);
		}
	}

	for (int i=0; i<RIG_NUM_STRINGS; i++)
	{
		// the offset is incremented so that we can identify
		// accesses to string 0 explicitly.
		uint16_t string_offset = cur_rig_header->strings[i];

		if (string_offset--)	// test and adjust offset
		{
			display(0,"STRING(%d, \"%s\");",
				i,
				&cur_rig_code->string_pool[string_offset]);
		}
	}

	// dump init_section statement list

	ok = ok && dumpStatementList(0,0);
	if (ok)
		display(0,"",0);

	// to dump buttons we check if any off it's sections exist
	// there is no single indicator for whether a button is defined

	for (int i=0; i<NUM_BUTTONS && ok; i++)
	{
		bool started = 0;
		for (int j=0; j<NUM_SUBSECTIONS && ok; j++)
		{
			if (cur_rig_header->button_refs[i][j])
			{
				if (!started)
				{
					started = 1;
					display(0,"BUTTON(%d):",i);
				}

				display(0,"    %s:", rigTokenToText(j + RIG_TOKEN_COLOR));

				if (j >= SUBSECTION_FIRST_CODE)	// statement lists
				{
					// statement list num is 1 based
					// so that we can identify a used statement list
					ok = dumpStatementList(1,cur_rig_header->button_refs[i][j] - 1);
				}
				else
				{
					// on the other hand, we only decrement the exprssion
					// offset if the high order is NOT set

					uint16_t offset = cur_rig_header->button_refs[i][j];
					dump_buf[0] = 0;
					ok = dumpExpressionAny(offset);
					proc_level = 2;
					display(0,"%s;",dump_buf);
					proc_level = 0;
				}
			}
		}
	}

	if (ok)
	{
		display(0,"",0);
		display(0,"# end of rig ",0);
		display(0,"",0);
	}

	proc_level = save_proc_level;

}
