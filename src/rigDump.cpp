//-------------------------------------------------------
// rigDump.cpp
//-------------------------------------------------------

#include "rigDump.h"		// by convention
#include <myDebug.h>
#include "rigParser.h"
#include "rigExpression.h"


#define dbg_code 1
	// show the statements and exptessions code
#define dbg_dump 1
	// 1 = basic state
	// 0 = dump
	// -1 = details
	// -2 = expressions
	// -3 = opcodes


// a buffer to build output text

#define MAX_DUMP_BUF 255

int 	dump_buf_ptr = 0;
char 	dump_buf[MAX_DUMP_BUF + 1];


//------------------------------------
// methods
//------------------------------------


static bool dumpNumber(uint8_t value, bool is_id)
	// dumps a number or an identifier if is_id
{
	if (!is_id)
	{
		sprintf(&dump_buf[strlen(dump_buf)],"%d",value);
	}
	// if it's an identifier, value is it's index in the defines
	// which is, in turn, a 1 based offset into the define_pool
	else
	{
		uint16_t def_offset = rig_header.define_ids[value] - 1;
		sprintf(&dump_buf[strlen(dump_buf)],"%s",&rig_code.define_pool[def_offset]);
	}
	return true;
}


static bool dumpInline(const uint8_t *code, uint16_t *offset)
{

	int dbg_offset = *offset;
	uint8_t op = code[(*offset)++];
	uint8_t value = code[(*offset)++];
	bool is_id = op & EXP_INLINE_ID;
	uint8_t opcode = op & ~(EXP_INLINE | EXP_INLINE_ID);

	display(dbg_dump+3,"# op at %d=INLINE(0x%02x) value=%d  %s",
		dbg_offset,
		op,
		value,
		is_id ? "IS_ID" : "");

	switch (opcode)
	{
		case EXP_DEFINE :
		case EXP_NUMBER :
			dumpNumber(value,is_id);
			break;
		case EXP_LED_COLOR :
			sprintf(&dump_buf[strlen(dump_buf)],"%s",rigTokenToText(RIG_TOKEN_LED_BLACK + value));
			break;
		case EXP_DISPLAY_COLOR :
			sprintf(&dump_buf[strlen(dump_buf)],"%s",rigTokenToText(RIG_TOKEN_DISPLAY_BLACK + value));
			break;
		case EXP_VALUE :
			sprintf(&dump_buf[strlen(dump_buf)],"VALUE[");
			dumpNumber(value,is_id);
			sprintf(&dump_buf[strlen(dump_buf)],"]");
			break;
		case EXP_STRING :
			sprintf(&dump_buf[strlen(dump_buf)],"STRING[");
			dumpNumber(value,is_id);
			sprintf(&dump_buf[strlen(dump_buf)],"]");
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
	display(dbg_dump+3,"# op at %d=0x%02x",
		*offset,
		code[*offset]);

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

		case EXP_NOT :
			sprintf(&dump_buf[strlen(dump_buf)]," %s ",rigTokenToText(RIG_TOKEN_NOT));
			break;

		case EXP_PLUS :
		case EXP_MINUS :
		case EXP_TIMES :
		case EXP_DIVIDE :
		case EXP_EQ :
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



static bool dumpExpression(const char *what, const uint8_t *code, uint16_t *offset)
{
	display(dbg_dump+2,"# dumpExpression(%s) at offset(%d)",what,*offset);

	bool ok = 1;
	while (ok &&  code[*offset] != EXP_END)
	{
		if (code[*offset] & EXP_INLINE)
			ok = dumpInline(code,offset);
		else
			ok = dumpOp(code,offset);

	}

	return ok;
}



static bool dumpCodeExpression(const char *what, uint16_t offset)
{
	if (offset & (EXP_INLINE << 8))
	{
		uint8_t byte0 = offset >> 8;
		uint8_t byte1 = offset & 0xff;
		uint8_t code[] = {byte0, byte1, EXP_END};

		display(dbg_dump+2,"# dumpInlineExpression(%s) == 0x%02x%02x",what,byte0,byte1);

		offset = 0;
		if (!dumpExpression(what,code,&offset))
			return false;
	}
	else	// dumpExpression for real
	{
		offset -= 1;		// real expression offset are one based
		const uint8_t *code = cur_rig_code->expression_pool;
		if (!dumpExpression(what,code,&offset))
			return false;
	}
	return true;
}


static bool dumpParam(bool button_section, int arg_type, bool last, const uint8_t *code, uint16_t *offset)
{
	uint8_t byte;
	uint16_t *ptr16;
	display(dbg_dump+1, "# dumpParam(%s) at offset %d",argTypeToString(arg_type),*offset);

	switch (arg_type)
	{
		case PARAM_AREA_NUM :
		case PARAM_VALUE_NUM :
		case PARAM_VALUE :
		case PARAM_PEDAL_NUM :
		case PARAM_ROTARY_NUM :
		case PARAM_MIDI_CHANNEL :
		case PARAM_MIDI_CC :
		case PARAM_MIDI_VALUE :
		case PARAM_STRING_EXPRESSION :
		case PARAM_LED_COLOR_EXPRESSION :
		case PARAM_DISPLAY_COLOR_EXPRESSION :
			ptr16 = (uint16_t *) &code[*offset];
			if (!dumpCodeExpression(argTypeToString(arg_type),*ptr16))
				return false;
			*offset += 2;
			break;

		case PARAM_FONT_TYPE :
			byte = code[(*offset)++];
			sprintf(&dump_buf[strlen(dump_buf)],"%s",rigTokenToText(byte + RIG_TOKEN_NORMAL));
			break;
		case PARAM_FONT_JUST :
			byte = code[(*offset)++];
			sprintf(&dump_buf[strlen(dump_buf)],"%s",rigTokenToText(byte + RIG_TOKEN_LEFT));
			break;
		case PARAM_MIDI_PORT :
			byte = code[(*offset)++];
			sprintf(&dump_buf[strlen(dump_buf)],"%s",rigTokenToText(byte + RIG_TOKEN_MIDI0));
			break;

		case PARAM_FONT_SIZE :
			byte = code[(*offset)++];
			sprintf(&dump_buf[strlen(dump_buf)],"%d",byte);
			break;
		case PARAM_END_X :
		case PARAM_END_Y :
		case PARAM_START_X :
		case PARAM_START_Y :
			ptr16 = (uint16_t *) &code[*offset];
			sprintf(&dump_buf[strlen(dump_buf)],"%d",*ptr16);
			*offset += 2;
			break;

		case PARAM_PEDAL_NAME :
			ptr16 = (uint16_t *) &code[*offset];
			sprintf(&dump_buf[strlen(dump_buf)],"%s",&rig_code.string_pool[*ptr16]);
			*offset += 2;
			break;

		default:
			rig_error("unknown parameter arg type(%d)",arg_type);
			return false;
			break;
	}
	sprintf(&dump_buf[strlen(dump_buf)],"%s",last?");":", ");
	return true;
}


static bool dumpStatement(bool button_section, uint16_t *offset, uint16_t last_offset)
{
	dump_buf[0] = 0;

	const uint8_t *code = cur_rig_code->statement_pool;
	uint8_t tt = code[(*offset)++];

	proc_level = button_section ? 2 : 0;
	display(dbg_dump+1,"# dumping statement(%d=%s) at offset %d",tt,rigTokenToText(tt),*offset - 1);

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
	display(dbg_dump+1,"# dumping statments list(%d) from %d to %d (%d bytes)",
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

	display(dbg_dump-1,"Dumping Rig",0);
	display(dbg_dump-1,"define_pool_len=%d",cur_rig_header->define_pool_len);
	display(dbg_dump-1,"string_pool_len=%d",cur_rig_header->string_pool_len);
	display(dbg_dump-1,"statement_pool_len=%d",cur_rig_header->statement_pool_len);
	display(dbg_dump-1,"expression_pool_len=%d",cur_rig_header->expression_pool_len);
	display(dbg_dump-1,"num_statements=%d",cur_rig_header->num_statements);
	for (int i=0; i<=cur_rig_header->num_statements; i++)
		display(dbg_dump+2,"statement[%d] = %d",i,cur_rig_header->statements[i]);

	display(dbg_code,"statements",0);
	display_bytes_long(dbg_code,0,cur_rig_code->statement_pool,cur_rig_header->statement_pool_len);
	display(dbg_code,"expressions",0);
	display_bytes_long(dbg_code,0,cur_rig_code->expression_pool,cur_rig_header->expression_pool_len);
	// dump the program

	if (dbg_dump > 0)
	{
		proc_level = save_proc_level;
		return;
	}

	display(0,"------------------------------------------------",0);
	display(0,"",0);
	display(0,"%s",cur_rig_header->overlay_type?"Overlay":"BaseRig");
	display(0,"",0);

	bool ok = 1;
	bool any = 0;
	int prev_define = 0;

	for (int i=0; i<RIG_NUM_DEFINES; i++)
	{
		uint16_t idxP1 = rig_header.define_ids[i];
		if (idxP1)	// definition indicator - they must name em'
		{
			any = 1;

			// blank line between non-subsequent definitions
			if (prev_define && i != prev_define + 1)
				display(0,"",0);

			display(0,"define(%d, %s, %d);",
				i,
				&rig_code.define_pool[idxP1-1],
				rig_header.define_values[i]);
			prev_define = i;
		}
	}

	if (any)
		display(0,"",0);
	for (int i=0; i<RIG_NUM_STRINGS; i++)
	{
		// the offset is incremented so that we can identify
		// accesses to string 0 explicitly.
		uint16_t string_offset = cur_rig_header->strings[i];

		if (string_offset--)	// test and adjust offset
		{
			any = 1;
			display(0,"STRING(%d, \"%s\");",
				i,
				&cur_rig_code->string_pool[string_offset]);
		}
	}

	// dump init_statements

	if (any)
		display(0,"",0);
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
					ok = dumpCodeExpression(rigTokenToText(j + RIG_TOKEN_COLOR),offset);
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
