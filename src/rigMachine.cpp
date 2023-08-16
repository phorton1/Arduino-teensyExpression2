//-------------------------------------------------------
// rigMachine.cpp
//-------------------------------------------------------

#include <myDebug.h>
#include "rigMachine.h"
#include "rigExpression.h"
#include "buttons.h"


#define dbg_rig 	0

#define dbg_stmt    0
	// 0  = just show statement header
	// -1 = show statment result
#define dbg_param	0
	// 0  = just show final param value
	// -1 = show param details and expression header
	// -2 = show expression return value

rigMachine rig_machine;


bool rigMachine::loadRig(const char *rig_name)
{
	display(dbg_rig,"loadRig(%s)",rig_name);
	proc_entry();

	if (strlen(rig_name) > MAX_RIG_NAME)
	{
		rig_error("loadRig() rig_name too long",0);
		proc_leave();
		return false;
	}

	m_rig_name[0] = 0;
	memset(&m_rig_state,0,sizeof(rigState_t));

	// load and start the rig

	m_rig_loaded = parseRig(rig_name);
	m_rig_loaded = m_rig_loaded && startRig();

	if (m_rig_loaded)
		strcpy(m_rig_name,rig_name);

	proc_leave();
	display(dbg_rig,"loadRig(%s) finished with %d",rig_name,m_rig_loaded);
	return m_rig_loaded;
}


//-----------------------------------------------
// rig execution
//-----------------------------------------------

bool rigMachine::startRig()
{
	display(dbg_rig,"startRig()",0);
	proc_entry();

	bool ok = executeStatementList(0);

	// setup the buttons

	for (int i=0; i<NUM_BUTTONS && ok; i++)
	{
		#define SUBSECTION_LAST_CODE  (RIG_TOKEN_REPEAT - RIG_TOKEN_COLOR)

		// invariantly add the long click for the system button

		int mask = i == THE_SYSTEM_BUTTON ? BUTTON_EVENT_LONG_CLICK : 0;	// BUTTON_MASK_USER_DRAW

		for (uint16_t j=RIG_TOKEN_PRESS; j<=RIG_TOKEN_REPEAT && ok; j++)
		{
			uint16_t ref = rig_header.button_refs[i][j - RIG_TOKEN_COLOR];
			if (ref)
			{
				// prh - parser should only allow click on THE_SYSTEM_BUTTON
				// and should not allow REPEAT on CLICKS
				if (j == RIG_TOKEN_PRESS)	mask |= BUTTON_EVENT_PRESS;
				if (j == RIG_TOKEN_CLICK)   mask |= BUTTON_EVENT_CLICK;
				if (j == RIG_TOKEN_LONG)	mask |= BUTTON_EVENT_LONG_CLICK;
				if (j == RIG_TOKEN_RELEASE) mask |= BUTTON_EVENT_RELEASE;
				if (j == RIG_TOKEN_REPEAT)  mask |= BUTTON_EVENT_PRESS | BUTTON_MASK_REPEAT;
			}
		}
		if (mask)
		{
			display(dbg_rig,"setButton(%d,0x%04x)",i,mask);
			theButtons.setButtonType(i,mask);
		}
	}
	proc_leave();
	display(dbg_rig,"startRig() finished",0);
	return ok;
}


bool rigMachine::executeStatementList(int statement_num)
{
	uint16_t offset = cur_rig_header->statements[statement_num];
	uint16_t last_offset  = cur_rig_header->statements[statement_num+1];

	display(dbg_rig,"executeStatmentList(%d) from %d to %d (%d bytes)",
		statement_num,
		offset,
		last_offset,
		last_offset-offset);
	proc_entry();

	while (offset < last_offset)
	{
		if (!executeStatement(&offset,last_offset))
		{
			proc_leave();
			return false;
		}
	}

	proc_leave();
	display(dbg_rig,"executeStatementList() finished",0);
	return true;
}


bool rigMachine::executeStatement(uint16_t *offset, uint16_t last_offset)
{
	const uint8_t *code = cur_rig_code->statement_pool;
	uint8_t tt = code[(*offset)++];

	display(dbg_stmt,"executeStatement(%d=%s) at offset %d",tt,rigTokenToText(tt),*offset - 1);
	proc_entry();

	const statement_param_t *params = findParams(tt);
	if (!params) // opposite should never happen
	{
		proc_leave();
		return false;
	}

	int param_num = 0;
	const int *arg = params->args;
	while (*arg)
	{
		if (!evalParam(&m_param_values[param_num++],*arg++,code,offset))
		{
			proc_leave();
			return false;
		}
	}

	bool ok = 1;
	switch (tt)
	{
		case RIG_TOKEN_SETVALUE:
		{
			uint16_t val0 = m_param_values[0].value;
			uint16_t val1 = m_param_values[1].value;

			display(dbg_rig,"setValue(%d,%d)",val0,val1);
			if (val0 >= RIG_NUM_VALUES)
			{
				rig_error("setValue(%d, V) value number must be less than %d",val0,RIG_NUM_VALUES);
				ok = 0;
			}
			if (val1 > MAX_RIG_VALUE)
			{
				rig_error("setValue(N, %d) value number must be less than %d",val1,MAX_RIG_VALUE + 1);
				ok = 0;
			}
			if (ok)
			{
				m_rig_state.values[val0] = val1;
			}
			break;
		}
		case RIG_TOKEN_AREA:
		case RIG_TOKEN_LISTEN:
		case RIG_TOKEN_PEDAL:
		case RIG_TOKEN_ROTARY:
		case RIG_TOKEN_DISPLAY:
		case RIG_TOKEN_SEND_CC:
		case RIG_TOKEN_SEND_PGM_CHG:
		case RIG_TOKEN_NOTE_ON:
		case RIG_TOKEN_NOTE_OFF:
		case RIG_TOKEN_ALL_NOTES_OFF:
		case RIG_TOKEN_FTP_TUNER:
		case RIG_TOKEN_FTP_SENSITIVITY:
			break;
		default:
			rig_error("unknown rigStatement: %d=%s",tt,rigTokenToString(tt));
			ok = 0;
			break;
	}

	proc_leave();
	if (ok)
		display(dbg_stmt+1,"executeStatement() finished",0);
	return ok;
}


bool rigMachine::evalParam(evalResult_t *rslt, int arg_type, const uint8_t *code, uint16_t *offset)
{
	display(dbg_param+1,"evalParam(%s) at code_offset %d",argTypeToString(arg_type),*offset);
	proc_entry();

	rslt->is_string = 0;

	uint16_t *ptr16;
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
			if (!evalCodeExpression(rslt,argTypeToString(arg_type),*ptr16))
			{
				proc_leave();
				return false;
			}
			*offset += 2;
			break;

		case PARAM_FONT_TYPE :
		case PARAM_FONT_JUST :
		case PARAM_FONT_SIZE :
		case PARAM_MIDI_PORT :
			rslt->value = code[(*offset)++];
			break;
		case PARAM_END_X :
		case PARAM_END_Y :
		case PARAM_START_X :
		case PARAM_START_Y :
			ptr16 = (uint16_t *) &code[*offset];
			rslt->value = *ptr16;
			*offset += 2;
			break;

		case PARAM_PEDAL_NAME :
			ptr16 = (uint16_t *) &code[*offset];
			rslt->text = &rig_code.string_pool[*ptr16];
			rslt->is_string = 1;
			*offset += 2;
			break;

		default:
			rig_error("unknown arg_type(%d)",arg_type);
			proc_leave();
			return false;
			break;
	}

	proc_leave();
	if (rslt->is_string)
		display(dbg_param,"evalParam(%s) returning(%s) at code_offset %d",argTypeToString(arg_type),rslt->text,*offset);
	else
		display(dbg_param,"evalParam(%s) returning(%d) at code_offset %d",argTypeToString(arg_type),rslt->value,*offset);
	return true;
}




bool rigMachine::evalCodeExpression(evalResult_t *rslt, const char *what, uint16_t offset)
{
	if (offset & (EXP_INLINE << 8))
	{
		uint8_t byte = offset >> 8;
		uint8_t inline_op = byte & 0x3f;
		uint8_t value = offset & 0xff;

		display(dbg_param+1,"inline %s == 0x%02x%02x",what,byte,value);

		if (byte & EXP_INLINE_ID)
		{
			display(dbg_param+1,"inline id(%d)=%s",value,&rig_code.define_pool[rig_header.define_ids[value] - 1]);
			value = rig_header.define_values[value];
		}

		if (inline_op == EXP_LED_COLOR)
		{
			display(dbg_param+1,"inline LED_COLOR(%d) = %s",value,rigTokenToText(RIG_TOKEN_LED_BLACK + value));
		}
		else if (inline_op == EXP_DISPLAY_COLOR)
		{
			display(dbg_param+1,"inline DISPLAY_COLOR(%d) = %s",value,rigTokenToText(RIG_TOKEN_DISPLAY_BLACK + value));
		}

		if (inline_op == EXP_VALUE)
		{
			display(dbg_param+1,"inline VALUE[%d]",value);
			value = m_rig_state.values[value];
		}

		if (inline_op == EXP_STRING)
		{
			display(dbg_param+1,"inline STRING[%d]",value);
			uint16_t off = rig_header.strings[value];
			rslt->text = &rig_code.string_pool[off];
			rslt->is_string = 1;
		}
		else
		{
			rslt->value = value;
		}
	}
	else	// dumpExpression for real
	{
		offset -= 1;		// real expression offset are one based
		const uint8_t *code = cur_rig_code->expression_pool;
		if (!evalExpression(rslt,what,code,&offset))
			return false;
	}
	return true;
}



bool rigMachine::evalExpression(evalResult_t *rslt, const char *what, const uint8_t *code, uint16_t *offset)
{
	display(dbg_param+1,"evalExpression(%s) at offset %d",what,*offset);
	proc_entry();

	if (!expression(rslt,code,offset))
	{
		rig_error("evalExpression(%s) failed at offset %d",what,*offset);
		proc_leave();
		return false;
	}

	proc_leave();
	if (rslt->is_string)
		display(dbg_param+2,"evalExpression(%s) returning(%d) at offset %d",what,rslt->value,*offset);
	else
		display(dbg_param+2,"evalExpression(%s) returning(%s) at offset %d",what,rslt->text,*offset);
	return true;
}


//--------------------------------------------------
// rig events
//--------------------------------------------------

void rigMachine::onMidiCC(int port, int channel, int cc_num, int value)
{

}
void rigMachine::onSerialMidi(int cc_num, int value)
{

}

void rigMachine::onButton(int row, int col, int event)
{
	int num = row * NUM_BUTTON_COLS + col;

	display(dbg_rig,"onButton(%d) 0x%04x proc_level=%d",num,event,proc_level);
	proc_entry();

	uint16_t *refs = rig_header.button_refs[num];
	uint16_t tt = 0;

	if (event == BUTTON_EVENT_PRESS)
	{
		if (refs[SUBSECTION_NUM(RIG_TOKEN_REPEAT)])		// press should not be allowed with repeat; repeat takes precedence
			tt = RIG_TOKEN_PRESS;
		else
			tt = RIG_TOKEN_PRESS;
	}
	else if (event == BUTTON_EVENT_CLICK)
		tt = RIG_TOKEN_CLICK;
	else if (event == BUTTON_EVENT_LONG_CLICK)
		tt = RIG_TOKEN_LONG;
	else if (event == BUTTON_EVENT_RELEASE)
		tt = RIG_TOKEN_RELEASE;

	if (tt)
	{
		int ref_num = SUBSECTION_NUM(tt);
		uint16_t ref = refs[ref_num];
		display(dbg_rig,"checking tt(%s=%d) ref=%d",rigTokenToText(tt),ref_num,ref);
		if (ref)
			executeStatementList(ref-1);
	}

	proc_leave();
	display(dbg_rig,"onButton(%d) 0x%04x finished",num,event);
}



//--------------------------------------------------
// updateUI
//--------------------------------------------------

void rigMachine::updateUI()
{

}
