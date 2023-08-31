#include <myDebug.h>
#include "pedals.h"
#include "prefs.h"
#include "theSystem.h"
#include "midiQueue.h"  // for sendMidiControlChange()

#define DEBUG_PEDALS  0

#define HYSTERISIS   30
    // in raw 0..1023 units
#define SETTLE_TIME  50
    // time to settle into a direction

pedalManager the_pedals;


//------------------------------------
// pedalManager
//------------------------------------

void pedalManager::init()
{
    m_pedals[0].init(0, PIN_EXPR1);
    m_pedals[1].init(1, PIN_EXPR2);
    m_pedals[2].init(2, PIN_EXPR3);
    m_pedals[3].init(3, PIN_EXPR4);
}

void pedalManager::task()
{
    for (int i=0; i<NUM_PEDALS; i++)
        m_pedals[i].poll();
}

void pedalManager::setPedal(int num, const char *name, uint8_t port, uint8_t channel, uint8_t cc)
{
    memcpy(m_pedals[num].m_name,name,MAX_PEDAL_NAME);
    m_pedals[num].m_name[MAX_PEDAL_NAME] = 0;
    m_pedals[num].m_port = port;
    m_pedals[num].m_channel = channel;
    m_pedals[num].m_cc = cc;
}


//------------------------------------
// expressionPedal
//------------------------------------

void expressionPedal::init(
    int num,
    int pin)
{
    m_num = num;
    m_pin = pin;


    strcpy(m_name,prefs.PEDAL[num].NAME);
    m_port    = MIDI_ENUM_TO_PORT(prefs.PEDAL[num].PORT);
    m_channel = prefs.PEDAL[num].CHANNEL;
    m_cc      = prefs.PEDAL[num].CC;

    m_raw_value = -1;         // 0..1023
    m_direction = -1;
    m_settle_time = 0;

    m_value = 0;
    m_valid = false;

    m_display_value = 0;
    m_last_display_value = -1;

    pinMode(m_pin,INPUT_PULLDOWN);
}


void expressionPedal::poll()
{
    uint32_t time = millis();
    bool raw_changed = false;
    int raw_value = analogRead(m_pin);

    // display(0,"poll(%d) raw_value=%d",m_num,raw_value);

    // if not moving, and outside of hysterisis range, start moving

    if (!m_direction)
    {
        if (raw_value > m_raw_value + HYSTERISIS)
        {
            m_direction = 1;
            m_raw_value = raw_value;
            m_settle_time = time;
            raw_changed = 1;
        }
        else if (raw_value < m_raw_value - HYSTERISIS)
        {
            m_direction = -1;
            m_raw_value = raw_value;
            m_settle_time = time;
            raw_changed = 1;
        }
    }

    // if stopped moving, reset to default state

    else if (time >= m_settle_time + SETTLE_TIME)
    {
        m_settle_time = 0;
        m_direction = 0;
    }

    // otherwise, process the input

    else if (m_direction > 0 && raw_value > m_raw_value)
    {
        m_raw_value = raw_value;
        m_settle_time = time;
        raw_changed = 1;
    }
    else if (m_direction < 0 && raw_value < m_raw_value)
    {
        m_raw_value = raw_value;
        m_settle_time = time;
        raw_changed = 1;
    }


    //-------------------------------
    // calculate value
    //-------------------------------

    if (raw_changed || !m_valid)
    {
        int value;
        pref_pedal_t *pref_pedal = &prefs.PEDAL[m_num];
        int curve_type = pref_pedal->CURVE_TYPE;
        pref_curve_t *pref_curve = &pref_pedal->CURVE[curve_type];

        // get the scaled x, and min and max values

        int scaled_x = getRawValueScaled();
        int min_x = pref_curve->POINTS[0].X;
        int max_x = pref_curve->POINTS[curve_type+1].X;

        // we are to the left of the MIN, so our value is MIN.Y

        if (scaled_x < min_x)
        {
            value = pref_curve->POINTS[0].Y;
            // display(0,"LESS THAN MIN_X value=%d",value);
        }

        // we are at, or to the right, of the MAX so our value is MAX.Y

        else if (scaled_x >= max_x)
        {
            value = pref_curve->POINTS[curve_type+1].Y;
            // display(0,"GE THAN MAX_X value=%d",value);
        }

        // loop thru points left (not max) pointstill we are at or to the right of one
        // or we are out of points

        else
        {
            int point_num = 0;
            int right_x =  pref_curve->POINTS[point_num+1].X;;

            while (point_num < curve_type &&
                   scaled_x >= right_x)
            {
                point_num++;
                right_x = pref_curve->POINTS[point_num+1].X;
            }

            // we are now between point_num and point_num+1


            int left_x = pref_curve->POINTS[point_num].X;
            int left_y = pref_curve->POINTS[point_num].Y;
            int right_y = pref_curve->POINTS[point_num+1].Y;

            // display(0,"SCALING %d BETWEEN POINT %d(%d,%d) and %d(%d,%d)",
            //     scaled_x,point_num,left_x,left_y,point_num+1,right_x,right_y);

            float range_x = (right_x - left_x) + 1;
            float range_y = (right_y - left_y) + 1;
            float val_x = (scaled_x - left_x);
            float pct = val_x / range_x;
            float val_y = left_y + pct * range_y + 0.5;
            value = val_y;

            // display(0,"    range_x(%0.2f) range_y(%0.2f) val_x(%0.2f) pct(%0.2f) val_y(%0.2f) VALUE=%d",
            //     range_x,range_y,val_x,pct,val_y,value);
        }

        // actual value changed
        // overwrites the display value

        if (value != m_value)
        {
            m_value = value;
            m_display_value = value;

            #if DEBUG_PEDALS
                display(0,"pedal(%d) raw(%d) changed to %d",m_num,m_raw_value,m_value);
            #endif

            the_pedals.pedalEvent(m_num,m_value);
        }
    }

    m_valid = true;
}



//----------------------------------------------------
// pedalEvent
//----------------------------------------------------

void pedalManager::pedalEvent(int num, int value)
{
	 sendMidiControlChange(
        m_pedals[num].m_port,
        m_pedals[num].m_channel + 1,
        m_pedals[num].m_cc,
        value);
}
