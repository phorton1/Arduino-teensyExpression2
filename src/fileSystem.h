//-------------------------------------------------------------
// fileSystem.h
//-------------------------------------------------------------
// abstracted file system for use with serial IO protocol

#pragma once

#define USE_OLD_FAT		1

#include "Arduino.h"
#include <SdFat.h>

#if USE_OLD_FAT
	#define myFileType_t File
	#define myDir_t		 dir_t
	extern SdFatSdio SD;
#else
	#define myFileType_t File32
	#define myDir_t		 DirFat_t
	extern SdFat32 SD;
#endif



// following in fileSystem.cpp

extern bool initFileSystem();
extern uint32_t getFreeMB();
extern uint32_t getTotalMB();

extern const char *getTimeStamp(myFileType_t *file);
extern const char *getTimeStamp(const char *path);
extern void setTimeStamp(myFileType_t the_file, const char *ts);
extern bool mkDirTS(const char *path, const char *ts);


// following in fileCommand.cpp


#define MAX_ACTIVE_COMMANDS 10
	// maximum number of simultaneously active commands
#define MAX_QUEUED_BUFFERS  10
	// maximum number of subcommands that can pending for a command


typedef struct
{
	int req_num;
	int head;
	int tail;
	char *queue[MAX_QUEUED_BUFFERS];

} fileCommand_t;


// these routines report errors but do not send file_replies on problems.

extern fileCommand_t *getCommand(int req_num, int sem_level = 0);
	// for file_messages, theSystem will blow it off if the
	// this returns NULL, indicating the command is not active
	// anymore, otherwise, it will add the buffer to the given
	// req_num's queue
extern bool startCommand(int req_num, char *initial_buffer);
	// for file_commands, calling this also invokes the thread,
	// it first initializes the command_queue for the command and
	// adds the buffer, then fires off the thread.
	// reports errors for any problems.
extern bool addCommandQueue(int req_num, char *buf);
	// for file_messages, after checking if the command exists,
	// it will call this method with the bufer. This method
	// reports an error if the command is not active.


// end of fileSystem.h
