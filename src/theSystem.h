//---------------------------------------------
// theSystem.h
//---------------------------------------------
// The guts of the teensyExpression system.

# pragma once

#include "defines.h"
#include <Arduino.h>        // for intevalTimer

#define MAX_WINDOW_STACK     10

// globally defined screen regions

extern int_rect tft_rect;                   // the full screen
extern int_rect title_rect;                 // title area not incuding line
extern int_rect full_client_rect;           // from under line to end of screen - available to windows
extern int_rect pedal_rect;                 // the area containing the pedals - shown in rigs
extern int_rect client_rect;                // area available to rigs - under title to top of pedals


class theSystem;
    // forward

class sysWindow
    // base class for system windows
{
    public:

        sysWindow() {}

        virtual const char *name() = 0;
        virtual const char *shortName() { return ""; }

        void setId(uint16_t id)     { m_id = id; }
        uint32_t getId()            { return m_id; }

    protected:

        friend class theSystem;

        virtual void begin(bool warm);
        virtual void end()  {}

        virtual void updateUI() {}
        virtual void onButton(int row, int col, int event) {}

        virtual void endWindow(uint16_t param);
            // end this window passing param to the parent, if any
        virtual bool onChildEnd(uint16_t param) { return true; }
            // called when a child window desires to end, before
            //    the parent window begin(0) is called.
            // parent window may return FALSE to prevent child from ending
            //    esp in the case when parent window knows that there has
            //    been another window popped up over the child
        uint16_t m_id;
};



class theSystem
{
    public:

        theSystem();
        ~theSystem()  {}

        void begin();
        void loop();

        void setTitle(const char *title, bool draw_pedals=false);
        void onButton(int row, int col, int event);

        inline void midiActivity(int act_num) { m_midi_activity[act_num] = millis(); }

        void startWindow(sysWindow *win);
        void swapWindow(sysWindow *win, uint16_t param);
        void endWindow(sysWindow *win, uint16_t param);

    private:

        bool m_draw_title;
        bool m_draw_pedals;
        const char *m_title;
        int m_battery_level;

        IntervalTimer m_timer;
        IntervalTimer m_critical_timer;
        static void timer_handler();
        static void critical_timer_handler();

        uint32_t m_midi_activity[NUM_ACTIVITY_INDICATORS];
        bool m_last_midi_activity[NUM_ACTIVITY_INDICATORS];

        volatile int m_num_windows;
        sysWindow *m_window_stack[MAX_WINDOW_STACK];
        sysWindow *getTopWindow();
};


extern theSystem the_system;
    // in the_system.cpp

// end of theSystem.h
