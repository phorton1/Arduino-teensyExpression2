//--------------------------------------------------------------
// winConfigPedal.cpp
//--------------------------------------------------------------
// This version does not support auto pedals.
//
// Navigation:
//
//    Upper Right Green - no changes have been made
//    Upper Right Purple - Accepts Pedal Changes and returns
//       to GeneralConfigurationMode, where, BTW, the overall prefs
//       can still be aborted or accepted.
//    Upper Right Orange - Abandons Pedal Changes
//		 ONLY VISIBLE ACTIVE IF THE PEDALS HAVE CHANGED
//    Upper Left Blue - Cycles through Pedals

#include <myDebug.h>
#include "winConfigPedal.h"
#include "myTFT.h"
#include "pedals.h"
#include "buttons.h"

#define dbg_pedal	 0


//----------------------------------
// Pedals
//----------------------------------

#define KEYPAD_UP      		12
#define KEYPAD_DOWN    		22
#define KEYPAD_LEFT    		16
#define KEYPAD_RIGHT   		18
#define KEYPAD_SELECT  		17

#define CANCEL_BUTTON  		3
#define NEXT_PEDAL_BUTTON  	0

#define ITEM_CALIBRATE		0
#define ITEM_CURVE_TYPE		1
#define NUM_FIXED_ITEMS   	2

#define CALIB_SAFETY_MARGIN   30

#define CUR_PEDAL	(prefs.PEDAL[m_pedal_num])
#define CUR_CURVE   (CUR_PEDAL.CURVE_TYPE)
#define CUR_POINTS  (CUR_PEDAL.CURVE[CUR_CURVE].POINTS)
#define NUM_POINTS	(CUR_CURVE + 2)
#define NUM_ITEMS	(NUM_FIXED_ITEMS + NUM_POINTS)


winConfigPedal  win_config_pedal;


winConfigPedal::winConfigPedal() : sysWindow()
{
    m_pedal_num = 0;
}

void winConfigPedal::setPedal(int i)
{
	m_pedal_num = i;
	init();
}


const char *winConfigPedal::name()
{
    static char buf[120];
    sprintf(buf,"Config Pedal %d (%s)",
        m_pedal_num + 1,
        the_pedals.getPedal(m_pedal_num)->getName());
    return buf;
}


void winConfigPedal::init_prev_points()
{
	memset(&m_prev_points,255,NUM_CURVE_POINTS * sizeof(pref_curve_point_t));
}


void winConfigPedal::init()
{
    m_cur_item = 0;
	m_cur_point = -1;    // -1 when not selected for editing
    m_display_item = -1;
    m_display_curve = -1;
    m_display_raw_value = -1;
    m_display_pedal_x = -1;
    m_display_pedal_value = -1;
    m_redraw_curve = 1;
    m_in_calibrate = 0;
	init_prev_points();
}


void winConfigPedal::begin(bool cold)
{
    display(dbg_pedal,"winConfigPedal::begin(%d) pedal=%d",cold,m_pedal_num);
	sysWindow::begin(cold);
	init();

	if (cold)
	{
		m_changed = 0;
		memcpy(m_save_prefs,prefs.PEDAL,NUM_PEDALS * sizeof(pref_pedal_t));
	}

    // m_cur_mode = getPref8(PREF_PEDAL(m_pedal_num) + PREF_PEDAL_MODE_OFFSET);
    // m_cur_curve = getPref8(PREF_PEDAL(m_pedal_num) + PREF_PEDAL_CURVE_TYPE_OFFSET);
    // m_curve_names = getPrefStrings(PREF_PEDAL(m_pedal_num) + PREF_PEDAL_CURVE_TYPE_OFFSET);
    // m_mode_names = getPrefStrings(PREF_PEDAL(m_pedal_num) + PREF_PEDAL_MODE_OFFSET);
    // getPrefPedalPoints();
    // setEditPoints();

	the_buttons.setButtonType(KEYPAD_UP,   		BUTTON_EVENT_PRESS | BUTTON_MASK_REPEAT, LED_BLUE);
	the_buttons.setButtonType(KEYPAD_DOWN,		BUTTON_EVENT_PRESS | BUTTON_MASK_REPEAT, LED_BLUE);
	the_buttons.setButtonType(KEYPAD_LEFT,		BUTTON_EVENT_PRESS | BUTTON_MASK_REPEAT, LED_BLUE);
	the_buttons.setButtonType(KEYPAD_RIGHT,		BUTTON_EVENT_PRESS | BUTTON_MASK_REPEAT, LED_BLUE);
	the_buttons.setButtonType(KEYPAD_SELECT,	BUTTON_EVENT_CLICK, LED_GREEN);
	the_buttons.setButtonType(NEXT_PEDAL_BUTTON,BUTTON_EVENT_CLICK, LED_BLUE);
	the_buttons.setButtonType(THE_SYSTEM_BUTTON,BUTTON_EVENT_CLICK, LED_GREEN);

	showLEDs();
}


void winConfigPedal::setChanged()
{
	if (!m_changed)
	{
		m_changed = 1;
		the_buttons.setButtonType(CANCEL_BUTTON,     BUTTON_EVENT_CLICK, LED_ORANGE);
		the_buttons.setButtonType(THE_SYSTEM_BUTTON, BUTTON_EVENT_CLICK, LED_PURPLE);
	}
}



//---------------------------------------------------
// BUTTONS
//---------------------------------------------------


// virtual
void winConfigPedal::onButton(int row, int col, int event)
{
    int num = row * NUM_BUTTON_COLS + col;

	if (num == KEYPAD_UP || num == KEYPAD_DOWN)
	{
        int inc = num == KEYPAD_UP ? -1 : 1;

        if (m_in_calibrate)
        {
            // empty case if calibrating
        }
        else if (m_cur_point >= 0)
        {
            pref_curve_point_t *points = CUR_POINTS;
            int y = points[m_cur_point].Y;
            int inc = num == KEYPAD_DOWN ? -1 : 1;
            y += inc;

            // restrict to above button to left and below button to right

            int right_y = m_cur_point < NUM_POINTS-1 ? points[m_cur_point+1].Y-1 : 127;
            int left_y = m_cur_point ? points[m_cur_point-1].Y+1 : 0;

            if (y > right_y) y = right_y;
            if (y < left_y) y = left_y;

            if (y != points[m_cur_point].Y)
            {
                points[m_cur_point].Y = y;
                m_redraw_curve = 1;
                the_pedals.getPedal(m_pedal_num)->invalidate();
				setChanged();
            }
        }
        else
        {
            m_cur_item += inc;
            if (m_cur_item >= NUM_ITEMS) m_cur_item = 0;
            if (m_cur_item < 0) m_cur_item = NUM_ITEMS - 1;
       }
	}
	else if (num == KEYPAD_LEFT || num == KEYPAD_RIGHT)
	{
        if (m_in_calibrate)
        {
            // empty case if calibrating
        }
        else if (m_cur_point >= 0)
        {
            pref_curve_point_t *points = CUR_POINTS;
            int x = points[m_cur_point].X;
            int inc = num == KEYPAD_LEFT ? -1 : 1;
            x += inc;

            // restrict to above button to left and below button to right

            int right_x = m_cur_point < NUM_POINTS-1 ? points[m_cur_point+1].X-1 : 127;
            int left_x = m_cur_point ? points[m_cur_point-1].X+1 : 0;

            if (x > right_x) x = right_x;
            if (x < left_x) x = left_x;

            if (x != points[m_cur_point].X)
            {
                points[m_cur_point].X = x;
                m_redraw_curve = 1;
                the_pedals.getPedal(m_pedal_num)->invalidate();
				setChanged();
            }
        }
        else
        {
            // empty case if right left while not editing point
        }
	}
    else if (num == KEYPAD_SELECT)
    {
        if (m_cur_point >= 0)
        {
            m_cur_point = -1;
            m_display_item = -1;
        }
        else if (m_cur_item >= NUM_FIXED_ITEMS)
        {
            m_cur_point = m_cur_item - NUM_FIXED_ITEMS;
            m_display_item = -1;
        }
		// not supported yet
		#define ITEM_CALIBRATE		0
		#define ITEM_NAME			1
		#define ITEM_PORT			2
		#define ITEM_CHANNEL		3
		#define ITEM_CC				4

        else if (m_cur_item == ITEM_CURVE_TYPE)
        {
            int curve = CUR_CURVE;
			curve = (curve + 1) % NUM_PEDAL_CURVES;
			CUR_CURVE = curve;
			init_prev_points();
            the_pedals.getPedal(m_pedal_num)->invalidate();
			setChanged();
        }
        else if (m_cur_item == ITEM_CALIBRATE)
        {
			if (m_in_calibrate)
			{
				m_in_calibrate = 0;
			}
			else
			{
				m_in_calibrate = 1;
				CUR_PEDAL.CALIB_MIN = 1023;
				CUR_PEDAL.CALIB_MAX = 0;
				setChanged();
			}
            m_display_curve = -1;
        }
    }

    // finishes

	else if (num == THE_SYSTEM_BUTTON)
	{
		endWindow(1);
    }
    else if (num == CANCEL_BUTTON)
	{
		memcpy(prefs.PEDAL,m_save_prefs,NUM_PEDALS * sizeof(pref_pedal_t));
		for (int i=0; i<NUM_PEDALS; i++)
			the_pedals.getPedal(i)->invalidate();
		endWindow(0);
    }
	else if (num == NEXT_PEDAL_BUTTON)
	{
        m_in_calibrate = 0;
        int next_num = (m_pedal_num+1) % NUM_PEDALS;
		setPedal(next_num);
		the_system.setTitle(name());
	}
}

//---------------------------------------------------
// DRAW
//---------------------------------------------------


#define TEXT_MARGIN  10
#define TEXT_SPACING 40

#define X_OFFSET    140
#define Y_OFFSET    50
#define CHART_MAX   255

#define RIGHT_COL          410
#define RIGHT_WIDTH        60
#define RIGHT_LINE_HEIGHT  20

const char *getCurveName(int i)
{
	switch(i)
	{
		case 0 : return "linear";
		case 1 : return "asympt";
		case 2 : return "scurve";
	}
	return "noWay!";
}

const char *getPointName(int curve, int point)
{
    if (!point) return "min";
    if (point == curve + 1) return "max";
    if (curve==2 && point==1) return "left";
    return curve==2 ? "right" : "mid";
}


void winConfigPedal::showSelectedItem(int item, int selected)
{
    const char *name = "huh?";
    if (item == ITEM_CALIBRATE)
    {
        name = "calib";
    }
	else if (item == ITEM_CURVE_TYPE)
    {
        name = getCurveName(CUR_CURVE);
    }
    else
    {
        name = getPointName(CUR_CURVE, item-NUM_FIXED_ITEMS);
    }

    int x = TEXT_MARGIN;
    int y = Y_OFFSET + item * TEXT_SPACING;
    int w = X_OFFSET - 2 * TEXT_MARGIN;
    int h = 30;

    int fc = TFT_WHITE;
    int bc = selected ? TFT_BLUE : TFT_BLACK;

    if (selected && (
        (m_in_calibrate && item==ITEM_CALIBRATE) ||
        (item>=NUM_FIXED_ITEMS && item-NUM_FIXED_ITEMS == m_cur_point)))
    {
        fc = TFT_BLUE;
        bc = TFT_YELLOW;
    }

    mylcd.setFont(Arial_16_Bold);
    mylcd.fillRect(x,y,w,h,bc);
    mylcd.printfJustified(
        x,
        y+5,
        w,
        h-3,
        LCD_JUST_CENTER,
        fc,
        bc,
        false,
        name);
}




// virtual
void winConfigPedal::updateUI()
{
    // disable interrupts while accessing pedal

    __disable_irq();
	expressionPedal *pedal = the_pedals.getPedal(m_pedal_num);
    int raw_value = pedal->getRawValue();
    int pedal_value = pedal->getValue();
    int pedal_x = pedal->getRawValueScaled();
	int pedal_pct = pedal->getRawValuePct();
    __enable_irq();

    bool full_redraw = m_display_curve != CUR_CURVE;
    bool select_changed = m_display_item != m_cur_item;
    bool calib_changed = m_in_calibrate && raw_value != m_display_raw_value;
	pref_curve_point_t *points = CUR_POINTS;

	// right side labels

    if (full_redraw)
    {
        fillRect(client_rect,TFT_BLACK);

        mylcd.setFont(Arial_12_Bold);
        int text_y = Y_OFFSET + 3 * RIGHT_LINE_HEIGHT;
        mylcd.printfJustified(RIGHT_COL,text_y,RIGHT_WIDTH,RIGHT_LINE_HEIGHT,
            LCD_JUST_CENTER,
            TFT_YELLOW,
            0,
            false,
            "CALIB");
        text_y += 4*RIGHT_LINE_HEIGHT;
        mylcd.printfJustified(RIGHT_COL,text_y,RIGHT_WIDTH,RIGHT_LINE_HEIGHT,
            LCD_JUST_CENTER,
            TFT_YELLOW,
            0,
            false,
            "VALUE");
    }

    // selected item

    for (int i=0; i<NUM_ITEMS; i++)
    {
        if (full_redraw || (
            select_changed && (
			i == m_cur_item ||
			i == m_display_item)))
        {
            showSelectedItem(i,i == m_cur_item);
        }
    }

	// calib min and max

    if (full_redraw || calib_changed)
    {
        int text_y = Y_OFFSET + 4*RIGHT_LINE_HEIGHT;
        if (!full_redraw)
            mylcd.fillRect(RIGHT_COL,text_y,RIGHT_WIDTH,2*RIGHT_LINE_HEIGHT,0);
        int color = m_in_calibrate ? TFT_GREEN : TFT_YELLOW;

        if (calib_changed)
        {
            if (raw_value < CUR_PEDAL.CALIB_MIN)
                CUR_PEDAL.CALIB_MIN = raw_value + CALIB_SAFETY_MARGIN;
            if (raw_value > CUR_PEDAL.CALIB_MAX)
                CUR_PEDAL.CALIB_MAX = raw_value - CALIB_SAFETY_MARGIN;
        }

        mylcd.setFont(m_in_calibrate ? Arial_12_Bold : Arial_12);
        mylcd.printfJustified(RIGHT_COL,text_y,RIGHT_WIDTH,RIGHT_LINE_HEIGHT,
            LCD_JUST_CENTER,
            color,
            0,
            false,
            "%d",
            CUR_PEDAL.CALIB_MIN);
        text_y += RIGHT_LINE_HEIGHT;

        mylcd.printfJustified(RIGHT_COL,text_y,RIGHT_WIDTH,RIGHT_LINE_HEIGHT,
            LCD_JUST_CENTER,
            color,
            0,
            false,
            "%d",
            CUR_PEDAL.CALIB_MAX);
    }



    // right side cur poit X and Y

    if (full_redraw || select_changed || m_redraw_curve)
    {
        int text_y = Y_OFFSET;
        mylcd.fillRect(RIGHT_COL,text_y,RIGHT_WIDTH,RIGHT_LINE_HEIGHT*2,0);
        if (m_cur_item >= NUM_FIXED_ITEMS)
        {
            int cur_point = m_cur_item-NUM_FIXED_ITEMS;

            mylcd.setFont(Arial_12);
            mylcd.printfJustified(RIGHT_COL,text_y,RIGHT_WIDTH,RIGHT_LINE_HEIGHT,
                LCD_JUST_CENTER,
                TFT_YELLOW,
                0,
                false,
                "x=%d",
                points[cur_point].X);
            text_y += RIGHT_LINE_HEIGHT;

            mylcd.printfJustified(RIGHT_COL,text_y,RIGHT_WIDTH,RIGHT_LINE_HEIGHT,
                LCD_JUST_CENTER,
                TFT_YELLOW,
                0,
                false,
                "y=%d",
                points[cur_point].Y);
        }
    }

    // REDRAW CHART

    if (full_redraw ||
        select_changed ||
        m_redraw_curve ||
        raw_value != m_display_raw_value ||
        m_display_pedal_value != pedal_value)
    {
        // erase old curve points if they moved

        for (int i=0; i<NUM_POINTS; i++)
        {
            pref_curve_point_t *cur0 = &points[i];
            pref_curve_point_t *prev0 = &m_prev_points[i];

            if (prev0->X != 255 &&
                prev0->Y != 255 && (
                prev0->X != cur0->X ||
                prev0->Y != cur0->Y))
            {
                mylcd.setDrawColor(TFT_BLACK);
                mylcd.fillCircle(
                    X_OFFSET + 2*prev0->X,
                    Y_OFFSET + (CHART_MAX-2*prev0->Y),
                    6);
            }
        }

		// erase the old current point

        if (m_display_pedal_x != -1)
        {
            mylcd.setDrawColor(TFT_BLACK);
            mylcd.fillCircle(
                X_OFFSET + 2*m_display_pedal_x,
                Y_OFFSET + (CHART_MAX-2*m_display_pedal_value),
                6);
        }

        // grid left w/ticks

        mylcd.setDrawColor(TFT_WHITE);
        mylcd.drawLine(X_OFFSET, Y_OFFSET, X_OFFSET, Y_OFFSET+CHART_MAX);
        for (int i=0; i<16; i++)
        {
            int y = Y_OFFSET + i * 16;
            mylcd.drawLine(X_OFFSET, y, X_OFFSET+9, y);
        }

        // grid center

		mylcd.drawLine(X_OFFSET, Y_OFFSET,   		 X_OFFSET+CHART_MAX, Y_OFFSET);  		   // top
        mylcd.drawLine(X_OFFSET, Y_OFFSET+CHART_MAX, X_OFFSET+CHART_MAX, Y_OFFSET+CHART_MAX);  // bottom
        mylcd.drawLine(X_OFFSET, Y_OFFSET+CHART_MAX, X_OFFSET+CHART_MAX, Y_OFFSET);            // diagonal

        // grid right w/ticks

        mylcd.drawLine(X_OFFSET+CHART_MAX, Y_OFFSET,   X_OFFSET+CHART_MAX,   Y_OFFSET+CHART_MAX);
        for (int i=0; i<16; i++)
        {
            int y = Y_OFFSET + i * 16;
            mylcd.drawLine(X_OFFSET+CHART_MAX-9, y, X_OFFSET+CHART_MAX, y);
        }


        //----------------------------------
        // lines
        //----------------------------------
        // line from 0 to MIN

		if (m_prev_points[0].X &&
			m_prev_points[0].X != 255)
		{
			mylcd.setDrawColor(TFT_BLACK);
			mylcd.drawLine(
				X_OFFSET,
				Y_OFFSET + CHART_MAX - 2*m_prev_points[0].Y,
				X_OFFSET + 2*m_prev_points[0].X,
				Y_OFFSET + CHART_MAX - 2*m_prev_points[0].Y);
		}

		// draw new one if off zero

		if (points[0].X)
		{
			mylcd.setDrawColor(TFT_YELLOW);
			mylcd.drawLine(
				X_OFFSET,
				Y_OFFSET + CHART_MAX - 2*points[0].Y,
				X_OFFSET + 2*points[0].X,
				Y_OFFSET + CHART_MAX - 2*points[0].Y);
		}

        // interior lines

        for (int i=0; i<NUM_POINTS-1; i++)
        {
            pref_curve_point_t *cur0 = &points[i];
            pref_curve_point_t *cur1 = &points[i+1];
            pref_curve_point_t *prev0 = &m_prev_points[i];
            pref_curve_point_t *prev1 = &m_prev_points[i+1];

			// if previous line was valid, redraw it in black

			if (prev0->X != 255 &&
				prev0->Y != 255 &&
				prev1->X != 255 &&
				prev1->Y != 255)
			{
				mylcd.setDrawColor(TFT_BLACK);
				mylcd.drawLine(
					X_OFFSET + 2*prev0->X ,
					Y_OFFSET + CHART_MAX - 2*prev0->Y,
					X_OFFSET + 2*prev1->X ,
					Y_OFFSET + CHART_MAX - 2*prev1->Y);
			}

			mylcd.setDrawColor(TFT_YELLOW);
			mylcd.drawLine(
				X_OFFSET + 2*cur0->X ,
				Y_OFFSET + CHART_MAX - 2*cur0->Y,
				X_OFFSET + 2*cur1->X ,
				Y_OFFSET + CHART_MAX - 2*cur1->Y);

        }   // for each line


        // line from MAX to 127
		// erase old one if it moved

        int max_point = NUM_POINTS-1;
		if (m_prev_points[max_point].X < 127)
		{
			mylcd.setDrawColor(TFT_BLACK);
			mylcd.drawLine(
				X_OFFSET + 2*m_prev_points[max_point].X,
				Y_OFFSET + CHART_MAX - 2*m_prev_points[max_point].Y,
				X_OFFSET + CHART_MAX,
				Y_OFFSET + CHART_MAX - 2*m_prev_points[max_point].Y);
		}

		// draw new one if off zero

		if (points[max_point].X < 127)
		{
			mylcd.setDrawColor(TFT_YELLOW);
			mylcd.drawLine(
				X_OFFSET + 2*points[max_point].X,
				Y_OFFSET + CHART_MAX - 2*points[max_point].Y,
				X_OFFSET + CHART_MAX,
				Y_OFFSET + CHART_MAX - 2*points[max_point].Y);
		}

        //---------------------
        // new points
        //---------------------

        for (int i=0; i<NUM_POINTS; i++)
        {
            pref_curve_point_t *cur0 = &points[i];
            int fill_color =
                i == m_cur_point ? TFT_RED :
                i == m_cur_item-NUM_FIXED_ITEMS ? TFT_GREEN : TFT_YELLOW;

            mylcd.setDrawColor(fill_color);
            mylcd.fillCircle(
                X_OFFSET + 2*cur0->X,
                Y_OFFSET + (CHART_MAX-2*cur0->Y),
                6);
        }

        mylcd.setDrawColor(TFT_CYAN);
        mylcd.fillCircle(
            X_OFFSET + 2*pedal_x,
            Y_OFFSET + (CHART_MAX-2*pedal_value),
            6);
    }

    // current VALUES

    if (full_redraw ||
        select_changed ||
        m_redraw_curve ||
        raw_value != m_display_raw_value ||
        m_display_pedal_value != pedal_value)
    {
        int text_y = Y_OFFSET + 8 * RIGHT_LINE_HEIGHT;
        mylcd.fillRect(RIGHT_COL,text_y,RIGHT_WIDTH,3*RIGHT_LINE_HEIGHT,0);

        mylcd.setFont(Arial_12);
        mylcd.printfJustified(RIGHT_COL,text_y,RIGHT_WIDTH,RIGHT_LINE_HEIGHT,
            LCD_JUST_CENTER,
            TFT_YELLOW,
            0,
            false,
            "%d",
            raw_value);
        text_y += RIGHT_LINE_HEIGHT;

        mylcd.printfJustified(RIGHT_COL,text_y,RIGHT_WIDTH,RIGHT_LINE_HEIGHT,
            LCD_JUST_CENTER,
            TFT_YELLOW,
            0,
            false,
            "%0.2f%%",
            pedal_pct);
        text_y += RIGHT_LINE_HEIGHT;

        mylcd.printfJustified(RIGHT_COL,text_y,RIGHT_WIDTH,RIGHT_LINE_HEIGHT,
            LCD_JUST_CENTER,
            TFT_YELLOW,
            0,
            false,
            "%d",
            pedal_x);
        text_y += 2*RIGHT_LINE_HEIGHT;

        mylcd.setFont(Arial_16_Bold);
        mylcd.printfJustified(RIGHT_COL,text_y,RIGHT_WIDTH,RIGHT_LINE_HEIGHT,
            LCD_JUST_CENTER,
            TFT_YELLOW,
            0,
            true,
            "%d",
            pedal_value);
    }

    // show raw value changes as a rectangle just below the grid
    // and a line to the value

    if (full_redraw ||
        m_redraw_curve ||
        pedal_x != m_display_pedal_x)
    {
        int bar_y = Y_OFFSET + CHART_MAX + 1;
        int bar_h = 320 - bar_y + 1;

        mylcd.fillRect(X_OFFSET-1,bar_y,pedal_x*2+3,bar_h,TFT_CYAN);
        mylcd.fillRect(X_OFFSET+pedal_x*2+3,bar_y,CHART_MAX-pedal_x*2-3,bar_h,0);

    }

    // finished

    m_redraw_curve = 0;
    m_display_curve = CUR_CURVE;
    m_display_item = m_cur_item;
    m_display_raw_value = raw_value;
    m_display_pedal_value = pedal_value;
    m_display_pedal_x = pedal_x;
	memcpy(m_prev_points,points,NUM_CURVE_POINTS * sizeof(pref_curve_point_t));

}   // updateUI()
