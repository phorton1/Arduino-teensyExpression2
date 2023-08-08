//---------------------------------------------
// theSystem.h
//---------------------------------------------
// The guts of the teensyExpression system.
//
# pragma once

#include "defines.h"
#include <Arduino.h>        // for intevalTimer


#define MIDI_ACTIVITY_INLINE  1




#if 0

#define MAX_EXP_RIGS        10
#define MAX_MODAL_STACK     10


class aSystem;

#define WIN_FLAG_DELETE_ON_END      0x00010000
    // window will be deleted after call to endModal
#define WIN_FLAG_OWNER_TITLE        0x00001000
    // window calls theSystem.setTitle() itself
#define WIN_FLAG_SHOW_PEDALS        0x00002000
    // window calls theSystem.setTitle() itself

// globally defined screen regions

extern int_rect tft_rect;                   // the full screen
extern int_rect title_rect;                 // title area not incuding line
extern int_rect full_client_rect;           // from under line to end of screen - available to windows
extern int_rect pedal_rect;                 // the area containing the pedals - shown in rigs
extern int_rect client_rect;                // area available to rigs - under title to top of pedals

// screen regions for "standard" rigs (derived from rigBase)
// that also work with the songMachine ...

extern int_rect synth_rect;                 // top part of the client area
extern int_rect song_title_rect;            // shows the current song title
extern int_rect song_state_rect;            // shows the current songmachine state
extern int_rect song_msg_rect[2];           // the two regions for user defined DISPLAY messages



class aWindow
    // base class for rigs, modal windows, and the configSystem
{
    public:

        aWindow()                 {m_flags = 0;}
        aWindow(uint32_t flags)   {m_flags = flags;}
        virtual ~aWindow()        {}

        virtual const char *name() = 0;
            // used for titles
        virtual const char *short_name() { return ""; };
            // only used for enumerated rigs in the config window
        virtual uint32_t getId()    { return 0; }


    protected:

        friend class aSystem;

        virtual void begin(bool warm)  {}
            // warm means that we are coming down the modal window stack
            // as opposed to being invoked as a new window.
        virtual void end()  {}
            // called when the window is taken out of focus, they
            // don't generally need to worry about buttons and LEDs,
            // but may want to unregister midi event handlers, etc

        virtual void onButtonEvent(int row, int col, int event) {}
        virtual void onSerialMidiEvent(int cc_num, int value) {}

        virtual void updateUI() {}
        virtual void timer_handler()  {}

        virtual void onEndModal(aWindow *win, uint32_t param) {}
            // called by aSystem after modal windows close themselves
            // with calls to endModal();

        virtual void endModal(uint32_t param);
            // called by modal windows when they end themselves

        uint32_t m_flags;

};



#endif  // 0


class aSystem
{
    public:

        aSystem();
        ~aSystem()  {}

        void begin();
        void loop();

        void buttonEvent(int row, int col, int event);

        #if MIDI_ACTIVITY_INLINE
            inline void midiActivity(int port_num) { m_midi_activity[port_num]=millis(); }
        #else
            void midiActivity(int port_num);
        #endif

#if 0

        void activateRig(int i);

        int getNumRigs()         { return m_num_rigs; }
        int getCurRigNum()       { return m_cur_rig_num; }
        int getPrevRigNum()      { return m_prev_rig_num; }
        aWindow *getCurRig()   { return m_rigs[m_cur_rig_num]; }

        void setTitle(const char *title);

        void startModal(aWindow *win);
        void swapModal(aWindow *win, uint32_t param);
        void endModal(aWindow *win, uint32_t param);
        aWindow *getTopModalWindow();

        int getTempo()      { return m_tempo; }
#endif

    private:

        IntervalTimer m_timer;
        static void timer_handler();

        unsigned m_midi_activity[NUM_MIDI_PORTS];
        bool m_last_midi_activity[NUM_MIDI_PORTS];

#if 0

        void addRig(aWindow *pRig);
        void startWindow(aWindow *win, bool warm);
        void handleSerialData();

        static void critical_timer_handler();
        IntervalTimer m_critical_timer;

        int m_num_rigs;
        int m_cur_rig_num;
        int m_prev_rig_num;
        aWindow *m_rigs[MAX_EXP_RIGS + 1];
            // 1 extra for rig #0 which is overloaded
            // as the configSystem window.

        volatile int m_num_modals;
        aWindow *m_modal_stack[MAX_MODAL_STACK];

        bool draw_title;
        bool draw_pedals;
        const char *m_title;
        int last_battery_level;


        int m_tempo;
#endif

};


extern aSystem theSystem;
    // in theSystem.cpp
