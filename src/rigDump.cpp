//-------------------------------------------------------
// rigDump.cpp
//-------------------------------------------------------

#include "rigDump.h"		// by convention
#include <myDebug.h>
#include "rigParser.h"
#include "rigExpression.h"


#define dbg_code 1
	// 0 = show header details (arrrays)
	// -1 = show bytes of code
#define dbg_dump 1
	// 1 = basic header vars
	// 0 = dump
	// -1 = details
	// -2 = expressions
	// -3 = opcodes


// a buffer to build output text

#define MAX_DUMP_BUF 255

static char dump_buf[MAX_DUMP_BUF + 1];


//------------------------------------
// methods
//------------------------------------




static bool dumpInline(const rig_t *rig, const uint8_t *code, uint16_t *offset)
{
	int dbg_offset = *offset;
	uint8_t opcode = code[(*offset)++] & ~EXP_INLINE;
	uint8_t value = code[(*offset)++];

	display(dbg_dump+3,"# op at %d=INLINE(0x%02x) value=%d",
		dbg_offset,
		opcode,
		value);

	switch (opcode)
	{
		case EXP_NUMBER :
			sprintf(&dump_buf[strlen(dump_buf)],"%d",value);
			break;
		case EXP_LED_COLOR :
			sprintf(&dump_buf[strlen(dump_buf)],"%s",rigTokenToText(RIG_TOKEN_LED_BLACK + value));
			break;
		case EXP_DISPLAY_COLOR :
			sprintf(&dump_buf[strlen(dump_buf)],"%s",rigTokenToText(RIG_TOKEN_DISPLAY_BLACK + value));
			break;
		case EXP_VALUE :
			sprintf(&dump_buf[strlen(dump_buf)],"VALUE[");
			sprintf(&dump_buf[strlen(dump_buf)],"%d",value);;
			sprintf(&dump_buf[strlen(dump_buf)],"]");
			break;
		case EXP_STRING :
			sprintf(&dump_buf[strlen(dump_buf)],"STRING[");
			sprintf(&dump_buf[strlen(dump_buf)],"%d",value);;
			sprintf(&dump_buf[strlen(dump_buf)],"]");
			break;
		default :
			my_error("dumpRig() - unknown inline op(0x%02x,0x%02x) at offset(%d)",opcode,value,*offset);
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
			my_error("dumpRig() - unknown expression op(0x%02x) at offset(%d)",op,*offset);
			return false;
			break;
	}

	return true;
}



static bool dumpExpression(const rig_t *rig, const char *what, const uint8_t *code, uint16_t *offset)
{
	display(dbg_dump+2,"# dumpExpression(%s) at offset(%d)",what,*offset);

	bool ok = 1;
	while (ok &&  code[*offset] != EXP_END)
	{
		if (code[*offset] & EXP_INLINE)
			ok = dumpInline(rig, code,offset);
		else
			ok = dumpOp(code,offset);
	}

	return ok;
}



static bool dumpCodeExpression(const rig_t *rig, const char *what, uint16_t offset)
{
	if (offset & (EXP_INLINE << 8))
	{
		uint8_t byte0 = offset >> 8;
		uint8_t byte1 = offset & 0xff;
		uint8_t code[] = {byte0, byte1, EXP_END};

		display(dbg_dump+2,"# dumpInlineExpression(%s) == 0x%02x%02x",what,byte0,byte1);

		offset = 0;
		if (!dumpExpression(rig, what, code, &offset))
			return false;
	}
	else	// dumpExpression for real
	{
		offset -= 1;		// real expression offset are one based
		const uint8_t *code = rig->expression_pool;
		if (!dumpExpression(rig, what, code, &offset))
			return false;
	}
	return true;
}


static bool dumpParam(const rig_t *rig, int arg_type, bool last, const uint8_t *code, uint16_t *offset)
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
		case PARAM_LISTEN_CHANNEL :
		case PARAM_MIDI_CC :
		case PARAM_MIDI_VALUE :
		case PARAM_STRING_EXPRESSION :
		case PARAM_LED_COLOR_EXPRESSION :
		case PARAM_DISPLAY_COLOR_EXPRESSION :
			ptr16 = (uint16_t *) &code[*offset];
			if (!dumpCodeExpression(rig,argTypeToString(arg_type),*ptr16))
				return false;
			*offset += 2;
			break;

		case PARAM_METER_TYPE :
			byte = code[(*offset)++];
			sprintf(&dump_buf[strlen(dump_buf)],"%s",rigTokenToText(byte + RIG_TOKEN_HORZ));
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
			sprintf(&dump_buf[strlen(dump_buf)],"%s",rigTokenToText(byte + RIG_TOKEN_USB1));
			break;

		case PARAM_BUTTON_NUM :
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

		case PARAM_RIG_NAME :
		case PARAM_PEDAL_NAME :
			ptr16 = (uint16_t *) &code[*offset];
			sprintf(&dump_buf[strlen(dump_buf)],"%s",&rig->string_pool[*ptr16]);
			*offset += 2;
			break;


		default:
			my_error("dumpRig() - unknown arg_type(%d) at offset(%d)",arg_type,*offset);
			return false;
			break;
	}
	sprintf(&dump_buf[strlen(dump_buf)],"%s",last?");":", ");
	return true;
}


static bool dumpStatement(const rig_t *rig, uint16_t *offset, uint16_t last_offset)
{
	const uint8_t *code = rig->statement_pool;
	uint8_t tt = code[(*offset)++];

	display(dbg_dump+1,"# dumping statement(%d=%s) at offset %d",tt,rigTokenToText(tt),*offset - 1);

	sprintf(dump_buf,"%s(",rigTokenToText(tt));
	const statement_param_t *params = findParams(tt);

	if (params) // opposite should never happen
	{
		const int *arg = params->args;
		while (*arg)
		{
			const int *args = arg++;
			if (!dumpParam(rig,*args,!*arg,code,offset))
			{
				proc_level = 0;
				return false;
			}
		}
	}

	display(0,dump_buf,0);
	return true;
}


static bool dumpStatementList(const rig_t *rig, int indent, int statement_num)
{
	uint16_t offset = rig->statements[statement_num];
	uint16_t last_offset  = rig->statements[statement_num+1];

	proc_level = indent;
	display(dbg_dump+1,"# dumping statement list(%d) from %d to %d (%d bytes)",
		statement_num,
		offset,
		last_offset,
		last_offset-offset);

	while (offset < last_offset)
	{
		if (!dumpStatement(rig,&offset,last_offset))
			return false;
	}
	return true;
}


//------------------------------------------------
// main
//-------------------------------------------------

// extern
void dumpRig(const rig_t *rig)
{
	int save_proc_level = proc_level;
	proc_level = 0;

	delay(500);

	// display interesting numbers]

	display(dbg_dump-1,"Dumping Rig",0);
	display(dbg_dump-1,"define_pool_len=%d",rig->define_pool_len);
	display(dbg_dump-1,"string_pool_len=%d",rig->string_pool_len);
	display(dbg_dump-1,"statement_pool_len=%d",rig->statement_pool_len);
	display(dbg_dump-1,"expression_pool_len=%d",rig->expression_pool_len);
	display(dbg_dump-1,"num_statements=%d",rig->num_statements);

	for (int i=0; i<=rig->num_statements; i++)
		display(dbg_code,"    statement[%d] = %d",i,rig->statements[i]);

	display(dbg_code,"button_types",0);
	for (int i=0; i<NUM_BUTTONS; i++)
	{
		display(dbg_code,"    button_type(%d) = 0x%04x",i,rig->button_type[i]);
	}

	display(dbg_code,"button_refs",0);
	for (int i=0; i<NUM_BUTTONS; i++)
	{
		const uint16_t *refs = rig->button_refs[i];
		display(dbg_code,"    refs(%d)  0x%04x  0x%04x  0x%04x",
			i,
			refs[0],
			refs[1],
			refs[2]);
	}

	display(dbg_code+1,"statements",0);
	display_bytes_long(dbg_code+1,0,rig->statement_pool,rig->statement_pool_len);
	display(dbg_code+1,"expressions",0);
	display_bytes_long(dbg_code+1,0,rig->expression_pool,rig->expression_pool_len);

	// dump the program

	if (dbg_dump > 0)
	{
		proc_level = save_proc_level;
		return;
	}

	display(0,"------------------------------------------------",0);
	display(0,"",0);
	display(0,"%s",rig->rig_type & RIG_TYPE_MODAL ?"ModalRig":"BaseRig");
	display(0,"",0);

	bool ok = 1;
	bool any = 0;
	int prev_define = 0;

	for (int i=0; i<RIG_NUM_DEFINES; i++)
	{
		uint16_t idxP1 = rig->define_ids[i];
		if (idxP1)	// definition indicator - they must name em'
		{
			any = 1;

			// blank line between non-subsequent definitions
			if (prev_define && i != prev_define + 1)
				display(0,"",0);

			display(0,"define(%d, %s, %d);",
				i,
				&rig->define_pool[idxP1-1],
				rig->define_values[i]);
			prev_define = i;
		}
	}

	if (any)
		display(0,"",0);
	for (int i=0; i<RIG_NUM_STRINGS; i++)
	{
		// the offset is incremented so that we can identify
		// accesses to string 0 explicitly.
		uint16_t string_offset = rig->strings[i];

		if (string_offset--)	// test and adjust offset
		{
			any = 1;
			display(0,"STRING(%d, \"%s\");",
				i,
				&rig->string_pool[string_offset]);
		}
	}

	// dump init_statements

	if (any)
		display(0,"",0);
	if (rig->statements[0] < rig->statements[0])
	{
		ok = ok && dumpStatementList(rig,0,0);
		if (ok)	display(0,"",0);
	}
	if (rig->statements[1] < rig->statements[2])
	{
		display(0,"onUpdate:",0);
		ok = ok && dumpStatementList(rig,1,1);
		if (ok)	display(0,"",0);
	}

	bool started = 0;
	for (int i=0; i<NUM_BUTTONS && ok; i++)
	{
		uint16_t type = rig->button_type[i];
		if (type)
		{
			if (started)
				display(0,"",0);
			started = 1;

			if (type == BUTTON_TYPE_INHERIT)
			{
				display(0,"BUTTON(%d): INHERIT",i);
				continue;
			}

			display(0,"BUTTON(%d):",i);
			if (type & BUTTON_TYPE_UPDATE)
			{
				ok = dumpStatementList(rig,1,rig->button_refs[i][0]-1);
			}
			if (ok && (type & BUTTON_TYPE_MASK_REF1))
			{
				display(0,"    %s:",
					type & BUTTON_TYPE_CLICK ? "repeat" :
					type & BUTTON_TYPE_PRESS ? "press" :
					"click");
				ok = dumpStatementList(rig,2,rig->button_refs[i][1]-1);
			}
			if (ok && (type & BUTTON_TYPE_MASK_REF2))
			{
				display(0,"    %s:",
					type & BUTTON_TYPE_CLICK ? "release" :
					"long");
				ok = dumpStatementList(rig,2,rig->button_refs[i][2]-1);
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
