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
        // const char *getName();          { return prefs.PEDAL[m_num].NAME; }

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

        // the pedal can
        // void setPedalMode();
        // uint8_t getMode()           { return m_mode; }
        //
        // void autoCalibrate();
        // bool inAutoCalibrate()      { return m_in_auto_calibrate; }
        //
        // void setAutoRawValue(int i);
        // int getAutoRawValue()       { return m_auto_value; }

        // midi

        // void setCCs(int channel, int cc_num)
        // {
        //     m_cc_channel = channel;
        //     m_cc_num = cc_num;
        // }
        //
        // int getCCChannel()              { return m_cc_channel; }
        // int getCCNum()                  { return m_cc_num; }

        // PRIVATE isr handling

        // void teensyReceiveByte();
        // void teensySendByte(int byte);

    protected:

        friend class pedalManager;

        expressionPedal() {}

        void init(
            int num,
            int pin );
            // const char *name,
            // int cc_channel,
            // int cc_num);

        void poll();

    private:

        // construction paramaters

        int     m_num;
        int     m_pin;          // defined in pedals.cpp
        int     m_pedal_num;    // they know this too ...

        // int     m_cc_channel;
        // int     m_cc_num;
        // const char *m_name;

        // runtime working variables

        bool     m_valid;
        int      m_raw_value;       // 0..1023
        int      m_direction;       // -1,0,1
        unsigned m_settle_time;
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

        // vestigial kludge for oldRig + quantiloop
        // void setLoopPedalRelativeVolumeMode(bool b) {m_relative_loop_volume_mode = b; }
        // int getRelativeLoopVolume(int i)  { return m_relative_loop_volume[i]; }
        // void setRelativeLoopVolume(int i, int value) { m_relative_loop_volume[i] = value; }

    private:

        friend class expressionPedal;

        expressionPedal m_pedals[NUM_PEDALS];

        bool m_relative_loop_volume_mode;
        int m_relative_loop_volume[4];


};


extern pedalManager thePedals;
