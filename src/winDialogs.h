//------------------------------------------------
// winDialogs.h
//------------------------------------------------
// Contains common dialog windows
// Note that these cannot be stacked on top of the same kind!
// YesNoDialog returns 0 or the ID that is passed in.

#pragma once

#include "theSystem.h"
#include "myTFT.h"

#define MAX_DIALOG_MSG	 255


//------------------------------------
// okDialog
//------------------------------------

class okDialog : public sysWindow
{
    public:

        okDialog() {}

		void setMessage(uint16_t display_color, const char *name, const char *msg)
		{
			m_color = display_color;
			strcpy(m_name,name);
			strcpy(m_msg,msg);
		}

    private:

		uint16_t m_color;
		char     m_name[32];
		char     m_msg[MAX_DIALOG_MSG + 1];

        virtual const char *name() override      { return m_name[0] ? m_name : "Error"; }
        virtual const char *shortName() override { return m_name[0] ? m_name :  "Error"; }

        virtual void begin(bool cold) override;
        virtual void onButton(int row, int col, int event) override;
};

extern okDialog ok_dlg;


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



//------------------------------------
// formatSDDialog
//------------------------------------

class formatSDDialog : public sysWindow
{
    public:

        formatSDDialog() {}

    private:

        virtual const char *name()          { return "Format SD Card"; }
        virtual const char *short_name()    { return "Format SD Card"; }

        virtual void begin(bool cold) override;
        virtual void onButton(int row, int col, int event) override;

};


extern formatSDDialog format_sd_dialog;


// end of winDialogs.h
