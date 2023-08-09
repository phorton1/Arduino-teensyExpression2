//-------------------------------------------------------------
// fileSystem.h
//-------------------------------------------------------------
// abstracted file system for use with serial IO protocol

#pragma once

#include "Arduino.h"
#include <SdFat.h>


class fileSystem
{
public:

   static bool init();

   static uint32_t getFreeMB();
   static uint32_t getTotalMB();

   static void handleFileCommand(const char *command, const char *param);

};


extern SdFatSdio SD;
    // in fileSystem.cpp
