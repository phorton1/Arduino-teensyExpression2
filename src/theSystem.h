//---------------------------------------------
// theSystem.h
//---------------------------------------------
// The guts of the teensyExpression system.

# pragma once

#include "defines.h"
#include <Arduino.h>        // for intevalTimer

// globally defined screen regions

extern int_rect tft_rect;                   // the full screen
extern int_rect title_rect;                 // title area not incuding line
extern int_rect full_client_rect;           // from under line to end of screen - available to windows
extern int_rect pedal_rect;                 // the area containing the pedals - shown in rigs
extern int_rect client_rect;                // area available to rigs - under title to top of pedals



class theSystem
{
    public:

        theSystem();
        ~theSystem()  {}

        void begin();
        void loop();

        void setTitle(const char *title);
        void onButton(int row, int col, int event);

        inline void midiActivity(int act_num) { m_midi_activity[act_num] = millis(); }

    private:

        bool draw_title;
        bool draw_pedals;
        const char *m_title;
        int last_battery_level;

        IntervalTimer m_timer;
        IntervalTimer m_critical_timer;
        static void timer_handler();
        static void critical_timer_handler();

        uint32_t m_midi_activity[NUM_ACTIVITY_INDICATORS];
        bool m_last_midi_activity[NUM_ACTIVITY_INDICATORS];

        void handleSerialData();
};


extern theSystem the_system;
    // in the_system.cpp
