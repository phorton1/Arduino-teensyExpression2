#pragma once

#include "prefs.h"


typedef struct
{
    int x;          // 0..127
    int y;
    int weight;

} pedalPoint_t;


class expressionPedal
{
    public:

        int getNum()                    { return m_num; }
        const char *getName()           { return m_name; }

        int getValue()                  { return m_value; }
        int getRawValue()               { return m_raw_value; }
            // the actual values

        int getDisplayValue()           { return m_display_value; }
        void setDisplayValue(int i)     { m_display_value = i; }
        bool displayValueChanged()      { return m_last_display_value != m_display_value; }
        void clearDisplayValueChanged() { m_last_display_value = m_display_value; }
            // completely separate display values

        inline float getRawValuePct()
        {
            float min = prefs.PEDAL[m_num].CALIB_MIN;
            float max = prefs.PEDAL[m_num].CALIB_MAX;
            float val = m_raw_value - min;
            if (val < 0.00) val = 0.00;
            float ret_val = val / (max - min);
            if (ret_val > 1.0) ret_val = 1.0;
            return ret_val;
        }
        int getRawValueScaled()
        {
            float ret_val = getRawValuePct() * 127.00 + 0.5;
            return ret_val;
        }
        void invalidate()
        {
            m_valid = false;
        }


    protected:

        friend class pedalManager;

        expressionPedal() {}

        void init(
            int num,
            int pin );

        void poll();

    private:

        // construction paramaters

        int     m_num;
        int     m_pin;

        // configuration parameters

        char    m_name[MAX_PEDAL_NAME+1];
        uint8_t m_port;
        uint8_t m_channel;      // zero based
        uint8_t m_cc;

        // runtime working variables

        bool     m_valid;
        int      m_raw_value;       // 0..1023
        int      m_direction;       // -1,0,1
        uint32_t m_settle_time;
        int      m_value;           // 0..127

        int      m_display_value;           // display helper
        int      m_last_display_value;      // display helper

        // uint8_t  m_mode;
        // int      m_auto_value;
        // bool     m_in_auto_calibrate;

};



class pedalManager
{
    public:

        pedalManager() {}

        void init();
            // called at runtime to setup pedals from prefs

        void task();
            // polls pedals, may call expSystem::onPedalEvent()

        expressionPedal *getPedal(int i)  { return &m_pedals[i]; }

        void pedalEvent(int num, int value);

        void setPedal(int num, const char *name, uint8_t port, uint8_t channel, uint8_t cc);
            // configure the pedal.
            // name will be truncated at 7 characters
            // port is an actual port (not an enum)
            // channel iz zero based

    private:

        friend class expressionPedal;

        expressionPedal m_pedals[NUM_PEDALS];

        bool m_relative_loop_volume_mode;
        int m_relative_loop_volume[4];


};


extern pedalManager the_pedals;
