//-------------------------------------------------------------
// fileSystem.h
//-------------------------------------------------------------
// abstracted file system for use with serial IO protocol

#pragma once

#define USE_OLD_FAT		1


#include "Arduino.h"
#include <SdFat.h>


class fileSystem
{
public:

   static bool init();

   static uint32_t getFreeMB();
   static uint32_t getTotalMB();

   static void handleFileCommand(void *ptr);

};


// update to SdFat v2

#if USE_OLD_FAT
	#define myFileType_t File
	#define myDir_t		 dir_t
	extern SdFatSdio SD;
#else
	#define myFileType_t File32
	#define myDir_t		 DirFat_t
	extern SdFat32 SD;
#endif
