//------------------------------------------------
// winDialogs.h
//------------------------------------------------
// Contains common dialog windows
// Note that these cannot be stacked on top of the same kind!
// YesNoDialog returns 0 or the ID that is passed in.

#pragma once

#include "theSystem.h"

#define MAX_DIALOG_MSG	 255


//------------------------------------
// rigErrorDialog
//------------------------------------

class rigErrorDialog : public sysWindow
{
    public:

        rigErrorDialog() {}

		void setMessage(const char *msg)   { strcpy(m_msg,msg); }

    private:

		char m_msg[MAX_DIALOG_MSG + 1];

        virtual const char *name() override      { return "Rig Error"; }
        virtual const char *shortName() override { return "Rig Error"; }

        virtual void begin(bool cold) override;
        virtual void onButton(int row, int col, int event) override;
};

extern rigErrorDialog rig_error_dlg;


//------------------------------------
// yesNoDialog
//------------------------------------

class yesNoDialog : public sysWindow
{
    public:

        yesNoDialog() { m_name = "yesNoDialog"; }

		void setName(const char *name)	   { m_name = name; }
		void setMessage(const char *msg)   { strcpy(m_msg,msg); }

    private:

        const char *m_name;
		char m_msg[MAX_DIALOG_MSG + 1];

        virtual const char *name()          { return m_name; }
        virtual const char *short_name()    { return m_name; }

        virtual void begin(bool cold) override;
        virtual void onButton(int row, int col, int event) override;

};


extern yesNoDialog yes_no_dlg;


// end of winDialogs.h
