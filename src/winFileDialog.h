//------------------------------------------------
// winFileDialog.h
//------------------------------------------------
// Contains the winFileDialog base class and specific
// derived classes.

#pragma once

#include "theSystem.h"

class winFileDialog : public sysWindow
{
	public:

		const char *getSelectedFilename();
			// if the window returns the ID,
			// call this to get the selected filename

    protected:

        winFileDialog() {}

		void setup(uint16_t id,
				   const char *name,
				   const char *path,
				   const char *ext,
				   const char *def = 0)
		{
			m_id = id;
			m_name = name;
			m_path = path;
			m_ext = ext;
			m_default = def;
		}

        virtual void begin(bool cold) override;
        virtual void onButton(int row, int col, int event) override;

    private:

        const char *m_name;
		const char *m_path;
		const char *m_ext;
		const char *m_default;

        virtual const char *name()          { return m_name; }
        virtual const char *short_name()    { return m_name; }

		virtual void end() override;
		virtual void updateUI() override;

		void getFilenames();

};



//----------------------------------------------------
// derived classes
//----------------------------------------------------
// exists to parse the rig on BUTTON_ACCEPT and NOT
// end the window if there is an error.  It also encapsulates
// the setup() parameters.

class rigFileDialog : public winFileDialog
{
	public:

		rigFileDialog()   {}

	private:

        virtual void begin(bool cold) override;
        virtual void onButton(int row, int col, int event) override;
};



extern rigFileDialog rig_file_dlg;




// end of winFileDialog.h
