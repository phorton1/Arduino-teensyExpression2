//---------------------------------------------
// theSystem.h
//---------------------------------------------
// The guts of the teensyExpression system.

# pragma once

#include "defines.h"
#include <Arduino.h>        // for intevalTimer

#define MAX_WINDOW_STACK     10

#define WIN_FLAG_OWNER_TITLE        0x00001000
    // window calls theSystem.setTitle() itself
#define WIN_FLAG_SHOW_PEDALS        0x00002000
    // pedal area shown by the system when window displayed

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

        sysWindow()                 {m_flags = 0;}
        sysWindow(uint32_t flags)   {m_flags = flags;}
        virtual ~sysWindow()        {}

        virtual const char *name() = 0;
        virtual uint32_t getId()    { return 0; }

    protected:

        friend class theSystem;

        virtual void begin(bool warm);
        virtual void end()  {}

        virtual void updateUI() {}
        virtual void onButton(int row, int col, int event) {}

        virtual void endWindow(uint32_t param);

        uint32_t m_flags;
};



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

        void startWindow(sysWindow *win);
        void swapWindow(sysWindow *win, uint32_t param);
        void endWindow(sysWindow *win, uint32_t param);

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

        void handleSerialData();

        volatile int m_num_windows;
        sysWindow *m_window_stack[MAX_WINDOW_STACK];
        sysWindow *getTopWindow();
};


extern theSystem the_system;
    // in the_system.cpp
