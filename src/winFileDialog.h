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

		const char *getSelectedFilename();
			// if the window returns the ID,
			// call this to get the selected filename

    private:

        const char *m_name;
		const char *m_path;
		const char *m_ext;
		const char *m_default;

        virtual const char *name()          { return m_name; }
        virtual const char *short_name()    { return m_name; }

		virtual void end() override;
        virtual void begin(bool cold) override;
        virtual void onButton(int row, int col, int event) override;
		virtual void updateUI() override;

		void getFilenames();

};


extern winFileDialog win_file_dlg;




// end of winFileDialog.h
