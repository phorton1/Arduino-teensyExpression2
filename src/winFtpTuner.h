//-------------------------------------------------
// winFtpTuner.h
//-------------------------------------------------
#pragma once

#include "theSystem.h"


class winFtpTuner : public sysWindow
{
    public:

        winFtpTuner();

    private:

        virtual const char *name() override      { return "FTP Tuner"; }
        virtual const char *shortName() override { return "FTP Tuner"; }

        virtual void end() override;
        virtual void begin(bool warm) override;
        virtual void updateUI() override;
        virtual void onButton(int row, int col, int event) override;

        // implementation

        bool m_draw_needed;
        int  m_string_pressed[NUM_FTP_STRINGS];
        int  m_tuner_note;
        int  m_tuner_value;

        void init();
        void drawCircle(int string, int fret, bool pressed);
        void fretsToInts(int *ints);
        void drawTunerPointer(int tuner_x, int color);
};


extern winFtpTuner win_ftp_tuner;


// end of winFtpTuner.h
