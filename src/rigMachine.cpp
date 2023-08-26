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
#include "midiQueue.h"	// for send methods
#include "pedals.h"
#include "rotary.h"


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


rigMachine rig_machine;


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

		int mask = (i == THE_SYSTEM_BUTTON) ? BUTTON_EVENT_LONG_CLICK : 0;
		for (uint16_t j=RIG_TOKEN_PRESS; j<=RIG_TOKEN_REPEAT && ok; j++)
		{
			uint16_t ref = rig_header.button_refs[i][j - RIG_TOKEN_COLOR];
			if (ref)
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
		// if (mask || rig_header.overlay_type == EXP_RIG_TYPE_BASERIG)
		{
			display(dbg_rig+1,"setButton(%d,0x%04x)",i,mask);
			theButtons.setButtonType(i,mask);
		}
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
		display(dbg_param+2,"evalExpression(%s) returning(%s) at offset %d",what,rslt->text,*offset);
	else
		display(dbg_param+2,"evalExpression(%s) returning(%d) at offset %d",what,rslt->value,*offset);
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


//---------------------------------------------------
// Statement execution
//---------------------------------------------------

bool rigMachine::evalParam(evalResult_t *rslt, int arg_type, const uint8_t *code, uint16_t *offset)
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
			ok = evalCodeExpression(rslt,argTypeToString(arg_type),*ptr16);
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
			ok = evalCodeExpression(rslt,argTypeToString(arg_type),*ptr16);
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
			ok = evalCodeExpression(rslt,argTypeToString(arg_type),*ptr16);
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


bool rigMachine::executeStatement(uint16_t *offset, uint16_t last_offset)
{
	const uint8_t *code = cur_rig_code->statement_pool;
	uint8_t tt = code[(*offset)++];

	display(dbg_stmt+1,"executeStatement(%d=%s) at offset %d",tt,rigTokenToText(tt),*offset - 1);
	proc_entry();

	const statement_param_t *params = findParams(tt);
	bool ok = params;

	int param_num = 0;
	const int *arg = params->args;
	while (ok && *arg)
	{
		ok = evalParam(&m_param_values[param_num++],*arg++,code,offset);
	}

	if (ok)
	{
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
				m_rig_state.areas[m_param_values[0].value].font_size = m_param_values[1].value;
				m_rig_state.areas[m_param_values[0].value].font_type = m_param_values[2].value;
				m_rig_state.areas[m_param_values[0].value].font_just = m_param_values[3].value;
				m_rig_state.areas[m_param_values[0].value].xs   	 = m_param_values[4].value;
				m_rig_state.areas[m_param_values[0].value].ys   	 = m_param_values[5].value;
				m_rig_state.areas[m_param_values[0].value].xe   	 = m_param_values[6].value;
				m_rig_state.areas[m_param_values[0].value].ye   	 = m_param_values[7].value;
				break;

			case RIG_TOKEN_LISTEN:
				display(dbg_calls,"LISTEN(%d,%s,%d,%d)",
					m_param_values[0].value,
					rigTokenToText(m_param_values[1].value + RIG_TOKEN_USB1),
					m_param_values[2].value,
					m_param_values[3].value);
				m_rig_state.listens[m_param_values[0].value].active  = 1;
				m_rig_state.listens[m_param_values[0].value].port    = m_param_values[1].value;
				m_rig_state.listens[m_param_values[0].value].channel = m_param_values[2].value;
				m_rig_state.listens[m_param_values[0].value].cc      = m_param_values[3].value;
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
			case RIG_TOKEN_FTP_TUNER:
			case RIG_TOKEN_FTP_SENSITIVITY:
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


bool rigMachine::executeStatementList(int statement_num)
{
	bool ok = 1;
	uint16_t offset = cur_rig_header->statements[statement_num];
	uint16_t last_offset  = cur_rig_header->statements[statement_num+1];

	display(dbg_stmt,"executeStatmentList(%d) from %d to %d (%d bytes)",
		statement_num,
		offset,
		last_offset,
		last_offset-offset);
	proc_entry();

	while (ok && offset < last_offset)
	{
		ok = executeStatement(&offset,last_offset);
	}

	proc_leave();
	display(dbg_stmt+1,"executeStatementList() returning %d",ok);
	return ok;
}


//--------------------------------------------------
// rig events
//--------------------------------------------------

void rigMachine::onMidiCC(int port, int channel, int cc_num, int value)
	// port is an enum, and channel is one based
{
	display(dbg_midi+1,"onMidiCC(%d,%d,%d,%d)",port,channel,cc_num,value);

	// set the value into any SERIAL Listens for the given CC number
	// with the convention that listening to channel 0 accepts all channels

	for (int num=0; num<RIG_NUM_LISTENS; num++)
	{
		rigListen_t *listen = &m_rig_state.listens[num];
		if (listen->active &&
			listen->port == port &&
			listen->cc == cc_num && (
			listen->channel == MIDI_OMNI_CHANNEL ||		// MIDI_OMNI_CHANNEL == 0
			listen->channel == channel))
		{
			display(dbg_midi,"onMidiCC(%d,%d,%d,%d) --> setting value(%d) to 0x%02x",
				port,
				channel,
				cc_num,
				value,
				num,
				value);
			m_rig_state.values[num] = value;
		}
	}
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
		{
			executeStatementList(ref-1);
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
	// evaulate and act on any defined button color or blink expression
	// if a button has a color expression, evaluate it and set it to the button.
	// and then if it has a blink, evaluate that, and set that into the button

	evalResult_t rslt;
	for (int num=0; num<NUM_BUTTONS; num++)
	{
		uint16_t *refs = rig_header.button_refs[num];
		uint16_t ref = refs[SUBSECTION_NUM(RIG_TOKEN_COLOR)];
		if (ref)
		{
			if (evalCodeExpression(&rslt, "LED_COLOR", ref))
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
					if (evalCodeExpression(&rslt, "BLINK", ref))
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
