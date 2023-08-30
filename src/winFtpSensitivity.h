//-------------------------------------------------
// winFtpSensitivity.h
//-------------------------------------------------
#pragma once

#include "theSystem.h"


#define NUM_SENSITIVITY_ITEMS 9


class winFtpSensitivity : public sysWindow
{
    public:

        winFtpSensitivity();

    private:

        virtual const char *name() override      { return "FTP String Sensitivity"; }
        virtual const char *shortName() override { return "FTP Sens"; }

        virtual void begin(bool warm) override;
        virtual void updateUI() override;
        virtual void onButton(int row, int col, int event) override;

        // implementation

        bool m_draw_needed;
        int  m_vel[NUM_FTP_STRINGS];
        int  m_velocity[NUM_FTP_STRINGS];
        int  m_value[NUM_SENSITIVITY_ITEMS];
        int  m_selected;
        int  m_last_selected;

        void init();
        void vel2ToInts(int *vel2, int *velocity);
        void drawBox(int string, int box32, int vel16);

};


extern winFtpSensitivity win_ftp_sensitivity;

// end of winFtpSensitivity.h
