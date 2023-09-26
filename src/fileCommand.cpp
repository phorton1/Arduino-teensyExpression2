//----------------------------------------------------------
// fileCommand.cpp
//----------------------------------------------------------
// contains fileCommand() and associated methods

#include "fileSystem.h"
#include "prefs.h"
#include <myDebug.h>
#include <Base64.h>
#include <TeensyThreads.h>


// The following debugging *may* affect timing.

#define dbg_queue 0

#define dbg_hdr	  0
	// 0 = show a header for any file system command
	// -1 = show entries
	// -2 = show parsing
#define dbg_cmd	  0
	// 0 = show file commands
	// -1 = show details


#define TEST_DELAY    0		// 2000
	// delay certain operations to test progress dialog etc
	// set to 1000 or 2000 ms to slow things down

#define COMMAND_TIMEOUT  15000	// ms
#define QUEUE_TIMEOUT    5		// ms

#define MAX_ACTIVE_COMMANDS 10
#define MAX_QUEUED_BUFFERS  10

#define MAX_DECODED_BUF   10240
    // the decoded base64 will always be less than the raw base64
    // and 10240 is agreed upon between console.pm and teensyExpression.


// structure for getNextEntry

typedef struct
{
	char size[10];
	char ts[22];
	char entry[255];
	bool is_dir;
}   textEntry_t;



//---------------------------------------------
// command_queue
//---------------------------------------------

static void fileCommand(int req_num);	// forward
	// expects the initial_buffer to be first in the queue

static volatile int queue_sem;
static volatile int num_active_commands;
static command_queue_t command_queue[MAX_ACTIVE_COMMANDS];
static bool missing_command_queue_reported;
	// implentation only reports one per program invocation
	// for debugging.


static bool waitQueueSem(int sem_level)
{
	uint32_t started = millis();
	while (queue_sem > sem_level && millis() - started < QUEUE_TIMEOUT)
	{
		delay(1);
	}
	if (queue_sem <= sem_level)
	{
		queue_sem++;
		return true;
	}
	my_error("timed out in waitQueueSem(%d)",sem_level);
	return false;
}


command_queue_t *getCommand(int req_num, int sem_level /* = 0*/)
{
	command_queue_t *retval = NULL;
	if (waitQueueSem(sem_level))
	{
		for (int i=0; i<num_active_commands; i++)
		{
			if (command_queue[i].req_num == req_num)
			{
				retval = &command_queue[i];
				break;
			}
		}
		queue_sem--;
	}
	return retval;
}



bool startCommand(int req_num, char *initial_buffer)
{
	display_level(dbg_queue,1,"startCommand(%d) buf_len(%d)",
		req_num,
		strlen(initial_buffer));

	bool ok = 0;
	if (waitQueueSem(0))
	{
		if (num_active_commands >= MAX_ACTIVE_COMMANDS -1)
		{
			my_error("startCommand(%d) command num_active_commands overflow",req_num);
		}
		else if (getCommand(req_num,queue_sem))
		{
			my_error("startCommand(%d) already active in addCommand()",req_num);
		}
		else
		{
			command_queue_t *queue = &command_queue[num_active_commands++];
			memset(queue,0,sizeof(command_queue_t));
			queue->req_num = req_num;
			queue->buffer[queue->tail++] = initial_buffer;
			ok = 1;
		}
		queue_sem--;
	}

	if (ok)
	{
		threads.addThread(fileCommand,req_num,10240);
				// 10240 stack size
	}

	return ok;
}


static void endCommand(int req_num)
	// only called by the threaded fileCommand()
{
	display_level(dbg_queue,1,"endCommand(%d)",req_num);

	if (waitQueueSem(0))
	{
		bool found = 0;
		for (int i=0; i<num_active_commands; i++)
		{
			if (command_queue[i].req_num == req_num)
			{
				int head = command_queue[i].head;
				int tail = command_queue[i].head;
				char **buffer = command_queue[i].buffer;
				while (head != tail)
				{
					free(buffer[head++]);
					if (head >= MAX_QUEUED_BUFFERS)
						head = 0;
				}
				memset(&command_queue[i],0,sizeof(command_queue_t));
				found = true;
				num_active_commands--;
			}
			if (found)
			{
				memcpy(&command_queue[i+1],&command_queue[i],sizeof(command_queue_t));
			}
		}
		queue_sem--;
	}
}


bool addCommandQueue(int req_num, char *buf)
{
	bool retval = false;
	display_level(dbg_queue,1,"addCommandQueue(%d) buf_len(%d)",
		req_num,
		strlen(buf));

	if (waitQueueSem(0))
	{
		command_queue_t *queue = getCommand(req_num,queue_sem);
		if (!queue)
		{
			my_error("addCommandQueue() could not find command_queue(%d)",req_num);
		}
		else
		{
			int next_tail = queue->tail + 1;
			if (next_tail > MAX_QUEUED_BUFFERS)
				next_tail = 0;

			if (next_tail == queue->head)
			{
				my_error("command_queue(%d) overflow",req_num);
			}
			else
			{
				display(dbg_queue+1,"    adding at %d",next_tail);
				queue->buffer[queue->tail++] = buf;
				retval = true;
			}
		}
		queue_sem--;
	}
	return retval;
}


static char *getCommandQueue(int req_num)
	// only called by threaded doCommand()
{
	char *retval = NULL;
	if (waitQueueSem(0))
	{
		command_queue_t *queue = getCommand(req_num,queue_sem);
		if (!queue)
		{
			if (!missing_command_queue_reported)
				my_error("getCommandQueue() could not find command_queue(%d)",req_num);
			missing_command_queue_reported = 1;
		}
		else if (queue->head != queue->tail)
		{
			retval = queue->buffer[queue->head];
			queue->buffer[queue->head] = 0;
			queue->head++;
			display_level(dbg_queue,2,"getCommandQueue(%d) returning buf_len(%d)",
				req_num,
				strlen(retval));

			if (queue->head >= MAX_QUEUED_BUFFERS)
				queue->head = 0;
		}
		queue_sem--;
	}
	return retval;
}



//---------------------------------------
// reply methods
//---------------------------------------


static void fileReplyEnd(Stream *fsd, int req_num)
{
    fsd->print("file_reply_end(");
 	fsd->print(req_num);
    fsd->print(")\r\n");
}

static void fileReply(Stream *fsd, int req_num, bool is_dir, uint32_t size, const char *ts,  const char *entry)
{
	fsd->print("file_reply(");
	fsd->print(req_num);
	fsd->print("):");
	if (!is_dir)
		fsd->print(size);
	fsd->print("\t");
	fsd->print(ts);
	fsd->print("\t");
	fsd->print(entry);
	if (is_dir && strcmp(entry,"/"))
		fsd->print("/");
	fsd->print("\r\n");
}

static void fileReplyError(Stream *fsd, int req_num, const char *format, ...)
{
	char buffer[255];

	va_list var;
	va_start(var, format);
	vsprintf(buffer,format,var);

	my_error(buffer,0);

	fsd->print("file_reply(");
	fsd->print(req_num);
	fsd->print("):");
	fsd->print("ERROR - ");
	fsd->print(buffer);
	fsd->print("\r\n");
}

void sendProgressADD(Stream *fsd,int req_num, int num_dirs, int num_files)
{
	fsd->print("file_reply(");
	fsd->print(req_num);
	fsd->print("):");
	fsd->print("PROGRESS\tADD\t");
	fsd->print(num_dirs);
	fsd->print("\t");
	fsd->print(num_files);
	fsd->print("\r\n");
	fileReplyEnd(fsd,req_num);
}

void sendProgressENTRY(Stream *fsd, int req_num, const char *entry)
{
	fsd->print("file_reply(");
	fsd->print(req_num);
	fsd->print("):");
	fsd->print("PROGRESS\tENTRY\t");
	fsd->print(entry);
	fsd->print("\r\n");
	fileReplyEnd(fsd,req_num);
}

void sendProgressDONE(Stream *fsd, int req_num, bool is_dir)
{
	fsd->print("file_reply(");
	fsd->print(req_num);
	fsd->print("):");
	fsd->print("PROGRESS\tDONE\t");
	fsd->print(is_dir);
	fsd->print("\r\n");
	fileReplyEnd(fsd,req_num);
}

static void fileReplyAborted(Stream *fsd, int req_num)
{
	fsd->print("file_reply(");
	fsd->print(req_num);
	fsd->print("):ABORTED\r\n");
}


static bool abortPending(Stream *fsd, int req_num, const char *command)
{
	char *pending = getCommandQueue(req_num);
	if (pending)
	{
		if (!strncmp(pending,"ABORT",5))
		{
			display_level(dbg_cmd,3,"ABORTING fileCommand(%d,%s)!!",req_num,command);
			fileReplyAborted(fsd,req_num);
		}
		free(pending);
		return true;
	}
	return false;
}



//---------------------------------------------
// atomic commands
//---------------------------------------------

static void doList(Stream *fsd, int req_num, const char *dir)
{
	display_level(dbg_cmd,2,"fileCommand::doList(%s)",dir);

	myFileType_t the_dir = SD.open(dir);
	if (!the_dir)
	{
		fileReplyError(fsd,req_num,"Could not open directory %s",dir);
		return;
	}

	const char *ts = getTimeStamp(&the_dir);
	fileReply(fsd,req_num,1,0,ts,dir);

	myFileType_t entry = the_dir.openNextFile();
	while (entry)
	{
		char filename[255];
		entry.getName(filename, sizeof(filename));
		const char *ts = getTimeStamp(&entry);
		uint32_t size = entry.size();

		fileReply(fsd,req_num,entry.isDirectory(),size,ts,filename);
		entry = the_dir.openNextFile();

	}   // while (entry)
}


static void makeDir(Stream *fsd, int req_num, const char *dir, const char *name)
{
	// name has trailing slash removed

	char path[255];
	strcpy(path,dir);
	strcat(path,"/");
	strcat(path,name);

	display_level(dbg_cmd,2,"fileCommand::makeDir(%s,%s) path=%s",dir,name,path);

	if (SD.exists(path))
	{
		fileReplyError(fsd,req_num,"Dir/File %s already exists",name);
		return;
	}

	// excluded code to set timestamp on dir
	// strcpy(write_timestamp,ts);

	#if USE_OLD_FAT
		// FatFile::dateTimeCallback(dtCallback);
		int rslt = SD.mkdir(path);
		// FatFile::dateTimeCallbackCancel();
	#else
		// FsDateTime::setCallback(dtCallback);
		int rslt = SD.mkdir(path);
		// FsDateTime::clearCallback();
	#endif

	if (!rslt)
	{
		fileReplyError(fsd,req_num,"Could not make directory %s",name);
		return;
	}

	doList(fsd,req_num,dir);
}


static void renameObj(Stream *fsd, int req_num, const char *dir, const char *name1, const char *name2)
{
	// names already have trailing /'s removed

	char path1[255];
	strcpy(path1,dir);
	strcat(path1,"/");
	strcat(path1,name1);

	char path2[255];
	strcpy(path2,dir);
	strcat(path2,"/");
	strcat(path2,name2);

	myFileType_t file = SD.open(path1);
	if (!file)
	{
		fileReplyError(fsd,req_num,"Could not open %s in order to rename",name1);
		return;
	}
	bool is_dir = file.isDirectory();
	const char *ts = getTimeStamp(&file);
	uint32_t size = file.size();
	file.close();

	display_level(dbg_cmd,2,"fileCommand::renameObj(%s,%s,%s) path1=%s path2=%s",dir,name1,name2,path1,path2);

	if (!SD.rename(path1,path2))
	{
		fileReplyError(fsd,req_num,"Could not rename %s to %s",name1,name2);
		return;
	}
	fileReply(fsd,req_num,is_dir,size,ts,name2);
}


static bool deleteObj(Stream *fsd, int req_num, const char *dir, const char *entry, bool last)
{
	char path[255];
	strcpy(path,dir);
	if (strcmp(dir,"/"))
		strcat(path,"/");
	strcat(path,entry);
    display_level(dbg_cmd,2,"fileCommand::deleteObj(%s)",path);
	sendProgressENTRY(fsd,req_num,path);

	#if TEST_DELAY
		delay(TEST_DELAY);
	#endif

	myFileType_t the_file = SD.open(path);

	bool ok = 1;
	if (the_file)
	{
		bool is_dir = the_file.isDirectory();
		if (is_dir)
		{
			ok = the_file.rmRfStar();		// remove directory and contents
		}
		else
		{
			the_file.close();
			ok = SD.remove(path);
		}

		#if TEST_DELAY
			delay(TEST_DELAY);
		#endif

		if (ok)
			sendProgressDONE(fsd,req_num,is_dir);
		else
			fileReplyError(fsd,req_num,"could not remove %s",path);
	}
	else
	{
		ok = 0;
		fileReplyError(fsd,req_num,"could not open %s for delete",path);
	}

	if (last && ok)
		doList(fsd,req_num,dir);

	return ok;
}



//-------------------------------------------------------
// getNextEntry()
//-------------------------------------------------------

static int getNextEntry(Stream *fsd, int req_num, textEntry_t *the_entry, const char **ptr)
	// parser for commands that have lists of entries
	// pass in ptr, starting at the list of entries
	// returns 0 if no entry, -1 if error, or 1 if entry
{
	the_entry->size[0] = 0;
	the_entry->ts[0] = 0;
	the_entry->entry[0] = 0;
	the_entry->is_dir = 0;

	if (!**ptr)
		return 0;

	// we set all 3 fields or fail
	// there is a tab after each entry

	int num_params = 0;
	char *out = the_entry->size;

	while (**ptr)
	{
		if (**ptr == '\t' ||
			**ptr == '\r')
		{
			*out = 0;
			num_params++;
			if (num_params == 1)
				out  = the_entry->ts;
			else if (num_params == 2)
				out = the_entry->entry;
			else if (num_params == 3 && *(out-1) == '/')
				// get rid of terminating '/' on dir entries
			{
				the_entry->is_dir = 1;
				*(out-1) = 0;
			}

			bool is_cr = (**ptr == '\r');
			(*ptr)++;
			if (is_cr)
				break;
		}
		else
		{
			*out++ = *(*ptr)++;
		}
	}

	if (num_params != 3)
	{
		fileReplyError(fsd,req_num,"Incorrect number of fields(%d) in fileEntry",num_params);
		return -1;
	}

	return 1;
}



//-------------------------------------------------------
// parseCommand()
//-------------------------------------------------------

#define MAX_COMMAND		10 	 // PROGRESS is only 8
#define MAX_PARAMS 		3

static char *parseCommand(		// returns buffer which must be freed if returned
	int req_num,				// takes a req_num
	bool expected,				// whether to report failure to get buffer
	int *num_params,			// filled in number of parameters
	const char **params,		// null terminates command, fills in param pointers (will null terminate BASE64 CONTENT)
	const char **entries)		// returns pointer to entries which may be NULL_ptr
{
    display_level(dbg_hdr+1,2,"parseCommand(%d,%d)",req_num,expected);
	char *buf = getCommandQueue(req_num);
	if (!buf)
	{
		if (expected)
			my_error("no file_command(%d) buffer",req_num);
		return NULL;
	}

	char *in = buf;
	*num_params = 0;
	while (*in && *in != '\r')
	{
		display_level(dbg_hdr+3,4,"parseCommand(%d) in='%c' 0x%02x",req_num,*in>=' '?*in:'.',*in);
		if (*in == '\t')
		{
			*in++ = 0;
			if (*num_params < MAX_PARAMS)
			{
				params[(*num_params)++] = in;
				display_level(dbg_hdr+2,3,"parseCommand(%d) num_params=%d",req_num,*num_params);
			}

		}
		else
		{
			in++;
		}
	}
	if (*in == '\r')
		*in++ = 0;
	*entries = in;

	for (int i=*num_params; i<MAX_PARAMS; i++)
	{
		params[i] = "";
	}
	return buf;

}



//-------------------------------------------------------
// fileCommand()
//-------------------------------------------------------

static void fileCommand(int req_num)
	// buf is pointing at req_num \t
	// the buf we are passed must be freed when done!!
{
    display_level(dbg_hdr+1,1,"fileCommand(%d)",req_num);
	Stream *fsd = ACTIVE_FILE_SYS_DEVICE;

	// get and parse the command buffer
	// command must be freed after this

	int num_params;
	const char *entries;
	const char *param[MAX_PARAMS];
	char *command = parseCommand(req_num, true, &num_params, &param[0], &entries);
	if (!command)
	{
		endCommand(req_num);
		return;
	}

	char dbg_buf[18];
	const char *dbg_ptr = param[2];
	if (!strcmp(command,"BASE64"))
	{
		sprintf(dbg_buf,"bytes(%d)",strlen(dbg_ptr));
		dbg_ptr = dbg_buf;
	}
    display_level(dbg_hdr,1,"fileCommand(%d) %s(%s,%s,%s)",
		req_num,
		command,
		param[0],
		param[1],
		dbg_ptr);


	//--------------------------
	// parse entries if any
	//--------------------------

	textEntry_t the_entry;
	const char *ptr = entries;

	int num_dirs = 0;
	int num_files = 0;
	int rslt = getNextEntry(fsd,req_num,&the_entry,&ptr);
	while (rslt == 1)
	{
		if (the_entry.is_dir)
			num_dirs++;
		else
			num_files++;

		display_level(dbg_hdr+1,3,"entry(%s) is_dir(%d) size(%s) ts(%s)",
			the_entry.entry, the_entry.is_dir, the_entry.size, the_entry.ts);
		rslt = getNextEntry(fsd,req_num,&the_entry,&ptr);
	}
	if (rslt == -1)	// error already reported
	{
		free(command);
		return;
	}

	//--------------------------------------
	// do the commands
	//--------------------------------------

	if (!strcmp(command,"LIST"))
	{
		doList(fsd,req_num,param[0]);
	}
	else if (!strcmp(command,"MKDIR"))
	{
		makeDir(fsd,req_num,param[0],param[1]);
	}
	else if (!strcmp(command,"RENAME"))
	{
		renameObj(fsd,req_num,param[0],param[1],param[2]);
	}
	else if (!strcmp(command,"DELETE"))
	{
		if (num_params == 2)		// single_file item is in param[1]
		{
			deleteObj(fsd,req_num,param[0],param[1],1);
		}

		// only cases where the fileClientPane puts up a progress dialog
		// need check for pending aborts

		else if (!abortPending(fsd, req_num, command))
		{
			// process entry list
			sendProgressADD(fsd,req_num,num_dirs,num_files);

			ptr = entries;
			int rslt = getNextEntry(fsd,req_num,&the_entry,&ptr);
			bool last = !*ptr || *ptr == '\r';
			while (rslt == 1)
			{
				if (abortPending(fsd, req_num, command))
					break;
				rslt = deleteObj(fsd,req_num,param[0],the_entry.entry,last);
				rslt = rslt && getNextEntry(fsd,req_num,&the_entry,&ptr);
				last = !*ptr || *ptr == '\r';
			}
		}
	}
	else
	{
		fileReplyError(fsd,req_num,"Unknown Command %s",command);
	}


	fileReplyEnd(fsd,req_num);
	display_level(dbg_hdr,1,"fileCommand(%d,%s) done",req_num,command);
	free(command);
	endCommand(req_num);

}	// fileCommand


// end of fileCommand.cpp
