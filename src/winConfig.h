//--------------------------------------
// winConfig.h
//--------------------------------------

#pragma once

#include "theSystem.h"
#include "configOptions.h"


class winConfig : public sysWindow
{
    public:

        winConfig() : sysWindow() {}

    private:

        virtual const char *name()      override { return "SYSTEM SETTINGS"; }
        virtual const char *shortName() override { return "Sys Settings"; }

        virtual void begin(bool warm) override;
        virtual void updateUI() override;
        virtual void onButton(int row, int col, int event) override;
        virtual void onChildEnd(uint16_t param) override;

        void setChanged();
        void checkChanged();

        int m_scroll_top;
        int m_opt_num;
        bool m_changed;

        configOption *m_rootOption;
        configOption *m_cur_menu;
        configOption *m_cur_option;
        configOption *m_last_option;
        configOption *m_display_menu;
        configOption *m_display_option;
        configOption *m_optBrightness;



};


extern winConfig win_config;


// end of winConfig.h