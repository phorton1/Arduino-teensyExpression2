//-------------------------------------------
// winFileDialog.cpp
//-------------------------------------------
// USES HEAP MEMORY
// If a default is provided, automatically excludes files of that name.

#include <myDebug.h>
#include "winFileDialog.h"
#include "buttons.h"
#include "myTFT.h"
#include "fileSystem.h"




#define dbg_fd		0
	// debug the dialog itself
#define dbg_files   1
	// 0 = show them after sorting
	// -1 = show whole process


#define NUM_PER_WINDOW  	8
#define LINE_HEIGHT     	33

#define BUTTON_CANCEL		4
#define BUTTON_UP			12
#define BUTTON_ACCEPT		17
#define BUTTON_DOWN			22


typedef struct fileEntry
{
	bool is_default;
	fileEntry *next;
	fileEntry *prev;
	char name[MAX_RIG_NAME + 1];
} fileEntry_t;


static int num_entries;
static fileEntry_t *first_entry;
static fileEntry_t *last_entry;

static int top;
static int selected;
static int last_selected;
static bool draw_needed;

static char selected_filename[MAX_RIG_NAME + 1];


//-------------------------------------------
// DERIVED CLASSES
//-------------------------------------------
#include "configOptions.h"
#include "rigParser.h"

#define dbg_rfd  0

rigFileDialog rig_file_dlg;


void rigFileDialog::begin(bool cold)
{
	display(dbg_rfd,0,"rigFileDialog::begin(%d)",cold);
	setup(OPTION_LOAD_RIG,"Load Rig ..","/",".rig",DEFAULT_RIG_TOKEN);
	winFileDialog::begin(cold);
}


void rigFileDialog::onButton(int row, int col, int event)
{
    int num = row * NUM_BUTTON_COLS + col;
	if (num == BUTTON_ACCEPT)
	{
		display(dbg_rfd,0,"rigFileDialog::onButton(%d) calling parseRig(%s)",num,selected_filename);
		if (parseRig(selected_filename,1))
		{
			display(dbg_rfd,0,"rigFileDialog::onButton(%d) calling endWindow(0x%04x)",num,m_id);
			endWindow(m_id);
		}
	}
	else
		winFileDialog::onButton(row,col,event);
}




//-------------------------------------------
// BASE CLASS
//-------------------------------------------
// file access

static void addEntry(const char *name, bool is_default)
{
	display(dbg_files+1,"addEntry(%d,%s,%d)",num_entries,name,is_default);

	fileEntry_t *entry = new fileEntry_t;
	strcpy(entry->name,name);
	entry->is_default = is_default;
	num_entries++;

	entry->next = 0;
	entry->prev = first_entry;
	if (!first_entry)
		first_entry = entry;
	if (last_entry)
		last_entry->next = entry;
	last_entry = entry;
}



static int cmpEntries(fileEntry_t *at, fileEntry_t *next)
	// returns 1 if at > next
{
	int retval = -1;
	if (at->is_default)
		retval = 0;
	else
	{
		int len1 = strlen(at->name);
		int len2 = strlen(next->name);
		int pos1 = 0;
		int pos2 = 0;
		while (pos1 < len1 && pos2 < len2)
		{
			char c1 = at->name[pos1];
			char c2 = next->name[pos2];

			// case insensitive compare
			if (c1 >= 'a' && c1 <= 'z')
				c1 = c1 - 'a' + 'A';
			if (c2 >= 'a' && c2 <= 'z')
				c2 = c2 - 'a' + 'A';

			if (c1 > c2)
			{
				retval = 1;
				break;
			}
			if (c1 < c2)
			{
				retval = 0;
				break;
			}
			pos1++;
			pos2++;
		}
		if (retval == -1)
		{
			retval = pos1 < len1 ? 1 : 0;
		}
	}
	display(dbg_files+1,"cmpEntries(%s,%s)=%d",at->name,next->name,retval);
	return retval;
}


void winFileDialog::getFilenames()
{
	display(dbg_files,"getFilenames()",0);
	proc_entry();

	num_entries = 0;
	first_entry = 0;
	last_entry = 0;

	if (m_default)
		addEntry(m_default,1);

    myFileType_t the_dir = SD.open(m_path);
    if (!the_dir)
    {
        my_error("Could not opendir %s",m_path);
		proc_leave();
		return;
    }

	// scan the directory

	int ext_len = strlen(m_ext);
    myFileType_t entry = the_dir.openNextFile();
    while (entry)
    {
        if (!entry.isDirectory())
        {
			char filename[255];	 // arbitrary max
			entry.getName(filename, sizeof(filename));
			int len = strlen(filename);
			int ext_pos = len - ext_len;
			if (len >= MAX_RIG_NAME + ext_len)
			{
				warning(dbg_files,"filename too long: %s",filename);
			}
			else if (len && ext_pos > 0 && !strcmp(&filename[ext_pos],m_ext))
			{
				filename[ext_pos] = 0;
				if (m_default && !strcmp(m_default,filename))
				{
					warning(dbg_files,"skipping file that maches default filename: %s",filename);
				}
				else if (legalFilename(filename))
					addEntry(filename,0);
				else
				{
					warning(dbg_files,"skipping illegal filename: %s",filename);
				}
			}
		}
        entry.close();
        entry = the_dir.openNextFile();
    }   // while (entry)
    the_dir.close();

	// sort em

	proc_leave();
	display(dbg_files+1,"sorting .......",0);
	proc_entry();

    fileEntry_t *at = first_entry;
    while (at && at->next)
    {
		fileEntry_t *next = at->next;

        int rslt = cmpEntries(at,next);
        if (rslt)
        {
			display(dbg_files+1,"    switchEntries(%s,%s)",at->name,next->name);
			char holder[MAX_RIG_NAME + 1];
			strcpy(holder,at->name);
			strcpy(at->name,next->name);
			strcpy(next->name,holder);
			at = at->prev ? at->prev : at;
        }
        else
        {
            at = at->next;
        }
    }

	proc_leave();

	if (dbg_files <= 0)
	{
		display(dbg_files + 1,"sorted",0);

		proc_entry();
		fileEntry_t *at = first_entry;
		while (at)
		{
			display(dbg_files,"name = %s",at->name);
			at = at->next;
		}
		proc_leave();
	}

}


//-------------------------------------------
// end() and begin()
//-------------------------------------------

const char *winFileDialog::getSelectedFilename()
{
	return selected_filename;
}


void winFileDialog::end()
{
	fileEntry_t *at = first_entry;
	first_entry = NULL;
	last_entry = NULL;
	while (at)
	{
		fileEntry_t *next = at->next;
		free(at);
		at = next;
	}
}


void winFileDialog::begin(bool cold)
{
	display(dbg_fd,"winFileDialog::begin(0x%04x,%s,%s,%s,%s)",
		m_id,
		m_name,
		m_path,
		m_ext,
		m_default ? m_default : "null");
	// mem_check("start");
    sysWindow::begin(cold);
	getFilenames();
	// mem_check("after");

    the_buttons.setButtonType(BUTTON_CANCEL, BUTTON_EVENT_CLICK, LED_RED);
	if (num_entries)
	{
		the_buttons.setButtonType(BUTTON_ACCEPT, BUTTON_EVENT_CLICK, LED_GREEN);
		if (num_entries > 1)
		{
			the_buttons.setButtonType(BUTTON_UP, 	 BUTTON_EVENT_CLICK, LED_BLUE);
			the_buttons.setButtonType(BUTTON_DOWN, 	 BUTTON_EVENT_CLICK, LED_BLUE);
		}
	}

	top = 0;
	selected = 0;
	last_selected = -1;
	draw_needed = 1;
	selected_filename[0] = 0;
}


//-------------------------------------------
// onButton
//-------------------------------------------

void winFileDialog::onButton(int row, int col, int event)
{
    int num = row * NUM_BUTTON_COLS + col;
	if (num == BUTTON_UP || num == BUTTON_DOWN)
	{
		int inc = num == BUTTON_UP ? -1 : 1;
		selected = selected + inc;
		if (selected < 0) selected = 0;
		if (selected >= num_entries) selected = num_entries-1;

		if (selected < top)
		{
			top = selected;
			draw_needed = 1;
		}
		else if (selected >= top + NUM_PER_WINDOW)
		{
			top = selected - NUM_PER_WINDOW + 1;
			draw_needed = 1;
		}
	}
	else if (num == BUTTON_CANCEL)
	{
	    endWindow(0);
	}
	else if (num == BUTTON_ACCEPT)
	{
		endWindow(m_id);
	}
}


//-------------------------------------------
// updateUI
//-------------------------------------------

void winFileDialog::updateUI()
{
	if (draw_needed && !num_entries)
	{
		mylcd.setFont(Arial_20);
		mylcd.setTextColor(TFT_RED);
		mylcd.drawString("NO FILES FOUND!!!",25,client_rect.ys+10);
		draw_needed = 0;
		return;
	}

	int num = 0;
	fileEntry *entry = first_entry;
	int y = client_rect.ys + 5;
	bool sel_changed = last_selected != selected;

	while (entry && num<top+NUM_PER_WINDOW)
	{
		if (num >= top)
		{
			if (draw_needed || (
				sel_changed &&
				(num == selected || num == last_selected)))
			{
				uint16_t fc = TFT_YELLOW;
				uint16_t bc = TFT_BLACK;
				if (num == selected)
				{
					fc = TFT_WHITE;
					bc = TFT_BLUE;

					// note that this is where we copy the entry
					// name into the global selected_filename!

					strcpy(selected_filename,entry->name);
				}

				mylcd.setTextColor(fc);
				mylcd.fillRect(0,y,client_rect.width()-1,LINE_HEIGHT,bc);
				mylcd.drawString(entry->name,25,y+5);
			}
			y += LINE_HEIGHT;
		}
		entry = entry->next;
		num++;
	}

	draw_needed = 0;
	last_selected = selected;

}
