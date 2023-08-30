//-------------------------------------------------------
// rigMachine.cpp
//-------------------------------------------------------
// Remember that the expression evaluator is not re-entrant!
// We 'solved' the problem by calling buttons, pedals, and rotaries
// from the_system.loop() method, which also then calls rigMachine::updateUI(),
// so we never find ourselves in the middle of executing a statement
// from onButton, and evalauating it's parameters, at the same time as we
// try to evaluate a COLOR/BLINK expression.


#include <myDebug.h>
#include "rigMachine.h"
#include "rigExpression.h"
#include "buttons.h"
#include "myLeds.h"
#include "myTFT.h"
#include "theSystem.h"
#include "pedals.h"
#include "rotary.h"
#include "winFtpTuner.h"
#include "winFtpSensitivity.h"


#define dbg_rig 	0
	// 0 = show loadRig() and onButton() calls
	// -1 = show rig setButtonType() calls
#define dbg_stmt    0
	// 0  = show executeStatementList()
	// -1 = show individual Statements
#define dbg_calls	0
	// 0 = show functions actually called by statements
	// -1 = show rigDisplay() details
#define dbg_param	1
	// 0  = just show final param value
	// -1 = show param details and expression header
	// -2 = show expression return value
#define dbg_midi    0
	// 0 = show setting of Listen Values
	// -1 = show onMidiCC header
#define dbg_btns	1
	// show calls to setButtonColor and setButtonBlink during loop() expression handling

#define MAX_RIG_STACK  16



typedef struct
{
	const rig_t *rig;
	char name[MAX_RIG_NAME + 1];
	const char *define_pool;
	const char *string_pool;
	const uint8_t *statement_pool;
	const uint8_t *expression_pool;
} rigStack_t;

int rig_stack_ptr = 0;
rigStack_t rig_stack[MAX_RIG_STACK];
rigMachine rig_machine;

uint16_t listen_mask;
	// 7 bits for each port for more rapid processing


//------------------------------------------------------
// enumerated types
//------------------------------------------------------

const uint16_t DISPLAY_COLORS[] = {
	TFT_BLACK,
	TFT_BLUE,
	TFT_RED,
	TFT_GREEN,
	TFT_CYAN,
	TFT_MAGENTA,
	TFT_YELLOW,
	TFT_WHITE,
	TFT_NAVY,
	TFT_DARKGREEN,
	TFT_DARKCYAN,
	TFT_MAROON,
	TFT_PURPLE,
	TFT_OLIVE,
	TFT_LIGHTGREY,
	TFT_DARKGREY,
	TFT_ORANGE,
	TFT_GREENYELLOW,
	TFT_PINK,
};

const uint32_t LED_COLORS[] = {
	LED_BLACK,
	LED_RED,
	LED_GREEN,
	LED_BLUE,
	LED_YELLOW,
	LED_PURPLE,
	LED_ORANGE,
	LED_WHITE,
	LED_CYAN,
};


//----------------------------------------
// rig_stack
//----------------------------------------

bool rigMachine::pushRig(const rig_t *rig, const char *name)
{
	if (rig_stack_ptr >= MAX_RIG_STACK)
	{
		my_error("RIG_STACK OVERFLOW!!",0);
		return 0;
	}
	display(dbg_rig,"pushRig(%d,%s)",rig_stack_ptr,name);
	rig_stack[rig_stack_ptr].rig = rig;
	strcpy(rig_stack[rig_stack_ptr].name,name);
	rig_stack_ptr++;
	return 1;
}

void rigMachine::popRig(bool exec_prev)
{
	if (rig_stack_ptr < 2)
	{
		my_error("RIG_STACK_UNDERFLOW!!",0);
		return;
	}

	const char *old_name = rig_stack[rig_stack_ptr-1].name;
	display(dbg_rig,"popRig(%d,%s) exec_prev=%d",rig_stack_ptr-1,old_name,exec_prev);
	proc_entry();

	if (exec_prev)
	{
		// cancel any buttons defined by the rig being popped

		for (int i=0; i<NUM_BUTTONS; i++)
		{
			if (rig_stack[rig_stack_ptr-1].rig->button_refs[i][0] != BUTTON_INHERIT_FLAG)
			{
				display(dbg_rig+1,"cancelButton(%d)",i);
				theButtons.setButtonType(i,0);
			}
		}
	}

	rig_stack_ptr--;
	if (exec_prev)
	{
		startRig(rig_stack[rig_stack_ptr-1].rig,0);
		the_system.setTitle(rig_stack[rig_stack_ptr-1].name);
	}

	proc_leave();
	display(dbg_rig,"popRig(%d,%s) finished",rig_stack_ptr,old_name);
}


static const uint16_t *inheritButtonRefs(int num, const rig_t **ret_context)
{
	int use_ptr = rig_stack_ptr;
	const rig_t *context = rig_stack[use_ptr-1].rig;
	const uint16_t *refs = context->button_refs[num];
	uint16_t ref = refs[0];

	// somehow need to implement inherit

	while (ref == BUTTON_INHERIT_FLAG)
	{
		display(dbg_btns,"rig(%d,%s) updateUI(%d) INHERITING from rig(%d,%s)",
			use_ptr-1,
			rig_stack[use_ptr-1].name,
			num,
			use_ptr-2,
			rig_stack[use_ptr-2].name);
		use_ptr--;
		context = rig_stack[use_ptr-1].rig;
		refs = context->button_refs[num];
		ref = refs[0];
	}

	*ret_context = context;
	return refs;
}



//------------------------------------------------------
// methods
//-------------------------------------------------------

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

	bool save_rig_loaded = m_rig_loaded;
	m_rig_loaded = 0;
		// stop the machine from processing the current rig, if any

	const rig_t *rig = parseRig(rig_name);
		// call the parser

	bool ok = 0;
	if (rig)
	{
		bool base_rig = !(rig->rig_type & RIG_TYPE_MODAL);
		if (base_rig)
			rig_stack_ptr = 0;
		ok = pushRig(rig,rig_name);
		if (ok)
		{
			ok = startRig(rig,base_rig);
			if (!ok)
			{
				popRig(0);
				m_rig_loaded = save_rig_loaded;
			}
			else
			{
				strcpy(m_rig_name,rig_name);
				m_rig_loaded = 1;
				the_system.setTitle(rig_name);
			}
		}
	}

	proc_leave();
	display(dbg_rig,"loadRig(%s) finished with %d",rig_name,m_rig_loaded);
	return ok;
}


//-----------------------------------------------
// rig execution
//-----------------------------------------------

void rigMachine::restartRig()
{
	if (!rig_stack_ptr)
	{
		warning(0,"attempt to start rig when non on stack!",0);
		return;
	}
	if (!m_rig_loaded)
	{
		warning(0,"attempt to start rig when none loaded!",0);
		return;
	}
	startRig(rig_stack[rig_stack_ptr-1].rig,0);
}


bool rigMachine::startRig(const rig_t *rig, bool cold)
{
	display(dbg_rig,"startRig(%d)",cold);
	proc_entry();

	if (cold)
	{
		listen_mask = 0;
		memset(&m_rig_state,0,sizeof(rigState_t));
	}

	// clear the client area

	fillRect(client_rect,TFT_BLACK);

	// process the init section statements

	bool ok = executeStatementList(rig,0);

	// setup the buttons

	for (int i=0; i<NUM_BUTTONS && ok; i++)
	{
		#define SUBSECTION_LAST_CODE  (RIG_TOKEN_REPEAT - RIG_TOKEN_COLOR)

		// invariantly add the long click for the system button
		// which *may* be inherited by this rig

		int mask = (i == THE_SYSTEM_BUTTON) ? BUTTON_EVENT_LONG_CLICK : 0;

		const rig_t *context;
		const uint16_t *refs = inheritButtonRefs(i,&context);
		for (uint16_t j=RIG_TOKEN_PRESS; j<=RIG_TOKEN_REPEAT && ok; j++)
		{
			if (refs[j - RIG_TOKEN_COLOR])
			{
				// prh - parser should only allow click on THE_SYSTEM_BUTTON
				// and should not allow REPEAT on CLICKS or PRESS
				if (j == RIG_TOKEN_PRESS)	mask |= BUTTON_EVENT_PRESS;
				if (j == RIG_TOKEN_CLICK)   mask |= BUTTON_EVENT_CLICK;
				if (j == RIG_TOKEN_LONG)	mask |= BUTTON_EVENT_LONG_CLICK;
				if (j == RIG_TOKEN_RELEASE) mask |= BUTTON_EVENT_RELEASE;
				if (j == RIG_TOKEN_REPEAT)  mask |= BUTTON_EVENT_PRESS | BUTTON_MASK_REPEAT;
			}
		}

		display(dbg_rig+1,"setButton(%d,0x%04x)",i,mask);
		theButtons.setButtonType(i,mask);
	}
	proc_leave();
	display(dbg_rig,"startRig() finished",0);
	return ok;
}



//-------------------------------------------------
// rig statement primitives
//-------------------------------------------------

void rigMachine::rigDisplay(uint16_t area_num, uint16_t color, const char *text)
{
	proc_entry();
	rigArea_t *area = &m_rig_state.areas[area_num];

	display(dbg_calls+1,"rigDisplay(%d,%d=%s,\"%s\") font_size=%d",
		area_num,
		color,
		rigTokenToText(color + RIG_TOKEN_DISPLAY_BLACK),
		text,
		area->font_size);


	const ILI9341_t3_font_t *font = 0;

	if (area->font_type)
	{
		switch (area->font_size)
		{
			case 12: font = &Arial_12_Bold; break;
			case 14: font = &Arial_14_Bold; break;
			case 16: font = &Arial_16_Bold; break;
			case 18: font = &Arial_18_Bold; break;
			case 20: font = &Arial_20_Bold; break;
			case 24: font = &Arial_24_Bold; break;
			case 28: font = &Arial_28_Bold; break;
			case 32: font = &Arial_32_Bold; break;
			case 40: font = &Arial_40_Bold; break;
			case 48: font = &Arial_48_Bold; break;
			default:
				rig_error("Unknown font_size(%d)_bold",area->font_type);
				proc_leave();
				return;
				break;
		}
	}
	else
	{
		switch (area->font_size)
		{
			case 12: font = &Arial_12; break;
			case 14: font = &Arial_14; break;
			case 16: font = &Arial_16; break;
			case 18: font = &Arial_18; break;
			case 20: font = &Arial_20; break;
			case 24: font = &Arial_24; break;
			case 28: font = &Arial_28; break;
			case 32: font = &Arial_32; break;
			case 40: font = &Arial_40; break;
			case 48: font = &Arial_48; break;
				rig_error("Unknown font_size(%d)",area->font_type);
				proc_leave();
				return;
				break;
		}
	}

	display(dbg_calls+1,"calling print_justified(%d,%d,%d,%d,  %d, %d=%s,BLACK,1,\"%s\")",
		client_rect.xs + area->xs,
		client_rect.ys + area->ys,
		area->xe - area->xs + 1,
		area->ye - area->ys + 1,
		area->font_just,
		color,
		rigTokenToText(color + RIG_TOKEN_DISPLAY_BLACK),
		text);

	mylcd.setFont(*font);

	mylcd.printJustified(
		client_rect.xs + area->xs,
		client_rect.ys + area->ys,
		area->xe - area->xs + 1,
		area->ye - area->ys + 1,
		area->font_just,
		DISPLAY_COLORS[color],
		TFT_BLACK,
		1,	// use bc
		text);
	proc_leave();
}



//-------------------------------------------------------------------
// Expression Evaluation
//-------------------------------------------------------------------

bool rigMachine::evalExpression(const rig_t *rig, evalResult_t *rslt, const char *what, const uint8_t *code, uint16_t *offset)
{
	display(dbg_param+1,"evalExpression(%s) at offset %d",what,*offset);
	proc_entry();

	if (!expression(rig, rslt,code,offset))
	{
		rig_error("evalExpression(%s) failed at offset %d",what,*offset);
		proc_leave();
		return false;
	}

	proc_leave();
	if (rslt->is_string)
		display(dbg_param+2,"evalExpression(%s) returning(%s) at offset %d",what,rslt->text,*offset);
	else
		display(dbg_param+2,"evalExpression(%s) returning(%d) at offset %d",what,rslt->value,*offset);
	return true;
}


bool rigMachine::evalCodeExpression(const rig_t *rig, evalResult_t *rslt, const char *what, uint16_t offset)
{
	if (offset & (EXP_INLINE << 8))
	{
		uint8_t byte = offset >> 8;
		uint8_t inline_op = byte & 0x3f;
		uint8_t value = offset & 0xff;

		display(dbg_param+1,"inline %s == 0x%02x%02x",what,byte,value);

		if (byte & EXP_INLINE_ID)
		{
			display(dbg_param+1,"inline id(%d)=%s",value,&rig->define_pool[rig->define_ids[value] - 1]);
			value = rig->define_values[value];
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
			uint16_t off = rig->strings[value];
			rslt->text = &rig->string_pool[off];
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
		const uint8_t *code = rig->expression_pool;
		if (!evalExpression(rig,rslt,what,code,&offset))
			return false;
	}
	return true;
}


//---------------------------------------------------
// Statement execution
//---------------------------------------------------

bool rigMachine::evalParam(const rig_t *rig, evalResult_t *rslt, int arg_type, const uint8_t *code, uint16_t *offset)
{
	const char *what = argTypeToString(arg_type);
	display(dbg_param+1,"evalParam(%s) at code_offset %d",what,*offset);
	proc_entry();

	bool ok = 1;
	uint16_t max = 0;
	uint16_t *ptr16 = 0;
	rslt->is_string = 0;

	switch (arg_type)
	{
		case PARAM_AREA_NUM :		max = RIG_NUM_AREAS - 1;	break;
		case PARAM_VALUE_NUM :		max = RIG_NUM_VALUES - 1;	break;
		case PARAM_VALUE :			max = MAX_RIG_VALUE;		break;
		case PARAM_PEDAL_NUM :		max = NUM_PEDALS - 1;		break;
		case PARAM_ROTARY_NUM :		max = NUM_ROTARY - 1;		break;
		case PARAM_LISTEN_CHANNEL :	max = MIDI_MAX_CHANNEL;		break;
		case PARAM_MIDI_CC :		max = MIDI_MAX_VALUE;		break;
		case PARAM_MIDI_VALUE :		max = MIDI_MAX_VALUE;		break;
	}

	switch (arg_type)
	{
		case PARAM_MIDI_CHANNEL :
			ptr16 = (uint16_t *) &code[*offset];
			ok = evalCodeExpression(rig,rslt,argTypeToString(arg_type),*ptr16);
			if (ok && (
				rslt->value > MIDI_MAX_CHANNEL ||
				rslt->value < MIDI_MIN_CHANNEL))
			{
				rig_error("%s must be between %d and %d",what,MIDI_MIN_CHANNEL,MIDI_MAX_CHANNEL);
				ok = 0;
			}
			*offset += 2;
			break;		case PARAM_AREA_NUM :
		case PARAM_VALUE_NUM :
		case PARAM_VALUE :
		case PARAM_PEDAL_NUM :
		case PARAM_ROTARY_NUM :
		case PARAM_LISTEN_CHANNEL :
		case PARAM_MIDI_CC :
		case PARAM_MIDI_VALUE :
			ptr16 = (uint16_t *) &code[*offset];
			ok = evalCodeExpression(rig,rslt,argTypeToString(arg_type),*ptr16);
			if (ok && rslt->value > max)
			{
				rig_error("%s must be %d or less",what,max);
				ok = 0;
			}
			*offset += 2;
			break;
		case PARAM_STRING_EXPRESSION :
		case PARAM_LED_COLOR_EXPRESSION :
		case PARAM_DISPLAY_COLOR_EXPRESSION :
			ptr16 = (uint16_t *) &code[*offset];
			ok = evalCodeExpression(rig,rslt,argTypeToString(arg_type),*ptr16);
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

		case PARAM_RIG_NAME :
		case PARAM_PEDAL_NAME :
			ptr16 = (uint16_t *) &code[*offset];
			rslt->text = &rig->string_pool[*ptr16];
			rslt->is_string = 1;
			*offset += 2;
			break;

		default:
			rig_error("unknown arg_type(%d)",arg_type);
			ok = 0;
			break;
	}

	proc_leave();
	if (ok)
	{
		if (rslt->is_string)
			display(dbg_param,"evalParam(%s) returning(%s) at code_offset %d",argTypeToString(arg_type),rslt->text,*offset);
		else
			display(dbg_param,"evalParam(%s) returning(%d) at code_offset %d",argTypeToString(arg_type),rslt->value,*offset);
	}
	return true;
}


bool rigMachine::executeStatement(const rig_t *rig, uint16_t *offset, uint16_t last_offset)
{
	const uint8_t *code = rig->statement_pool;
	uint8_t tt = code[(*offset)++];

	display(dbg_stmt+1,"executeStatement(%d=%s) at offset %d",tt,rigTokenToText(tt),*offset - 1);
	proc_entry();

	const statement_param_t *params = findParams(tt);
	bool ok = params;

	int param_num = 0;
	const int *arg = params->args;
	while (ok && *arg)
	{
		ok = evalParam(rig,&m_param_values[param_num++],*arg++,code,offset);
	}

	if (ok)
	{
		uint16_t idx = 0;
		switch (tt)
		{
			case RIG_TOKEN_SETVALUE:
				display(dbg_calls,"setValue(%d,%d)",
					m_param_values[0].value,
					m_param_values[1].value);
				m_rig_state.values[m_param_values[0].value] = m_param_values[1].value;
				break;

			case RIG_TOKEN_AREA:
				display(dbg_calls,"AREA(%d,%d,%s,%s,%d,%d,%d,%d)",
					m_param_values[0].value,
					m_param_values[1].value,
					rigTokenToText(m_param_values[2].value + RIG_TOKEN_NORMAL),
					rigTokenToText(m_param_values[3].value + RIG_TOKEN_LEFT),
					m_param_values[4].value,
					m_param_values[5].value,
					m_param_values[6].value,
					m_param_values[7].value);
				idx = m_param_values[0].value;
				m_rig_state.areas[idx].font_size = m_param_values[1].value;
				m_rig_state.areas[idx].font_type = m_param_values[2].value;
				m_rig_state.areas[idx].font_just = m_param_values[3].value;
				m_rig_state.areas[idx].xs   	 = m_param_values[4].value;
				m_rig_state.areas[idx].ys   	 = m_param_values[5].value;
				m_rig_state.areas[idx].xe   	 = m_param_values[6].value;
				m_rig_state.areas[idx].ye   	 = m_param_values[7].value;
				break;

			case RIG_TOKEN_LISTEN:
				display(dbg_calls,"LISTEN(%d,%s,%d,%d)",
					m_param_values[0].value,
					rigTokenToText(m_param_values[1].value + RIG_TOKEN_USB1),
					m_param_values[2].value,
					m_param_values[3].value);
				idx = m_param_values[0].value;
				m_rig_state.listens[idx].active  = 1;
				m_rig_state.listens[idx].port    = m_param_values[1].value;
				m_rig_state.listens[idx].channel = m_param_values[2].value;
				m_rig_state.listens[idx].cc      = m_param_values[3].value;
				listen_mask |= (1 << m_param_values[1].value);
				break;

			case RIG_TOKEN_PEDAL:
				display(dbg_calls,"PEDAL(%d,%s,%d=%s,%d,%d)",
					m_param_values[0].value,
					m_param_values[1].text,
					m_param_values[2].value,
					rigTokenToText(m_param_values[2].value + RIG_TOKEN_USB1),
					m_param_values[3].value,
					m_param_values[4].value);
				thePedals.setPedal(
					m_param_values[0].value,			// pedeal num
					m_param_values[1].text,		// name
					MIDI_ENUM_TO_PORT(m_param_values[2].value),	// port
					m_param_values[3].value - 1,		// switch to zero based channel number
					m_param_values[4].value);			// cc
				break;

			case RIG_TOKEN_ROTARY:
				display(dbg_calls,"ROTARY(%d,%d=%s,%d,%d)",
					m_param_values[0].value,
					m_param_values[1].value,
					rigTokenToText(m_param_values[1].value + RIG_TOKEN_USB1),
					m_param_values[2].value,
					m_param_values[3].value);
				setRotary(
					m_param_values[0].value,			// rotary num
					MIDI_ENUM_TO_PORT(m_param_values[1].value),	// port
					m_param_values[2].value - 1,		// switch to zero based channel number
					m_param_values[3].value);			// cc
				break;

			case RIG_TOKEN_DISPLAY:
				display(dbg_calls,"display(%d,%d=%s,\"%s\")",
					m_param_values[0].value,
					m_param_values[1].value,
					rigTokenToText(m_param_values[1].value + RIG_TOKEN_DISPLAY_BLACK),
					m_param_values[2].text);
				rigDisplay(
					m_param_values[0].value,
					m_param_values[1].value,
					m_param_values[2].text);
				break;

			case RIG_TOKEN_SEND_CC:
				display(dbg_calls,"sendCC(%d=%s,%d,%d,%d)",
					m_param_values[0].value,
					rigTokenToText(m_param_values[0].value + RIG_TOKEN_USB1),
					m_param_values[1].value,
					m_param_values[2].value,
					m_param_values[3].value);
				sendMidiControlChange(
					MIDI_ENUM_TO_PORT(m_param_values[0].value),
					m_param_values[1].value,
					m_param_values[2].value,
					m_param_values[3].value);
				break;

			case RIG_TOKEN_SEND_PGM_CHG:
				display(dbg_calls,"sendPgmChg(%d=%s,%d,%d)",
					m_param_values[0].value,
					rigTokenToText(m_param_values[0].value + RIG_TOKEN_USB1),
					m_param_values[1].value,
					m_param_values[2].value);
				sendMidiProgramChange(
					MIDI_ENUM_TO_PORT(m_param_values[0].value),
					m_param_values[1].value,
					m_param_values[2].value);
				break;

			case RIG_TOKEN_NOTE_ON:
			case RIG_TOKEN_NOTE_OFF:
			case RIG_TOKEN_ALL_NOTES_OFF:
				break;

			case RIG_TOKEN_FTP_SENSITIVITY:
				display(dbg_calls,"ftpSensitivity()",0);
				the_system.startWindow(&win_ftp_sensitivity);
				break;
			case RIG_TOKEN_FTP_TUNER:
				display(dbg_calls,"ftpTuner()",0);
				the_system.startWindow(&win_ftp_tuner);
				break;

			case RIG_TOKEN_LOAD_RIG:
				display(dbg_calls,"loadRig(%s)",
					m_param_values[0].text);
				ok = rigMachine::loadRig(m_param_values[0].text);
				break;

			case RIG_TOKEN_END_MODAL:
				display(dbg_calls,"endModal(%d,%d)",
					m_param_values[0].value,
					m_param_values[1].value);
				m_rig_state.values[m_param_values[0].value] = m_param_values[1].value;
				popRig(1);
				break;

			default:
				rig_error("unknown rigStatement: %d=%s",tt,rigTokenToString(tt));
				ok = 0;
				break;
		}
	}

	proc_leave();
	if (ok)
		display(dbg_stmt+2,"executeStatement() finished",0);
	return ok;
}


bool rigMachine::executeStatementList(const rig_t *rig, int statement_num)
{
	bool ok = 1;
	uint16_t offset = rig->statements[statement_num];
	uint16_t last_offset = rig->statements[statement_num+1];

	display(dbg_stmt,"executeStatmentList(%d) from %d to %d (%d bytes)",
		statement_num,
		offset,
		last_offset,
		last_offset-offset);
	proc_entry();

	while (ok && offset < last_offset)
	{
		ok = executeStatement(rig,&offset,last_offset);
	}

	proc_leave();
	display(dbg_stmt+1,"executeStatementList() returning %d",ok);
	return ok;
}


//--------------------------------------------------
// rig events
//--------------------------------------------------

void rigMachine::onMidiCC(const msgUnion &msg)
	// for now, within the language, we will allow
	// output or th given port set the value, thus
	// allowing programs to monitor their own pedals, etc.
{
	if (!m_rig_loaded)
		return;
	uint16_t mask = 1 << msg.port();
	if (!(listen_mask && mask))
		return;

	display(dbg_midi+1,"onMidiCC(0x%02x,%d,0x%02x,0x%02x)",
			msg.port(),
			msg.channel(),
			msg.param1(),
			msg.param2());

	// set the value into any SERIAL Listens for the given CC number
	// with the convention that listening to channel 0 accepts all channels

	for (int num=0; num<RIG_NUM_LISTENS; num++)
	{
		rigListen_t *listen = &m_rig_state.listens[num];
		if (listen->active &&
			listen->port == msg.port() &&
			listen->cc == msg.param1() && (
			listen->channel == MIDI_OMNI_CHANNEL ||		// MIDI_OMNI_CHANNEL == 0
			listen->channel == msg.channel()))
		{
			display(dbg_midi,"onMidiCC(0x%02x,%d,0x%02x) --> setting value(%d) to 0x%02x",
				msg.port(),
				msg.channel(),
				msg.param1(),
				num,
				msg.param2());
			m_rig_state.values[num] = msg.param2();
		}
	}
}


void rigMachine::onButton(int row, int col, int event)
{
	if (!m_rig_loaded)
		return;

	int num = row * NUM_BUTTON_COLS + col;

	display(dbg_rig,"onButton(%d) 0x%04x",num,event);
	proc_entry();

	const rig_t *context;
	const uint16_t *refs = inheritButtonRefs(num,&context);

	// process the button

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
		{
			executeStatementList(context,ref-1);
		}
	}

	proc_leave();
	display(dbg_rig,"onButton(%d) 0x%04x finished",num,event);
}



//--------------------------------------------------
// updateUI
//--------------------------------------------------

void rigMachine::updateUI()
{
	if (!m_rig_loaded)
		return;

	// evaulate and act on any defined button color or blink expression
	// if a button has a color expression, evaluate it and set it to the button.
	// and then if it has a blink, evaluate that, and set that into the button

	evalResult_t rslt;
	for (int num=0; num<NUM_BUTTONS; num++)
	{
		const rig_t *context;
		const uint16_t *refs = inheritButtonRefs(num,&context);
		uint16_t ref = refs[SUBSECTION_NUM(RIG_TOKEN_COLOR)];

		if (ref)		// REFS[0] IS COLOR !!!
		{
			if (evalCodeExpression(context, &rslt, "LED_COLOR", ref))
			{
				// these checks are only to generate change debug messags
				if (theButtons.getButtonColor(num) != LED_COLORS[rslt.value])
				{
					display(dbg_btns,"rigMachine calling setButtonColor(%d) to index %d=0x%06x  old=0x%06x",
						num,
						rslt.value,
						LED_COLORS[rslt.value],
						theButtons.getButtonColor(num));
					theButtons.setButtonColor(num,LED_COLORS[rslt.value]);
				}

				ref = refs[SUBSECTION_NUM(RIG_TOKEN_BLINK)];
				if (ref)
				{
					if (evalCodeExpression(context, &rslt, "BLINK", ref))
					{
						if (theButtons.getButtonBlink(num) != rslt.value)
						{
							display(dbg_btns,"rigMachine calling setButtonBlink(%d,%d)",
								num,
								rslt.value);
							theButtons.setButtonBlink(num,rslt.value);
						}
					}
				}
			}
		}
	}
}
