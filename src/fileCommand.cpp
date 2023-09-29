//----------------------------------------------------------
// fileCommand.cpp
//----------------------------------------------------------
// contains fileCommand() and associated methods

#include "fileSystem.h"
#include "prefs.h"
#include <myDebug.h>
#include <TeensyThreads.h>
#include <Base64.h>


// The following debugging *may* affect timing.

#define dbg_queue 0

#define dbg_queue  0
	//  0 = show queue actions
#define dbg_parse  -1
	//  0 = show command parse header
	// -1 = show parameters found and entries
	// -2 = show every character in parseCommand
#define dbg_hdr	  0
	//  0 = show a msg after parsing and at end of command
	// -1 = show msg at top of doCommand
#define dbg_cmd	  -1
	//  0 = show file commands
	// -1 = show command details


#define TEST_DELAY    0		// 2000
	// delay certain operations to test progress dialog etc
	// set to 1000 or 2000 ms to slow things down

#define MAX_PARAMS	  3
	// maximum number of paremeters per command

#define QUEUE_TIMEOUT    5		// ms
	// timeout for semaphore to get access to queue
#define FILE_TIMEOUT	15000	// ms
	// amount of time to wait for the next BASE64 packet

#define THREAD_STACK_SIZE   4096
	// stack for doCommand()
#define MAX_DIRECTORY_BUF   4096
	// maximum size of a directory listing returned by _list

#define MAX_FILENAME    255
	// maximum length of fully qualified filename supported by fileCommands
#define MAX_FILE_BUF	10000
	// 10000 is agreed upon limit in Perl
#define MAX_DECODED_BUF    (MAX_FILE_BUF + 5)
	// allows 10000 + 4 byte checksum + null terminator
	// invariantly allocated in PUT
	// decoded buffer is allocated to size in BASE64
#define MAX_ENCODED_BUF	   14000
	// must be big enough to encode MAX_DECODED_BUF
	// invariantly allocated in PUT
#define SIZE_TIMESTAMP	   20
	// YYYY-MM-DD HH:MM:SS plus nul terminator
#define DISK_FULL_MARGIN	(1024 * 1024)
	// leave at least 1MB free during FILE command



// structure for getNextEntry

typedef struct
{
	char size[10];
	char ts[22];
	char entry[255];
	bool is_dir;
}   textEntry_t;



//---------------------------------------------
// commands
//---------------------------------------------

static void fileCommand(int req_num);	// forward
	// expects the initial_buffer to be first in the queue

static volatile int command_sem;
static fileCommand_t *commands[MAX_ACTIVE_COMMANDS];
	// pointer to a command remains valid through it's lifetime

static bool missing_command_reported;
	// implentation only reports one per program invocation
	// for debugging.


static bool waitCommandSem(int sem_level)
{
	uint32_t started = millis();
	while (command_sem > sem_level && millis() - started < QUEUE_TIMEOUT)
	{
		delay(1);
	}
	if (command_sem <= sem_level)
	{
		command_sem++;
		return true;
	}
	my_error("timed out in waitCommandSem(%d)",sem_level);
	return false;
}


fileCommand_t *getCommand(int req_num, int sem_level /* = 0*/)
{
	fileCommand_t *retval = NULL;
	if (waitCommandSem(sem_level))
	{
		for (int i=0; i<MAX_ACTIVE_COMMANDS; i++)
		{
			if (commands[i] && commands[i]->req_num == req_num)
			{
				retval = commands[i];
				break;
			}
		}
		command_sem--;
	}
	return retval;
}



bool startCommand(int req_num, char *initial_buffer)
{
	display_level(dbg_queue,1,"startCommand(%d) buf_len(%d)",
		req_num,
		strlen(initial_buffer));

	bool ok = 0;
	if (waitCommandSem(0))
	{
		if (getCommand(req_num,command_sem))
		{
			my_error("startCommand(%d) already active in addCommand()",req_num);
		}
		else
		{
			int cmd_num = -1;
			for (int i=0; i<MAX_ACTIVE_COMMANDS; i++)
			{
				if (!commands[i])
				{
					cmd_num = i;
					break;
				}
			}

			if (cmd_num == -1)
			{
				my_error("startCommand(%d) number commands overflow",req_num);
			}
			else
			{
				// mem_check("before new fileCommand_t");
				fileCommand_t *cmd = (fileCommand_t *) malloc(sizeof(fileCommand_t));
				// mem_check("after new fileCommand_t");

				if (!cmd)
				{
					my_error("unable to allocate new fileCommand_t(%d)",req_num);
				}
				else
				{
					memset(cmd,0,sizeof(fileCommand_t));
					cmd->req_num = req_num;
					cmd->queue[cmd->tail++] = initial_buffer;
					commands[cmd_num] = cmd;
					ok = 1;
				}
			}	// found an empty slot for command
		}	// command doesn't already exist

		command_sem--;
	}

	if (ok)
	{
		threads.addThread(fileCommand,req_num,10240);
	}

	return ok;
}


static void endCommand(int req_num)
	// only called by the threaded fileCommand()
{
	display_level(dbg_queue,1,"endCommand(%d)",req_num);

	if (waitCommandSem(0))
	{
		for (int i=0; i<MAX_ACTIVE_COMMANDS; i++)
		{
			fileCommand_t *cmd = commands[i];
			if (cmd && cmd->req_num == req_num)
			{
				int head = cmd->head;
				int tail = cmd->head;
				char **queue = commands[i]->queue;
				while (head != tail)
				{
					free(queue[head++]);
					if (head >= MAX_QUEUED_BUFFERS)
						head = 0;
				}
				free(cmd);
				commands[i] = 0;
				command_sem--;
				return;
			}
		}
		my_error("Could not endCommand(%d)",req_num);
		command_sem--;
	}
}


bool addCommandQueue(int req_num, char *buf)
{
	bool retval = false;
	display_level(dbg_queue,1,"addCommandQueue(%d) buf_len(%d)",
		req_num,
		strlen(buf));

	if (waitCommandSem(0))
	{
		fileCommand_t *cmd = getCommand(req_num,command_sem);
		if (!cmd)
		{
			my_error("addCommandQueue() could not find command(%d)",req_num);
		}
		else
		{
			int next_tail = cmd->tail + 1;
			if (next_tail > MAX_QUEUED_BUFFERS)
				next_tail = 0;

			if (next_tail == cmd->head)
			{
				my_error("commands(%d) overflow",req_num);
			}
			else
			{
				display(dbg_queue+1,"    adding at %d",next_tail);
				cmd->queue[cmd->tail++] = buf;
				retval = true;
			}
		}
		command_sem--;
	}
	return retval;
}


static char *getCommandQueue(int req_num)
	// returns buffer which must be freed by caller
	// and pointer to the command for passing to _methods
{
	char *retval = NULL;
	if (waitCommandSem(0))
	{
		fileCommand_t *cmd = getCommand(req_num,command_sem);
		if (!cmd)
		{
			if (!missing_command_reported)
				my_error("getCommandQueue() could not find commands(%d)",req_num);
			missing_command_reported = 1;
		}
		else if (cmd->head != cmd->tail)
		{
			retval = cmd->queue[cmd->head];
			cmd->queue[cmd->head] = 0;
			cmd->head++;
			if (cmd->head >= MAX_QUEUED_BUFFERS)
				cmd->head = 0;

			display_level(dbg_queue,2,"getCommandQueue(%d) returning buf_len(%d)",
				req_num,
				strlen(retval));
		}
		command_sem--;
	}
	return retval;
}


//---------------------------------------
// reply methods
//---------------------------------------

static void fileReplyError(Stream *fsd, int req_num, const char *format, ...)
{
	char buffer[255];
	char format_buffer[255];
	sprintf(format_buffer,"file_reply(%d):ERROR - %s\r\n",req_num,format);

	va_list var;
	va_start(var, format);
	vsprintf(buffer,format_buffer,var);
	my_error(buffer,0);
	fsd->print(buffer);
}


void fileReply(Stream *fsd, int req_num, const char *format, ...)
{
	char buffer[255];
	char format_buffer[255];
	sprintf(format_buffer,"file_reply(%d):%s\r\n",req_num,format);

	va_list var;
	va_start(var, format);
	vsprintf(buffer,format_buffer,var);
	fsd->print(buffer);
}



//-------------------------------------------------------
// Command and Entry Parsers
//-------------------------------------------------------

static int parseCommand(		// returns num_params
	char *buf,					// buffer to parse == the command
	const char **params,		// null terminates command, fills in param pointers (will null terminate BASE64 CONTENT)
	const char **entries = 0)	// optional returns pointer to entries which may be NULL_ptr
{
	char *in = buf;
	int num_params = 0;
	while (*in && *in != '\r')
	{
		display_level(dbg_parse+2,4,"parseCommand() in='%c' 0x%02x",*in>=' '?*in:'.',*in);
		if (*in == '\t')
		{
			*in++ = 0;
			if (num_params < MAX_PARAMS)
			{
				params[num_params++] = in;
				display_level(dbg_parse+1,3,"parseCommand() num_params=%d",num_params);
			}
		}
		else
		{
			in++;
		}
	}
	if (*in == '\r')
		*in++ = 0;
	if (entries)
		*entries = in;
	for (int i=num_params; i<MAX_PARAMS; i++)
	{
		params[i] = "";
	}
	return num_params;
}


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


//---------------------------------------------
// atomic commands
//---------------------------------------------

static void _list(Stream *fsd, int req_num, const char *dir)
{
	display_level(dbg_cmd,2,"LIST(%d,%s)",req_num,dir);
	// mem_check("_list");

	char *buffer = (char *) malloc(MAX_DIRECTORY_BUF);
	if (!buffer)
	{
		my_error("could not allocate memory",0);
		delay(1000);
		fileReplyError(fsd,req_num,"LIST(%d,%s) could not allocate buffer",req_num,dir);
		return;
	}

	myFile_t the_dir = SD.open(dir);
	if (!the_dir)
	{
		my_error("could not opendir(%s)",dir);
		delay(1000);
		fileReplyError(fsd,req_num,"LIST(%d,%s) could not open directory",req_num,dir);
		free(buffer);
		return;
	}

	const char *ts = getTimeStamp(&the_dir);

	sprintf(buffer,"file_reply(%d):0\t%s\t%s%s\r",
		req_num,
		ts,
		dir,
		strcmp(dir,"/")?"/":"");

	unsigned int at = strlen(buffer);
	char *out = &buffer[at];

	myFile_t entry = the_dir.openNextFile();
	while (entry)
	{
		char name[255];
		entry.getName(name, sizeof(name));
		display(0,"got name(%s)",name);

		const char *ts = getTimeStamp(&entry);
		uint32_t size = entry.size();
		bool is_dir = entry.isDirectory();

		#define MAX_FILE_SIZE_CHARS   10	// 9GB

		display_level(dbg_cmd+1,3,"_list at(%d) is_dir(%d) size(%lu) ts(%d) name(%s)",at,is_dir,size,ts,name);

		if (at > MAX_DIRECTORY_BUF - MAX_FILE_SIZE_CHARS - 1 - strlen(ts) - 1 - strlen(name) - 2)
		{
			// report this as an error here, but not to client
			my_error("LIST(%d,%s) not reporting directory buffer overflow at %d!",req_num,dir,at);
			break;
		}

		sprintf(out,"%lu\t%s\t%s%s\r",size,ts,name,is_dir?"/":"");
		at += strlen(out);
		out = &buffer[at];
		entry = the_dir.openNextFile();

	}   // while (entry)

	the_dir.close();
	sprintf(out,"\n");
	fsd->printf(buffer);
	free(buffer);
}


static void _mkdir(Stream *fsd, int req_num, const char *dir, const char *name)
{
	// name has trailing slash removed

	char path[255];
	strcpy(path,dir);
	strcat(path,"/");
	strcat(path,name);

	display_level(dbg_cmd,2,"MKDIR(%s,%s) path=%s",dir,name,path);

	if (SD.exists(path))
	{
		fileReplyError(fsd,req_num,"MKDIR %s already exists",name);
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

	if (rslt)
		_list(fsd,req_num,dir);
	else
		fileReplyError(fsd,req_num,"MKDIR could not make directory %s",name);
}


static void _rename(Stream *fsd, int req_num, const char *dir, const char *name1, const char *name2)
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

	myFile_t file = SD.open(path1);
	if (!file)
	{
		fileReplyError(fsd,req_num,"RENAME Could not open %s",name1);
		return;
	}
	bool is_dir = file.isDirectory();
	const char *ts = getTimeStamp(&file);
	uint32_t size = file.size();
	file.close();

	display_level(dbg_cmd,2,"RENAME(%s,%s,%s) path1=%s path2=%s",dir,name1,name2,path1,path2);

	if (SD.rename(path1,path2))
		fileReply(fsd,req_num,"%d\t%s\t%s%s",size,ts,name2,is_dir?"/":"");
	else
		fileReplyError(fsd,req_num,"Could not RENAME %s to %s",name1,name2);
}



//---------------------------------------------
// session-like _delete command
//---------------------------------------------

static bool abortPending(Stream *fsd, int req_num, const char *command)
{
	char *pending = getCommandQueue(req_num);
	if (pending)
	{
		if (!strncmp(pending,"ABORT",5))
		{
			display_level(dbg_cmd,3,"ABORTING fileCommand(%d,%s)!!",req_num,command);
			fileReply(fsd,req_num,"ABORTED");
		}
		free(pending);
		return true;
	}
	return false;
}


static bool _delete(Stream *fsd, int req_num, const char *dir, const char *entry)
{
	char path[255];
	strcpy(path,dir);
	if (strcmp(dir,"/"))
		strcat(path,"/");
	strcat(path,entry);
    display_level(dbg_cmd,2,"DELETE(%s)",path);
	fileReply(fsd,req_num,"PROGRESS\tENTRY\t%s",entry);

	#if TEST_DELAY
		delay(TEST_DELAY);
	#endif

	myFile_t the_file = SD.open(path);

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
			fileReply(fsd,req_num,"PROGRESS\tDONE\t%d",is_dir);
		else
			fileReplyError(fsd,req_num,"could not DELETE %s",path);
	}
	else
	{
		ok = 0;
		fileReplyError(fsd,req_num,"DELETE could not open %s",path);
	}

	return ok;
}




//-------------------------------------------------------
// session-like _file() command
//-------------------------------------------------------

const char *leafName(const char *filename)
{
	return filename;

	const char *retval = filename;
	while (*filename)
	{
		if (*filename == '/')
			retval = &filename[1];
		filename++;
	}
	return retval;
}


uint32_t calcChecksum(const uint8_t *buf)
{
	uint32_t cs = 0;
	while (*buf) { cs += *buf++; }
	return cs;
}


bool makeSubdirs(Stream *fsd, int req_num, const char *in)
	// expects a fully qualified path name starting with /
	// with a leaf terminal filename
	// makes any needed subdirectories for the file
{
	if (!in || !*in || *in != '/')
	{
		fileReplyError(fsd,req_num,"filename(%s) must be fully qualified",in);
		return 0;
	}


	char path[MAX_FILENAME];
	char *out = path;
	*out++ = *in++;

	while (*in)
	{
		if (*in == '/')
		{
			*out = 0;
			File check_file = SD.open(path);
			if (check_file)
			{
				if (!check_file.isDirectory())
				{
					fileReplyError(fsd,req_num,"attempt to overwrite file(%s) with a subdirectory",path);
					return 0;
				}
			}
			else
			{
				display_level(dbg_cmd+1,3,"FILE making subdir(%s)",path);
				if (!SD.mkdir(path))
				{
					fileReplyError(fsd,req_num,"could not create subdirectory(%s)",path);
					return 0;
				}
			}
		}
		*out++ = *in++;
	}

	return 1;

}



static void _file(Stream *fsd, int req_num, const char *sz_size, const char *ts, const char *file_name)
{
	uint32_t size = atol(sz_size);
	display_level(dbg_cmd,2,"FILE(%s,%u,%s)",leafName(file_name),size,ts);

	//--------------------------------
	// validate _file request
	//--------------------------------

	int name_len = strlen(file_name);
	if ( name_len >= MAX_FILENAME)
	{
		fileReplyError(fsd,req_num,"FILE file(%s) name(%d) too long",leafName(file_name),name_len);
		return;
	}

	#if 0
		# getFreeBytes() is the slow culprit().
		# Instead, we let write() file if there is not room
		uint64_t avail = getFreeBytes();
		uint64_t size_64 = size;
		if (avail <= size_64 + DISK_FULL_MARGIN)
		{
			uint64_t mb = size_64 / BYTES_PER_MB;
			uint32_t mb_32 = mb;
			fileReplyError(fsd,req_num,"FILE(%s) size(%lu) too large to fit in remaining MB(%lu)",leafName(file_name),size,mb_32);
			return;
		}
	#endif

	const char *use_name = file_name;
	char temp_name[MAX_FILENAME + 6];	// temp_name gets the thread id as an extension
	temp_name[0] = 0;

	if (SD.exists(file_name))
	{
		File check_file = SD.open(file_name);
		if (check_file.isDirectory())
		{
			fileReplyError(fsd,req_num,"FILE(%s) is a directory",leafName(file_name));
			check_file.close();
			return;
		}
		check_file.close();
		strcpy(temp_name,file_name);
		sprintf(&temp_name[name_len],".%d",threads.id());
		use_name = temp_name;
	}

	//------------------------------------------
	// allocate buffer and open file
	//------------------------------------------

	char *decoded_buf = (char *) malloc(MAX_DECODED_BUF);
	if (!decoded_buf)
	{
		fileReplyError(fsd,req_num,"FILE could not allocated DECODED_BUF");
		return;
	}

	if (!makeSubdirs(fsd,req_num,use_name))
		return;

	myFile_t the_file = SD.open(use_name,FILE_WRITE);
	if (!the_file)
	{
		fileReplyError(fsd,req_num,"could not open FILE(%s) for output",leafName(file_name));
		free(decoded_buf);
		return;
	}

	//------------------------------------------
	// loop sending CONTINE and getting BASE64
	//------------------------------------------

	bool ok = 1;
	uint32_t offset = 0;
	while (ok && offset < size)
	{
		// send CONTINUE and wait for the BASE64
		// anything else consitutes an error and stops the transfer

		fileReply(fsd,req_num,"CONTINUE");
		uint32_t wait_base64 = millis();
		char *buf = getCommandQueue(req_num);
		while (!buf && millis() - wait_base64 < FILE_TIMEOUT)
		{
			threads.delay(100);		// yield thread for 100 ms
			buf = getCommandQueue(req_num);
		}
		if (!buf)
		{
			my_error("fileCommand::FILE(%s) offset(%d) timeout waiting for buf",leafName(file_name),offset);
			ok = 0;
		}
		else
		{
			if (strncmp(buf,"BASE64",6))
			{
				my_error("FILE(%s) offset(%d) got: %s",leafName(file_name),offset,buf);
				ok = 0;
			}
			else
			{
				//------------------------------------
				// got a BASE64 packet
				//------------------------------------

				const char *param[MAX_PARAMS];
				int num_params = parseCommand(buf, &param[0]);

				uint32_t got_offset = atol(param[0]);
				int32_t got_size   = atol(param[1]);	// limited to 10000
				int32_t encoded_size = strlen(param[2]);
				display_level(dbg_cmd+1,3,"BASE64(%lu,%l) encoded=%l bytes",got_offset,got_size,encoded_size);

				int32_t expected_size = size - offset;
				if (expected_size > MAX_FILE_BUF)
					expected_size = MAX_FILE_BUF;

				if (num_params != 3)
				{
					fileReplyError(fsd,req_num,"BASE64 expects 3 params got(%d)",num_params);
					ok = 0;
				}
				else if (got_size != expected_size)
				{
					fileReplyError(fsd,req_num,"BASE64 got_size(%l) but expected(%l)",got_size,expected_size);
					ok = 0;
				}
				else if (got_offset != offset)
				{
					fileReplyError(fsd,req_num,"BASE64 got_offset(%lu) but expected(%lu)",got_offset,offset);
					ok = 0;
				}

				//--------------------------
				// decode && write
				//--------------------------

				else
				{
					int32_t decoded_size = base64_decode(decoded_buf,(char *) param[2],encoded_size);
					if (decoded_size != got_size+4)
					{
						fileReplyError(fsd,req_num,"BASE64 decoded_size(%l) but expected(%l)",decoded_size,got_size+4);
						ok = 0;
					}
					else
					{
						uint32_t got_cs = 0;
						uint8_t *cs_ptr = (uint8_t *) &decoded_buf[got_size];
						for (int i=0; i<4; i++)
						{
							got_cs <<= 8;
							got_cs += *cs_ptr++;
						}
						decoded_buf[got_size] = 0;
						uint32_t calc_cs = calcChecksum((uint8_t *) decoded_buf);
						display_level(dbg_cmd+1,4,"got_cs(0x%08x) calc_cs(0x%08x)",got_cs,calc_cs);

						if (got_cs != calc_cs)
						{
							fileReplyError(fsd,req_num,"BASE64 checksum error got_cs(0x%08x) calc_cs(0x%08x)",got_cs,calc_cs);
							ok = 0;
						}
						else
						{
							int32_t bytes_written = the_file.write(decoded_buf,got_size);
							if (bytes_written != got_size)
							{
								fileReplyError(fsd,req_num,"BASE64(%s) file write error at(%lu) wrote(%l) expected(%l)",
									leafName(use_name),
									got_offset,
									bytes_written,
									got_size);
								ok = 0;

							}	// error writing to file
						}	// write to file

						offset += got_size;

					}	// correct decoded size
				}	// decode & write
			}	// got a BASE64 packet

			free(buf);

		}	// got a buff
	}	// while ok && offset < size

	//-----------------------------------------------------
	// finished
	//-----------------------------------------------------
	// rename temp_name to file_name if needed
	// 		and set the timeStamp()
	// or remove the file on any errors

	free(decoded_buf);

	if (ok)
	{
		setTimeStamp(the_file,ts);
		the_file.close();
		if (temp_name[0])
		{
			if (!SD.remove(file_name))
			{
				fileReplyError(fsd,req_num,"FILE could not remove old(%s)",
					leafName(file_name));
				ok = 0;
			}
			else if (!SD.rename(temp_name,file_name))
			{
				fileReplyError(fsd,req_num,"FILE could not rename(%s) to(%s)",
					leafName(temp_name),
					leafName(file_name));
				ok = 0;
			}
		}
	}
	else
	{
		the_file.close();
		SD.remove(use_name);
	}

	// done ...

	if (ok)
		fileReply(fsd,req_num,"OK");
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


	const char *entries;
	const char *param[MAX_PARAMS];
	char *command = getCommandQueue(req_num);
	if (!command)
	{
		my_error("no file_command(%d) queue",req_num);
		return;
	}

	int num_params = parseCommand(command, &param[0], &entries);
	if (!num_params)
	{
		fileReplyError(fsd,req_num,"fileCommand(%d) with no params!",command);
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

		display_level(dbg_parse+1,3,"entry(%s) is_dir(%d) size(%s) ts(%s)",
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
		_list(fsd,req_num,param[0]);
	}
	else if (!strcmp(command,"MKDIR"))
	{
		_mkdir(fsd,req_num,param[0],param[1]);
	}
	else if (!strcmp(command,"RENAME"))
	{
		_rename(fsd,req_num,param[0],param[1],param[2]);
	}
	else if (!strcmp(command,"DELETE"))
	{
		bool ok = 1;
		if (num_params == 2)		// single_file item is in param[1]
		{
			ok = _delete(fsd,req_num,param[0],param[1]);
		}
		else if (!abortPending(fsd, req_num, command))
		{
			// process entry list
			fileReply(fsd,req_num,"PROGRESS\tADD\t%d\t%d",num_dirs,num_files);

			ptr = entries;
			int cont = getNextEntry(fsd,req_num,&the_entry,&ptr);
			while (ok && cont == 1)
			{
				if (abortPending(fsd, req_num, command))
					break;
				ok = _delete(fsd,req_num,param[0],the_entry.entry);
				if (ok)
					cont = getNextEntry(fsd,req_num,&the_entry,&ptr);
			}
			ok = cont == -1 ? 0 : ok;
		}
		if (ok)
			_list(fsd,req_num,param[0]);
	}

	// FILE and PUT

	else if (!strcmp(command,"FILE"))
	{
		_file(fsd,req_num,param[0],param[1],param[2]);
	}
	else
	{
		fileReplyError(fsd,req_num,"Unknown Command %s",command);
	}

	display_level(dbg_hdr,1,"fileCommand(%d,%s) done",req_num,command);
	free(command);
	endCommand(req_num);

}	// fileCommand


// end of fileCommand.cpp
