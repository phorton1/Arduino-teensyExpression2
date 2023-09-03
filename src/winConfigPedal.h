//-----------------------------------------
// winConfigPedal.h
//-----------------------------------------
// This window keeps a copy of the entire prefs.PEDAL information,
// and if the changes are cancelled restores the prefs.
//
// It is started on a particular pedal but has a button to allow
// the user to cycle through pedals, so all changes are either
// accepted or cancelled.

#pragma once

#include "prefs.h"
#include "theSystem.h"


class winConfigPedal : public sysWindow
{
    public:

        winConfigPedal();

        void setPedal(int i);

    private:

        int m_pedal_num;
        bool m_changed;

        int m_cur_item;       // 0 .. m_num_items-1
        int m_cur_point;      // 1 based

        // int m_cur_mode;      // getPref8(PREF_PEDAL(m_pedal_num) + PREF_PEDAL_AUTO_OFFSET)  0..1
        // int m_cur_curve;      // getPref8(PREF_PEDAL(m_pedal_num) + PREF_PEDAL_CURVE_TYPE_OFFSET)  0..2
        // int m_num_points;     // m_cur_curve + 2
        // int m_num_items;      // m_num_points + 1

        int m_display_item;
        int m_display_curve;
        int m_display_raw_value;
        int m_display_pedal_x;
        int m_display_pedal_value;
        bool m_redraw_curve;
        bool m_in_calibrate;
        pref_curve_point_t m_prev_points[NUM_CURVE_POINTS];

        pref_pedal_t m_save_prefs[NUM_PEDALS];

        void init();
        void init_prev_points();
        virtual const char *name();

        virtual void begin(bool warm) override;
        virtual void onButton(int row, int col, int event) override;
        virtual void updateUI() override;

        void setChanged();
        void onPedalCurveChanged();
        void showSelectedItem(int item, int selected);

};


extern winConfigPedal  win_config_pedal;


// end of winConfigPedal.h
