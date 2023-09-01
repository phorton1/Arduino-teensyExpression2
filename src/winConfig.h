//--------------------------------------
// winConfig.h
//--------------------------------------

#pragma once

#include "theSystem.h"
#include "configOptions.h"


class winConfig : public sysWindow
{
    public:

        winConfig() : sysWindow(WIN_FLAG_OWNER_TITLE) {}

    private:

        virtual const char *name()      override { return "SYSTEM SETTINGS"; }
        virtual const char *shortName() override { return "Sys Settings"; }

        virtual void begin(bool warm) override;
        virtual void updateUI() override;
        virtual void onButton(int row, int col, int event) override;
        virtual void onChildEnd(uint16_t param) override;

        void notifyTerminalModeEnd();
            // called by options with implemented terminal modes
            // at the end of their operation to return to the
            // configSystem editor

        int m_scroll_top;
        int m_opt_num;


};


extern winConfig win_config;


// end of winConfig.h