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


#define AUTO_START_RIGS	 1


#define dbg_rig 	0
	// 0 = show loadRig() and onButton() calls
	// -1 = show rig setButtonType() calls
#define dbg_stmt    1
	// 0  = show executeStatementList()
	// -1 = show individual Statements
#define dbg_calls	0
	// 0 = show functions actually called by statements
	// -1 = show functions called during update cycle
#define dbg_display  0
	// 0 = show rigDisplay() details
#define dbg_meter	0
	// 0 = show calls (only when it changes)
	//-1 = show details (only shen it changes)
#define dbg_param	1
	// 0  = just show final param value
	// -1 = show param details and expression header
	// -2 = show expression return value
#define dbg_midi    0
	// 0  = show onMidiCC setting of Listen Values
	// -1 = show onMidiCC header
	// -2 = show onMidiCC checking each listen
#define dbg_refs	1
	// show calls to inheritRefs


//------------------------------------
// the rig
//------------------------------------

#define LOAD_STATE_NONE			0
#define LOAD_STATE_LOADING  	1
#define LOAD_STATE_ERR_PARSE    2
#define LOAD_STATE_ERR_START 	3

rigMachine rig_machine;

static int in_update_cycle = 0;

static const char *NULL_STRING = "";

// available to rigEval.cpp

int rig_button_num = -1;
uint16_t eval_line_num;
uint16_t eval_param_num;


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
// rig Stack
//----------------------------------------

void rigMachine::showRigName()
{
	const char *use_name = m_stack[m_stack_ptr-1].name;

	warning(0,"showName(%s)",use_name);
	if (!strcmp(use_name,DEFAULT_RIG_TOKEN))
		use_name = DEFAULT_RIG_NAME;
	else if (!strcmp(use_name,DEFAULT_MODAL_TOKEN))
		use_name = DEFAULT_MODAL_NAME;

	strcpy(m_rig_title,use_name);
	the_system.setTitle(m_rig_title,true);
}


bool rigMachine::pushRig(const rig_t *rig, const char *name)
{
	if (m_stack_ptr >= MAX_RIG_STACK)
	{
		rig_error(0,0,"RIG_STACK OVERFLOW!!");
		return 0;
	}
	display(dbg_rig,"pushRig(%d,%s) rig_pool_len=%d",m_stack_ptr,name,rig_pool_len);
	m_stack[m_stack_ptr].rig = rig;
	m_stack[m_stack_ptr].pool_top = rig_pool_len;
	strcpy(m_stack[m_stack_ptr].name,name);
	m_stack_ptr++;
	return 1;
}

void rigMachine::popRig()
{
	if (!m_stack_ptr)
	{
		my_error("RIG_STACK_UNDERFLOW!!",0);
		return;
	}

	const char *old_name = m_stack[m_stack_ptr-1].name;
	display(dbg_rig,"popRig(%d,%s) pool_top=%d",m_stack_ptr-1,old_name,m_stack[m_stack_ptr-1].pool_top);
	proc_entry();

	// cancel buttons

	the_buttons.clear();

	m_stack_ptr--;
	if (m_stack_ptr)
	{
		rig_pool_len = m_stack[m_stack_ptr-1].pool_top;
		display(dbg_rig,"    popRig() new rig_pool_len=%d",rig_pool_len);
		showRigName();
		if (startRig(m_stack[m_stack_ptr-1].rig,0))
		{
			m_load_state = LOAD_STATE_NONE;
			m_rig_loaded = true;							// it is now running
		}
		else
			m_load_state = LOAD_STATE_ERR_START;		// pop another one
	}
	else	// we are popping back to the system
	{
		rig_pool_len = 0;
		display(dbg_rig,"    popRig() empty stack rig_pool_len=%d",rig_pool_len);
		initRotary();
		the_pedals.init();
		the_system.setTitle("",true);
		the_buttons.setButtonType(THE_SYSTEM_BUTTON,BUTTON_EVENT_LONG_CLICK,LED_PURPLE);
	}

	proc_leave();
	display(dbg_rig,"popRig(%d,%s) finished",m_stack_ptr,old_name);
}


const uint16_t *rigMachine::inheritButtonRefs(int num, const rig_t **ret_context)
{
	int use_ptr = m_stack_ptr;
	const rig_t *context = m_stack[use_ptr-1].rig;
	uint16_t type = context->button_type[num];
	const uint16_t *refs = context->button_refs[num];

	while (type == BUTTON_TYPE_INHERIT)
	{
		display(dbg_refs,"rig(%d,%s) updateUI(%d) INHERITING from rig(%d,%s)",
			use_ptr-1,
			m_stack[use_ptr-1].name,
			num,
			use_ptr-2,
			m_stack[use_ptr-2].name);
		use_ptr--;
		context = m_stack[use_ptr-1].rig;
		type = context->button_type[num];
		refs = context->button_refs[num];
	}

	*ret_context = context;
	return refs;
}



//------------------------------------------------------
// loadRig(), startRig(), && restartRig()
//-------------------------------------------------------

void rigMachine::invalidateRig()
{
	if (m_load_state)
	{
		warning(0,"Attempt to call invalidateRig() in load_state(%d)",m_load_state);
	}
	m_load_state = LOAD_STATE_NONE;
	m_stack_ptr = 0;
	m_rig_loaded = 0;
	eval_line_num = 0;
	eval_param_num = 0;
}


bool rigMachine::loadRig(const char *rig_name)
{
	eval_line_num = 0;
	eval_param_num = 0;

	if (m_load_state)
	{
		my_error("Attempt to call loadRig() in load_state(%d)",m_load_state);
		return false;
	}
	if (strlen(rig_name) > MAX_RIG_NAME)
	{
		rig_error(0,0,"loadRig() rig_name too long(%d)",strlen(rig_name));
		return false;
	}

	// stop the machine from processing the current rig, if any

	m_rig_loaded = 0;

	// parse the rig
	// on any errors we assume an error is on the
	// screen, set the error state, and return false.

	display(dbg_rig,"loadRig(%s)",rig_name);
	proc_entry();
	m_load_state = LOAD_STATE_LOADING;
	const rig_t *rig = parseRig(rig_name);

	if (!rig)
	{
		m_load_state = LOAD_STATE_ERR_PARSE;
		proc_leave();
		return false;
	}

	// push the rig
	// a base rig will always succeed in pushing
	// and invalidates any other rigs.

	bool base_rig = !(rig->rig_type & RIG_TYPE_MODAL);
	if (base_rig)
		m_stack_ptr = 0;
	if (!pushRig(rig,rig_name))
	{
		m_load_state = LOAD_STATE_ERR_PARSE;
		proc_leave();
		return false;
	}

	// At this point we need to pop the rig off
	// the stack if start fails, BUT we can't
	// pop it off until the error dialog is put away,
	// so we set LOAD_STATE_ERR_START.

	if (!startRig(rig,base_rig))
	{
		m_load_state = LOAD_STATE_ERR_START;
		proc_leave();
		return false;
	}

	// if it started ok, then we are finished

	m_load_state = LOAD_STATE_NONE;
	showRigName();
	m_rig_loaded = 1;	// starts running now!
	proc_leave();
	return true;
}



bool rigMachine::startRig(const rig_t *rig, bool cold)
{
	display(dbg_rig,"startRig(%d)",cold);
	proc_entry();

	eval_line_num = 0;
	eval_param_num = 0;

	if (cold)
	{
		m_listen_mask = 0;
		memset(&m_rig_state,0,sizeof(rigState_t));
	}

	for (int i=0; i<RIG_NUM_AREAS; i++)
	{
		m_rig_state.areas[i].last_display_color = -1;
		m_rig_state.areas[i].last_display_value = -1;
	}

	// clear the client area

	fillRect(client_rect,TFT_BLACK);

	// process the init section statements

	bool ok = executeStatementList(rig,0);

	// setup the buttons

	if (ok)
	{
		for (int button_num=0; button_num<NUM_BUTTONS; button_num++)
		{
			const rig_t *context;
			inheritButtonRefs(button_num,&context);
			uint16_t type = context->button_type[button_num];

			// invariantly add the long click for the system button
			// in the base rig

			uint16_t mask = (button_num == THE_SYSTEM_BUTTON) ? // && !(context->rig_type & RIG_TYPE_MODAL) ?
				BUTTON_EVENT_LONG_CLICK : 0;

			// display only button type not implemented yet

			if (type & BUTTON_TYPE_CLICK)  	mask |= BUTTON_EVENT_CLICK;
			if (type & BUTTON_TYPE_LONG) 	mask |= BUTTON_EVENT_LONG_CLICK;
			if (type & BUTTON_TYPE_REPEAT) 	mask |= BUTTON_EVENT_PRESS | BUTTON_MASK_REPEAT;
			if (type & BUTTON_TYPE_PRESS) 	mask |= BUTTON_EVENT_PRESS;
			if (type & BUTTON_TYPE_RELEASE)	mask |= BUTTON_EVENT_RELEASE;

			display(dbg_rig+1,"setButton(%d,0x%04x) type(0x%04x)",button_num,mask,type);
			the_buttons.setButtonType(button_num,mask,
				button_num == THE_SYSTEM_BUTTON ? LED_PURPLE : LED_BLACK);
		}
	}

	proc_leave();
	display(dbg_rig,"startRig() returning  %d",ok);
	return ok;
}


void rigMachine::restartRig()
	// called when the last modal window pops off
{
	if (!m_stack_ptr)
	{
		warning(0,"attempt to restart rig when none on stack!",0);
		return;
	}
	if (!m_rig_loaded)
	{
		warning(0,"attempt to restart rig when none loaded!",0);
		return;
	}

	m_rig_loaded = false;    	// stop execution
	display(dbg_rig,"restartRig(%d,%s)",m_stack_ptr,m_stack[m_stack_ptr-1].name);
	showRigName();
	if (startRig(m_stack[m_stack_ptr-1].rig,0))
		m_rig_loaded = true;	// restart execution
	else
		m_load_state = LOAD_STATE_ERR_START;
}



//-------------------------------------------------
// rig statement primitives
//-------------------------------------------------

void rigMachine::rigDrawMeter(int area_num, uint16_t color_idx, int value, int last_value)
{
	rigArea_t *area = &m_rig_state.areas[area_num];
	if (area->last_display_color != color_idx)
		last_value = -1;
	if (value == last_value)
		return;

	proc_entry();
	display(dbg_meter,"rigDrawMeter(%d,%d=%s,%d) area(%d,%d,%d,%d) last=%d",
		area_num,
		color_idx,
		rigTokenToText(color_idx + RIG_TOKEN_DISPLAY_BLACK),
		value,
		area->xs,
		area->ys,
		area->xe,
		area->ye,
		last_value);

	if (last_value == -1)
	{
		mylcd.fillRect(
			client_rect.xs + area->xs,
			client_rect.ys + area->ys,
			area->xe - area->xs + 1,		// width
			area->ye - area->ys + 1,        // height
			TFT_BLACK);						// TFT_BLUE helps see details
		mylcd.setDrawColor(TFT_WHITE);
		mylcd.drawRect(
			client_rect.xs + area->xs,
			client_rect.ys + area->ys,
			client_rect.xs + area->xe,		// end x
			client_rect.ys + area->ye);		// end y
	}

	// normalize last_value

	if (last_value == -1)
		last_value = 0;

	// if the value has not changed, there's nothing to do

	uint32_t from = last_value;
	uint32_t to = value;
	display(dbg_meter+1,"    initial from(%d) to(%d)",from,to);

	if (from != to)		// value changed
	{
		uint16_t color = DISPLAY_COLORS[color_idx];
		if (value < last_value)
		{
			display(0,"    color BLACK",0);
			color = TFT_BLACK;
		}

		// convert to pixels
		// this will generally expand the range
		// so we don't check again for from==to

		uint32_t extent = area->type == AREA_TYPE_HMETER ?
			area->xe - area->xs - 2 :
			area->ye - area->ys - 2 ;

		from = (from * extent) / 127;
		to =  (to * extent) / 127;
		display(dbg_meter+1,"    pixels from(%d) to(%d) extent(%d)",from,to,extent);

		// switch from-to as needed

		if (from > to)
		{
			uint32_t temp = from;
			from = to;
			to = temp;
			display(dbg_meter+1,"    switch from(%d) to(%d)",from,to);
		}

		// draw horizontal or vertical
		// note that vertical coords are reversed as lcd start at top
		// and that it's all 1 pixel inside the frame, but that extent
		// already knew that.

		if (area->type == AREA_TYPE_HMETER)
		{
			display(dbg_meter+1,"    fillRect(%d,%d,%d,%d)",
				client_rect.xs + area->xs + 1 + from,
				client_rect.ys + area->ys + 1,
				to - from + 1,
				area->ye - area->ys - 1);
			mylcd.fillRect(
				client_rect.xs + area->xs + 1 + from,
				client_rect.ys + area->ys + 1,
				to - from + 1,						// width
				area->ye - area->ys - 1,			// height
				color);
		}
		else
		{
			display(dbg_meter+1,"    fillRect(%d,%d,%d,%d)",
				client_rect.xs + area->xs + 1,
				client_rect.ys + area->ye - to - 1,
				area->xe - area->xs - 1,
				to - from + 1);
			mylcd.fillRect(
				client_rect.xs + area->xs + 1,
				client_rect.ys + area->ye - to - 1,	// to is on top
				area->xe - area->xs - 1,			// width
				to - from + 1,						// height
				color);
		}
	}

	area->last_display_value = value;
	area->last_display_color = color_idx;
	proc_leave();
}


static bool setAreaFont(int font_type, int font_size)
{
	const ILI9341_t3_font_t *font = 0;
	if (font_type)
	{
		switch (font_size)
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
			default:	// should never happen
				rig_error(1,0,"Unknown font_size(%d)_bold",font_type);
				return false;
		}
	}
	else
	{
		switch (font_size)
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
			default: 	// should never happen
				rig_error(1,0,"Unknown font_size(%d)",font_type);
				return false;
		}
	}
	mylcd.setFont(*font);
	return true;

}


void rigMachine::rigDisplayNumber(int area_num, uint16_t color_idx, int value, int last_value)
{
	rigArea_t *area = &m_rig_state.areas[area_num];
	if (area->last_display_color != color_idx)
		last_value = -1;
	if (value == last_value)
		return;

	proc_entry();
	display(dbg_display,"rigDisplayNumber(%d,%d=%s,%d) last=%d",
		area_num,
		color_idx,
		rigTokenToText(color_idx + RIG_TOKEN_DISPLAY_BLACK),
		value,
		last_value);

	if (!setAreaFont(area->font_type,area->font_size))
	{
		proc_leave();
		return;
	}

	mylcd.printfJustified(
		client_rect.xs + area->xs,
		client_rect.ys + area->ys,
		area->xe - area->xs + 1,			// width
		area->ye - area->ys + 1,            // height
		area->font_just,
		DISPLAY_COLORS[color_idx],
		TFT_BLACK,
		1,	// use bc
		"%d",
		value);

	area->last_display_value = value;
	area->last_display_color = color_idx;

	proc_leave();
}



void rigMachine::rigDisplay(uint16_t area_num, uint16_t color_idx, const char *text)
{
	rigArea_t *area = &m_rig_state.areas[area_num];

	proc_entry();
	display(dbg_display,"rigDisplay(%d,%d=%s,\"%s\")",
		area_num,
		color_idx,
		rigTokenToText(color_idx + RIG_TOKEN_DISPLAY_BLACK),
		text);

	if (!setAreaFont(area->font_type,area->font_size))
	{
		proc_leave();
		return;
	}

	mylcd.printJustified(
		client_rect.xs + area->xs,
		client_rect.ys + area->ys,
		area->xe - area->xs + 1,		// width
		area->ye - area->ys + 1,        // height
		area->font_just,
		DISPLAY_COLORS[color_idx],
		TFT_BLACK,
		1,	// use bc
		text);
	proc_leave();
}



//-------------------------------------------------------------------
// Statement Expressions
//-------------------------------------------------------------------

bool rigMachine::evalExpression(const rig_t *rig, evalResult_t *rslt, const char *what, const uint8_t *code, uint16_t *offset)
{
	display(dbg_param+1,"evalExpression(%s) at offset %d",what,*offset);
	proc_entry();

	if (!expression(rig, rslt,code,offset))
	{
		my_error("evalExpression(%s) failed at offset %d",what,*offset);
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


bool rigMachine::evalCodeExpression(const rig_t *rig, evalResult_t *rslt, const char *what, uint16_t offset, uint16_t stmt_offset)
{
	if (offset & (EXP_INLINE << 8))
	{
		uint8_t byte0 = (offset >> 8);
		uint8_t opcode = byte0 & ~(EXP_INLINE | EXP_INLINE_BUTTON);
		uint8_t value = offset & 0xff;

		// turn embedded EXP_INLINE_BUTTONS into their
		// proper rig_button_number equivilant before
		// falling through to the other things.

		if (byte0 & EXP_INLINE_BUTTON)
		{
			uint16_t new_value = rig_button_num;
			if (value == EXP_BUTTON_ROW)
				new_value = rig_button_num / NUM_BUTTON_COLS;
			else if (value == EXP_BUTTON_COL)
				new_value = rig_button_num % NUM_BUTTON_COLS;
			else
				new_value = rig_button_num;
			display(dbg_param+1,"INLINE_BUTTON(0x%02x) rig_button_num=%d  new_value=%d",value,rig_button_num,new_value);
			value = new_value;
		}

		if (opcode == EXP_LED_COLOR)
		{
			display(dbg_param+1,"inline LED_COLOR(%d) = %s",value,rigTokenToText(RIG_TOKEN_LED_BLACK + value));
			rslt->value = value;
		}
		else if (opcode == EXP_DISPLAY_COLOR)
		{
			display(dbg_param+1,"inline DISPLAY_COLOR(%d) = %s",value,rigTokenToText(RIG_TOKEN_DISPLAY_BLACK + value));
			rslt->value = value;
		}
		else if (opcode == EXP_VALUE)
		{
			display(dbg_param+1,"inline VALUE[%d] = %d",value,m_rig_state.values[value]);
			rslt->value = m_rig_state.values[value];
		}
		else if (opcode == EXP_STRING)
		{
			display(dbg_param+1,"inline STRING[%d]",value);
			uint16_t off = rig->strings[value];
			rslt->text = &rig->string_pool[off];
			rslt->is_string = 1;
		}
		else if (opcode == EXP_NUMBER)
		{
			display(dbg_param+1,"inline NUMBER = %d",value);
			rslt->value = value;
		}
		else if (opcode == EXP_TEXT)
		{
			if (!value)
			{
				display(dbg_param+1,"inline NULL TEXT",0);
				rslt->text = NULL_STRING;
				rslt->is_string = 1;

			}
			else
			{
				rig_error(1,0,"unexpected non-null inline TEXT value(0x%02x)",value);
				return false;
			}
		}
		else
		{
			rig_error(1,0,"unknown inline opcode(0x%02x)",opcode);
			return false;
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
// Parameters and Statements
//---------------------------------------------------

bool rigMachine::evalParam(const rig_t *rig, evalResult_t *rslt, int arg_type, const uint8_t *stmts, uint16_t *stmt_offset)
{
	const char *what = argTypeToString(arg_type);
	display(dbg_param+1,"evalParam(%d,%s) at stmt_offset %d",eval_param_num,what,*stmt_offset);
	proc_entry();

	bool ok = 1;
	uint16_t max = 0;
	uint16_t *ptr16 = 0;
	rslt->is_string = 0;

	switch (arg_type)
	{
		case PARAM_AREA_NUM :		max = RIG_NUM_AREAS - 1;	break;
		case PARAM_PEDAL_NUM :		max = NUM_PEDALS - 1;		break;
		case PARAM_ROTARY_NUM :		max = NUM_ROTARY - 1;		break;
		case PARAM_VALUE_NUM :		max = RIG_NUM_VALUES - 1;	break;
		case PARAM_VALUE :			max = MAX_RIG_VALUE;		break;
		case PARAM_LISTEN_CHANNEL :	max = MIDI_MAX_CHANNEL;		break;
		case PARAM_MIDI_CC :		max = MIDI_MAX_VALUE;		break;
		case PARAM_MIDI_VALUE :		max = MIDI_MAX_VALUE;		break;

	}

	switch (arg_type)
	{
		case PARAM_BUTTON_NUM :
			// PARAM button numbers are a single byte and are either
			// EXP_BUTTON_NUMBER or the button number number itself.
			if (stmts[*stmt_offset] == EXP_BUTTON_NUM)
				rslt->value = rig_button_num;
			else
				rslt->value = stmts[*stmt_offset];
			(*stmt_offset)++;
			break;

		case PARAM_METER_TYPE :
		case PARAM_FONT_TYPE :
		case PARAM_FONT_JUST :
		case PARAM_FONT_SIZE :
		case PARAM_MIDI_PORT :
		case PARAM_LISTEN_DIR :
			rslt->value = stmts[(*stmt_offset)++];
			break;
		case PARAM_END_X :
		case PARAM_END_Y :
		case PARAM_START_X :
		case PARAM_START_Y :
			ptr16 = (uint16_t *) &stmts[*stmt_offset];
			rslt->value = *ptr16;
			*stmt_offset += 2;
			break;

		case PARAM_RIG_NAME :
		case PARAM_PEDAL_NAME :
			ptr16 = (uint16_t *) &stmts[*stmt_offset];
			rslt->text = &rig->string_pool[*ptr16];
			rslt->is_string = 1;
			*stmt_offset += 2;
			break;

		case PARAM_MIDI_CHANNEL :
			ptr16 = (uint16_t *) &stmts[*stmt_offset];
			ok = evalCodeExpression(rig,rslt,argTypeToString(arg_type),*ptr16,*stmt_offset);
			if (ok)
			{
				if (rslt->value > MIDI_MAX_CHANNEL ||
					rslt->value < MIDI_MIN_CHANNEL)
				{
					uint16_t actual_code = *ptr16 & (EXP_INLINE << 8) ? *ptr16 : *ptr16-1;
						// show inline opcode or stmt_offset-1 (actual expression offset) for debugging
					rig_error(1,0,"code(0x%04x) - %s(%d) must be between %d and %d",
						actual_code,what,rslt->value,MIDI_MIN_CHANNEL,MIDI_MAX_CHANNEL);
					ok = 0;
				}
				else
				{
					*stmt_offset += 2;
				}
			}
			break;

		case PARAM_AREA_NUM :
		case PARAM_VALUE_NUM :
		case PARAM_VALUE :
		case PARAM_PEDAL_NUM :
		case PARAM_ROTARY_NUM :
		case PARAM_LISTEN_CHANNEL :
		case PARAM_MIDI_CC :
		case PARAM_MIDI_VALUE :
			ptr16 = (uint16_t *) &stmts[*stmt_offset];
			ok = evalCodeExpression(rig,rslt,argTypeToString(arg_type),*ptr16,*stmt_offset);
			if (ok)
			{
				if (rslt->value > max)
				{
					uint16_t actual_code = *ptr16 & (EXP_INLINE << 8) ? *ptr16 : *ptr16-1;
						// show inline opcode or stmt_offset-1 (actual expression offset) for debugging
					rig_error(1,0,"code(0x%04x) - %s(%d) must be %d or less",
						actual_code, what,rslt->value,max);
					ok = 0;
				}
				else
				{
					*stmt_offset += 2;
				}
			}
			break;

		case PARAM_STRING_EXPRESSION :
		case PARAM_LED_COLOR_EXPRESSION :
		case PARAM_DISPLAY_COLOR_EXPRESSION :
			ptr16 = (uint16_t *) &stmts[*stmt_offset];
			ok = evalCodeExpression(rig,rslt,argTypeToString(arg_type),*ptr16,*stmt_offset);
			if (ok)
				*stmt_offset += 2;
			break;

		default:
			rig_error(1,0,"unknown arg_type(%d)",arg_type);
			ok = 0;
			break;
	}

	proc_leave();
	if (ok)
	{
		if (rslt->is_string)
			display(dbg_param,"evalParam(%s) returning(%s) at stmt_offset %d",argTypeToString(arg_type),rslt->text,*stmt_offset);
		else
			display(dbg_param,"evalParam(%s) returning(%d) at stmt_offset %d",argTypeToString(arg_type),rslt->value,*stmt_offset);
	}
	return ok;
}



bool rigMachine::executeStatement(const rig_t *rig, uint16_t *stmt_offset, uint16_t last_offset)
{
	const uint8_t *stmts = rig->statement_pool;
	uint8_t tt = stmts[(*stmt_offset)++];

	eval_line_num = *((uint16_t *) &stmts[*stmt_offset]);
	display(dbg_stmt+1,"executeStatement(%d=%s) at line(%d) offset(%d)",tt,rigTokenToText(tt),eval_line_num,*stmt_offset - 3);
	*stmt_offset += 2;

	proc_entry();

	const statement_param_t *params = findParams(tt);
	bool ok = params;

	eval_param_num = 0;
	const int *arg = params->args;
	while (ok && *arg)
	{
		ok = evalParam(rig,&m_params[eval_param_num],*arg++,stmts,stmt_offset);
		eval_param_num++;
	}

	if (ok)
	{
		uint16_t idx = 0;
		switch (tt)
		{
			case RIG_TOKEN_LISTEN:
			{
				int inout = m_params[2].value;
				display(dbg_calls,"LISTEN(%d,%s,%s,%d,%d)",
					m_params[0].value,
					rigTokenToText(m_params[1].value + RIG_TOKEN_USB1),
					rigTokenToText(inout + RIG_TOKEN_INPUT),
					m_params[3].value,
					m_params[4].value);
				idx = m_params[0].value;
				m_rig_state.listens[idx].active  = 1;
				m_rig_state.listens[idx].port    = m_params[1].value;
				m_rig_state.listens[idx].channel = m_params[3].value;
				m_rig_state.listens[idx].cc      = m_params[4].value;
				if (inout == 0 || inout == 2)
					m_listen_mask |= 1 << m_params[1].value;
				if (inout == 1 || inout == 2)
					m_listen_mask |= 1 << (m_params[1].value + 16);
				// display(dbg_calls,"m_listen_mask=0x%08x",m_listen_mask);
				break;
			}

			case RIG_TOKEN_LISTEN_RANGED:
			{
				int range 	   = m_params[0].value;
				int base_value = m_params[1].value;
				int port 	   = m_params[2].value;
				int inout	   = m_params[3].value;
				int channel    = m_params[4].value;
				int base_cc    = m_params[5].value;

				display(dbg_calls,"LISTEN_RANGED(%d,%d,%d=%s,%d=%s,%d,%d)",
					range,
					base_value,
					port,
					rigTokenToText(port + RIG_TOKEN_USB1),
					inout,
					rigTokenToText(inout + RIG_TOKEN_INPUT),
					channel,
					base_cc);

				if (inout == 0 || inout == 2)
					m_listen_mask |= 1 << m_params[1].value;
				if (inout == 1 || inout == 2)
					m_listen_mask |= 1 << (m_params[1].value + 16);;

				for (idx=0; idx<range; idx++)
				{
					m_rig_state.listens[base_value + idx].active  = 1;
					m_rig_state.listens[base_value + idx].port    = port;
					m_rig_state.listens[base_value + idx].channel = channel;
					m_rig_state.listens[base_value + idx].cc      = base_cc + idx;
				}
				break;
			}


			case RIG_TOKEN_AREA:
				display(dbg_calls,"AREA(%d,%d,%s,%s,%d,%d,%d,%d)",
					m_params[0].value,
					m_params[1].value,
					rigTokenToText(m_params[2].value + RIG_TOKEN_NORMAL),
					rigTokenToText(m_params[3].value + RIG_TOKEN_LEFT),
					m_params[4].value,
					m_params[5].value,
					m_params[6].value,
					m_params[7].value);
				idx = m_params[0].value;
				m_rig_state.areas[idx].type      = AREA_TYPE_STRING;
				m_rig_state.areas[idx].font_size = m_params[1].value;
				m_rig_state.areas[idx].font_type = m_params[2].value;
				m_rig_state.areas[idx].font_just = m_params[3].value;
				m_rig_state.areas[idx].xs   	 = m_params[4].value;
				m_rig_state.areas[idx].ys   	 = m_params[5].value;
				m_rig_state.areas[idx].xe   	 = m_params[6].value;
				m_rig_state.areas[idx].ye   	 = m_params[7].value;
				break;

			case RIG_TOKEN_METER:
			{
				int area_num = m_params[0].value;
				int type 	 = m_params[1].value;
				display(dbg_calls,"METER(%d,%d=%s,%d,%d,%d,%d)",
					area_num,
					type,
					rigTokenToText(type + RIG_TOKEN_HORZ),
					m_params[2].value,
					m_params[3].value,
					m_params[4].value,
					m_params[5].value);
				m_rig_state.areas[area_num].type = AREA_TYPE_HMETER + type;
				m_rig_state.areas[area_num].xs   = m_params[2].value;
				m_rig_state.areas[area_num].ys   = m_params[3].value;
				m_rig_state.areas[area_num].xe   = m_params[4].value;
				m_rig_state.areas[area_num].ye   = m_params[5].value;
				break;
			}

			case RIG_TOKEN_PEDAL:
				display(dbg_calls,"PEDAL(%d,%s,%d=%s,%d,%d)",
					m_params[0].value,
					m_params[1].text,
					m_params[2].value,
					rigTokenToText(m_params[2].value + RIG_TOKEN_USB1),
					m_params[3].value,
					m_params[4].value);
				the_pedals.setPedal(
					m_params[0].value,			// pedal num
					m_params[1].text,				// name
					MIDI_ENUM_TO_PORT(m_params[2].value),	// port
					m_params[3].value - 1,		// switch to zero based channel number
					m_params[4].value);			// cc
				break;

			case RIG_TOKEN_ROTARY:
				display(dbg_calls,"ROTARY(%d,%s,%d=%s,chan=%d, cc=%d, listen=%d)",
					m_params[0].value,
					m_params[1].text,
					m_params[2].value,
					rigTokenToText(m_params[2].value + RIG_TOKEN_USB1),
					m_params[3].value,
					m_params[4].value,
					m_params[5].value);
				setRotary(
					m_params[0].value,			// rotary num
					m_params[1].text,			// name
					MIDI_ENUM_TO_PORT(m_params[2].value),	// port
					m_params[3].value - 1,		// switch to zero based channel number
					m_params[4].value,			// cc
					m_params[5].value);			// listen

					if (m_params[5].value)
					{
						m_listen_mask |= 1 << m_params[5].value;
					}
				break;

			case RIG_TOKEN_SETVALUE:
				display(dbg_calls+in_update_cycle,"setValue(%d,%d)",
					m_params[0].value,
					m_params[1].value);
				m_rig_state.values[m_params[0].value] = m_params[1].value;
				break;

			case RIG_TOKEN_SETTITLE:
			{
				const char *new_title = m_params[0].text;
				display(dbg_calls+in_update_cycle,"setTitle(%s)",new_title);
				if (strcmp(m_rig_title,new_title))
				{
					int len = strlen(new_title);
					if (len > MAX_RIG_TITLE)
						len = MAX_RIG_TITLE;
					memcpy(m_rig_title,new_title,len);
					m_rig_title[len] = 0;
					the_system.setTitle(m_rig_title,true);
				}
				break;
			}

			case RIG_TOKEN_DISPLAY:
			{
				int area_num = m_params[0].value;
				display(dbg_calls+in_update_cycle,"display(%d,%d=%s,\"%s\")",
					area_num,
					m_params[1].value,
					rigTokenToText(m_params[1].value + RIG_TOKEN_DISPLAY_BLACK),
					m_params[2].text);
				// debug_level = 0;
				if (m_rig_state.areas[area_num].type != AREA_TYPE_STRING)
				{
					rig_error(1,0,"Attempt to display in AREA of type(%d=%s)",rigTokenToString(m_rig_state.areas[area_num].type + RIG_TOKEN_HORZ));
					ok = 0;
				}
				else
				{
					rigDisplay(
						m_params[0].value,
						m_params[1].value,
						m_params[2].text);
				}
				break;
			}

			case RIG_TOKEN_DISPLAY_NUMBER:
			case RIG_TOKEN_SET_METER:
			{
				int area_num = m_params[0].value;
				int color_idx = m_params[1].value;
				int value = m_params[2].value;
				int last_value = m_rig_state.areas[area_num].last_display_value;
				int type = m_rig_state.areas[area_num].type;
				display(dbg_calls+in_update_cycle,"%s(%d,%d=%s,%d) type=%d",
					rigTokenToText(tt),
					area_num,
					color_idx,
					rigTokenToText(color_idx + RIG_TOKEN_DISPLAY_BLACK),
					value,
					type);
				if (tt == RIG_TOKEN_DISPLAY_NUMBER)
				{
					if (type != AREA_TYPE_STRING)
					{
						rig_error(1,0,"Attempt to displayNumber in AREA of type(%d=%s)",rigTokenToString(type + RIG_TOKEN_HORZ));
						ok = 0;
					}
					else
					{
						rigDisplayNumber(area_num,color_idx,value,last_value);
					}
				}
				else
				{
					if (m_rig_state.areas[area_num].type == AREA_TYPE_STRING)
					{
						rig_error(1,0,"Attempt to setMeter in STRING AREA",0);
						ok = 0;
					}
					else
					{
						rigDrawMeter(area_num,color_idx,value,last_value);
					}
				}
				break;
			}

			case RIG_TOKEN_SEND_CC:
				display(dbg_calls,"sendCC(%d=%s,%d,%d,%d)",
					m_params[0].value,
					rigTokenToText(m_params[0].value + RIG_TOKEN_USB1),
					m_params[1].value,
					m_params[2].value,
					m_params[3].value);
				// debug_level = 0;
				sendMidiControlChange(
					MIDI_ENUM_TO_PORT(m_params[0].value),
					m_params[1].value,
					m_params[2].value,
					m_params[3].value);
				break;
			case RIG_TOKEN_SEND_PGM_CHG:
				display(dbg_calls,"sendPgmChg(%d=%s,%d,%d)",
					m_params[0].value,
					rigTokenToText(m_params[0].value + RIG_TOKEN_USB1),
					m_params[1].value,
					m_params[2].value);
				sendMidiProgramChange(
					MIDI_ENUM_TO_PORT(m_params[0].value),
					m_params[1].value,
					m_params[2].value);
				break;

			case RIG_TOKEN_NOTE_ON:
			case RIG_TOKEN_NOTE_OFF:
			case RIG_TOKEN_ALL_NOTES_OFF:
				break;

			case RIG_TOKEN_SET_BUTTON_COLOR:
				display(dbg_calls+in_update_cycle,"setButtonColor(%d,%d=%s)",
					m_params[0].value,
					m_params[1].value,
					rigTokenToText(m_params[1].value + RIG_TOKEN_LED_BLACK));
				the_buttons.setButtonColor(m_params[0].value,LED_COLORS[m_params[1].value]);
				break;
			case RIG_TOKEN_SET_BUTTON_BLINK:
				display(dbg_calls+in_update_cycle,"setButtonBlink(%d,%d)",
					m_params[0].value,
					m_params[1].value);
				the_buttons.setButtonBlink(m_params[0].value,m_params[1].value);
				break;

			case RIG_TOKEN_LOAD_RIG:
				display(dbg_calls,"loadRig(%s)",
					m_params[0].text);
				ok = rigMachine::loadRig(m_params[0].text);
				break;

			case RIG_TOKEN_END_MODAL:
				display(dbg_calls,"endModal(%d,%d)",
					m_params[0].value,
					m_params[1].value);
				m_rig_state.values[m_params[0].value] = m_params[1].value;
				popRig();
				break;
			case RIG_TOKEN_FTP_SENSITIVITY:
				display(dbg_calls,"ftpSensitivity()",0);
				the_system.startWindow(&win_ftp_sensitivity);
				break;
			case RIG_TOKEN_FTP_TUNER:
				display(dbg_calls,"ftpTuner()",0);
				the_system.startWindow(&win_ftp_tuner);
				break;

			default:
				rig_error(1,0,"unknown rigStatement: %d=%s",tt,rigTokenToString(tt));
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
	uint16_t stmt_offset = rig->statements[statement_num];
	uint16_t last_offset = rig->statements[statement_num+1];

	display(dbg_stmt,"executeStatmentList(%d) from %d to %d (%d bytes)",
		statement_num,
		stmt_offset,
		last_offset,
		last_offset-stmt_offset);
	proc_entry();

	while (ok && stmt_offset < last_offset)
	{
		ok = executeStatement(rig,&stmt_offset,last_offset);
	}

	proc_leave();
	display(dbg_stmt+1,"executeStatementList() returning %d",ok);
	return ok;
}


//--------------------------------------------------
// Event Handlers
//--------------------------------------------------

void rigMachine::onMidiCC(const msgUnion &msg)
	// Note that this may be called even when no rig is loaded!
	// (a) midiQueue only sends us message on ports that match our m_listen_mask.
	// (b) we accept output OR input ports
{
	uint32_t mask = 1 << (msg.portEnum() + (msg.isOutput() ? 16 : 0));
	if (!(m_listen_mask & mask))
		return;

	if (dbg_midi <= -1)
		display_level(dbg_midi+1,0,"onMidiCC(0x%02x,%d,0x%02x,0x%02x) isOutput=%d",
			msg.port(),
			msg.channel(),
			msg.param1(),
			msg.param2(),
			msg.isOutput());

	// See if the Rotaries want it

	onRotaryMidiCC(msg);

	// set the value into any SERIAL Listens for the given CC number
	// with the convention that listening to channel 0 accepts all channels

	for (int num=0; num<RIG_NUM_LISTENS; num++)
	{
		rigListen_t *listen = &m_rig_state.listens[num];

		if (listen->active &&
			listen->port == msg.portEnum() &&
			listen->cc == msg.param1() && (
			listen->channel == MIDI_OMNI_CHANNEL ||		// MIDI_OMNI_CHANNEL == 0
			listen->channel == msg.channel()))
		{
			if (dbg_midi <= 0)
				display_level(dbg_midi,2,"onMidiCC(0x%02x,%d,0x%02x) --> setting value(%d) to 0x%02x",
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

	display(dbg_rig,"rigMachine::onButton(%d) 0x%04x",num,event);
	proc_entry();

	const rig_t *context;
	const uint16_t *refs = inheritButtonRefs(num,&context);
	int ref_num = 1;
	// process the button

	if (event == BUTTON_EVENT_LONG_CLICK ||
		event == BUTTON_TYPE_RELEASE)
	{
		ref_num = 2;
	}
	uint16_t ref = refs[ref_num];
	display(dbg_rig+1,"checking button(%d) ref_num(%d) ref(%d)",num,ref_num,ref);
	if (ref)
	{
		rig_button_num = num;
		executeStatementList(context,ref-1);
		rig_button_num = -1;
	}

	proc_leave();
	display(dbg_rig+1,"rigMachine::onButton(%d) 0x%04x finished",num,event);
}



//--------------------------------------------------
// updateUI
//--------------------------------------------------

static bool pref_rig_failed;
static bool default_rig_failed;
static bool update_failed;
	// rigs shall not fail twice

void rigMachine::updateUI()
{

	//---------------------------------------------
	// asynchronous rig loading
	//---------------------------------------------
	// if we encountered an error starting the rig on
	// the top of the stack, pop the bad one off the stack
	// and continue

	if (m_load_state == LOAD_STATE_ERR_START)
	{
		if (m_stack_ptr)
		{
			display(dbg_rig,"LOAD_STATE_ERR_START calling popRig()",0);
			popRig();
		}
		else	// I give up!
		{
			display(dbg_rig,"LOAD_STATE_ERR_START setting LOAD_STATE_NONE",0);
			m_load_state = LOAD_STATE_NONE;
		}
		return;
	}

	// if there was an error parsing a rig, then
	// it was never pushed.  Once we set LOAD_STATE_NONE,
	// the default rig will attempt to be loaded.

	if (m_load_state == LOAD_STATE_ERR_PARSE)
	{
		display(dbg_rig,"LOAD_STATE_ERR_PARSE setting LOAD_STATE_NONE",0);
		m_load_state = LOAD_STATE_NONE;
	}

	#if AUTO_START_RIGS
		// if no rig is loaded, try to load the
		// preferred rig and if that fails try
		// to load the default rig

		if (m_load_state == LOAD_STATE_NONE &&
			!m_rig_loaded &&
			!pref_rig_failed)
		{
			display(dbg_rig,"updateUI() loading preferred rig: %s",prefs.RIG_NAME);
			if (!loadRig(prefs.RIG_NAME))
				pref_rig_failed = 1;
		}

		// This means that if the default rig IS
		// the pref rig, and I have a problem in the
		// default rig, it will try to load it twice

		if (m_load_state == LOAD_STATE_NONE &&
			!m_rig_loaded &&
			!default_rig_failed)
		{
			display(dbg_rig,"updateUI() loading default rig: %s",DEFAULT_RIG_TOKEN);
			if (!loadRig(DEFAULT_RIG_TOKEN))
				default_rig_failed = 1;
		}


	#endif

	// and finally, return if no rig loaded

	if (!m_rig_loaded)
		return;

	//-------------------------------------
	// Update Cycle
	//-------------------------------------
	// for safety we only allow one update error

	if (!update_failed)
	{
		bool ok = 1;
		in_update_cycle = 1;
		const rig_t *rig = m_stack[m_stack_ptr-1].rig;
		if (rig->statements[1] < rig->statements[2])
			ok = executeStatementList(rig,1);
		for (int num=0; num<NUM_BUTTONS && ok; num++)
		{
			const rig_t *context;
			const uint16_t *refs = inheritButtonRefs(num,&context);
			if (refs[0])
			{
				rig_button_num = num;
				ok = executeStatementList(context,refs[0]-1);
				rig_button_num = -1;
			}
		}
		if (!ok)
			update_failed = 1;
		in_update_cycle = 0;
	}

}	// rigMachine::updateUI()


// end of rigMachine.cpp
