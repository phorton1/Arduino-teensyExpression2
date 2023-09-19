//----------------------------------------------------------
// fileSystem.cpp
//----------------------------------------------------------
// GET USES 20K of stack!!
// Abstracted file system for use with serial IO protocol
//
// OLD NOTE:
//
// prh - I was getting an error when starting the system from a cold power up.
// it was working ok from a compile (ctrl-I in Komodo) right after the TeensyLoader
// put the program on the teensy, but not on a subsequent cold-powerup.
// I "fixed" it by increasing BUSY_TIMEOUT_MICROS from 500000 to 1000000
// in /Arduino/libraries/SdFat/src/SdCard/SdioTeensy.cpp.
//
// VERSION2 NOTES
//
// In TE1 was using a slighly modified version of the sdFat library v1.1.2 that I
// downloaded from https://github.com/greiman/SdFat 3 years ago in my libraries
// folder.  It was working well with my old version 1.53 of teensyDuino ...
// The only mod I had made was mentioned above.
//
// I was worried about my teensy3.6's, they're being discontinued, and I have two brand new 4.1's.
// But the 4.1 is not supported by that old version of SdFat.  To get SdFat for 4.1's, it was
// "recommended" that one upgrades the teensyDuino to a newer version which includes
// SdFat (and SD) libraries ...
//
// So I went through the hassle of upgrading teensyDuino to v1.58.
// A nice change with the new teensyDuino was that I also was able to do away
// with my usb_desc.prh scheme and use a somewhat approved method of overriding
// the USB definitions.
//
// With teensyDuino 1.58 I got SdFat v2.1.2 in the Program Files (x86)
// folder as part of the deal. So, I saved off and removed my old SdFat library,
// and endeavored to build with the new SdFat library. I immediatly found that
// SdFat v2.1.2 is a complete rewrite from the old version and I had to figure
// out what objects to use just to get it basically working:
//
//		SdFat32 instead of the old SdFatSdio for the SD type
//		File32 instead of the old File for files
//		DirFat_t instead the old dir_t for directory entries
//      ... I still haven't figured out directory entries and datetime stamps
//
// I got it basically working with the teensy3.6 (then made new PCB's and
// fixed the bad connector problem), and then noticed that new SdFat library
// has the opposite problem of the one I 'fixed' in the old library. It
// fails to initialze the SD card on a soft reboot (after a compile/upload),
// and now requiires me to unplug and replug in the teensy each time I want
// to test the program.
//
// So I made the compile conditional based on a USE_OLD_FAT define.
// I moved the SdFat and SD libraries from Program Files (x86) and put
// them in my libraries folder and saved them off elsewhere.
// I can now place either the 'new' or 'old' SdFat library in my
// libraries folder, adjust the #define, and build against either one.
// And I verified that the old one still works on either kind of reboot.
// Howevber,
//
//--------------------------------------------------------------------------------
// I CANNOT HAVE BOTH SDFAT LIBRARIES IN MY LIBRARIES FOLDER AT THE SAME TIME.
//--------------------------------------------------------------------------------
// SO I NEED A GOOD PLACE TO STORE THEM, esp since one is under source control
// and I am tempted to modify them, and /junk is not a good place for them.
// So they are in:     /zip/_teensy/_SdFat_libraries


#include "fileSystem.h"
#include "prefs.h"
#include <myDebug.h>
#include <Base64.h>
#include <TeensyThreads.h>


#define TEST_DELAY    0		// 2000
	// delay certain operations to test progress dialog etc
	// set to 1000 or 2000 ms to slow things down

// The following debugging *may* affect timing.

#define dbg_ts    1
	// 0 = show timestamp operations
	// -1 = show callback setting
#define dbg_hdr	   -1
	// show a header for any file system command
	// -1 = show entries
#define dbg_cmd	   -1
	// 0 = show file commands
	// -1 = show details

#if USE_OLD_FAT
	SdFatSdio SD;
#else
	SdFat32 SD;
#endif


#define MAX_DECODED_BUF   10240
    // the decoded base64 will always be less than the raw base64
    // and 10240 is agreed upon between console.pm and teensyExpression.

typedef struct
{
	char size[10];
	char ts[22];
	char entry[255];
	bool is_dir;
}   textEntry_t;



// state of current put

// myFileType_t write_file;
// uint32_t write_size;
// uint32_t write_offset;
// uint32_t write_checksum;



char write_timestamp[32];
char static_timestamp[32];
	// buffers for use by getTimeStamp and dtCallback
unsigned char decode_buf[MAX_DECODED_BUF];
	// used to decode base64 and in getNextEntry



//---------------------------------------------
// debugging
//---------------------------------------------

#define LIST_DIRECTORY_AT_STARTUP  0	// USE_OLD_FAT

#if LIST_DIRECTORY_AT_STARTUP

	#define dbg_print_dir  1

    void printDirectory(myFileType_t dir, int numTabs = 0)
    {
        while(true)
        {
            myFileType_t entry = dir.openNextFile();
            if (!entry)
                break;

            char filename[255];
            entry.getName(filename, sizeof(filename));

            dir_t dir_entry;
            if (!entry.dirEntry(&dir_entry))
            {
                my_error("Could not get dir_entry for %s",filename);
                return;
            }
            display_level(dbg_print_dir,numTabs + 1,"    cdate(%d) ctime(%d) c10ths(%d)",
                dir_entry.creationDate,dir_entry.creationTime,dir_entry.creationTimeTenths);
            display_level(dbg_print_dir,numTabs + 1,"    adate(%d) wdate(%d) wtime(%d)",
                dir_entry.lastAccessDate,dir_entry.lastWriteDate,dir_entry.lastWriteTime);
            display_level(dbg_print_dir,,numTabs + 1"    attr(0x%02x), size(%d)",
                dir_entry.attributes,dir_entry.fileSize);

            uint16_t year = FAT_YEAR(dir_entry.creationDate);
            uint16_t month = FAT_MONTH(dir_entry.creationDate);
            uint16_t day = FAT_DAY(dir_entry.creationDate);
            uint16_t hour = FAT_HOUR(dir_entry.creationTime);
            uint16_t minute = FAT_MINUTE(dir_entry.creationTime);
            uint16_t second = FAT_SECOND(dir_entry.creationTime);
            uint16_t hundredths = dir_entry.creationTimeTenths;
                // according to comment this actually hundredths ...

            if (hundredths > 100)
            {
                hundredths -= 100;
                second += 1;
            }

			char time_buf[36];
			sprintf(time_buf,"%04d-%02d-%02d %02d:%02d:%02d.%02d",
                year,
                month,
                day,
                hour,
                minute,
                second,
                hundredths);

			char tab_buf[24];
			memset(tab_buf,32,24);
			if (numTabs > 4)
				tab_buf[16] = 0;
			tab_buf[ (4-numTabs) * 4 ] = 0;

            if (entry.isDirectory())
            {
                display(0,":%-32s%s         %s",filename,tab_buf,time_buf);
                printDirectory(entry, numTabs+1);
            }
            else
            {
                display_level(0,numTabs + 1,"%-32s%s%-08d  %s",
                    filename,
					tab_buf,
                    entry.size(),
					time_buf);
            }

            entry.close();
        }
     }
#endif  // LIST_DIRECTORY_AT_STARTUP



//---------------------------------------------
// INIT
//---------------------------------------------

// static
bool fileSystem::init()
{
	uint8_t dd = prefs.DEBUG_DEVICE;
	uint8_t fsd = prefs.FILE_SYS_DEVICE;

    warning(0,"FILE_SYS_DEVICE %s",
		fsd == OUTPUT_DEVICE_SERIAL ? "is SERIAL" :
		fsd == OUTPUT_DEVICE_USB 	? "is USB" :
		fsd == OUTPUT_DEVICE_OFF 	? "is OFF!!" :
		dd == DEBUG_DEVICE_SERIAL 	? "follows DEBUG_DEVICE which is SERIAL" :
		dd == DEBUG_DEVICE_USB 		? "follows DEBUG_DEVICE which is USB" :
		"follows DEBUG_DEVICE which is OFF" );

#if USE_OLD_FAT
    if (!SD.begin())
#else
	if (!SD.begin(BUILTIN_SDCARD))
#endif
	{
        my_error("Could not initialize SD",0);
        return false;
    }

    #if 0   // SHOW CID

        // detailed SD card debugging
        // prh - I could not effing figure out how to get the volumeLabel from the (?) MBR

        cid_t cid;
        if (!SD.card()->readCID(&cid))
        {
            my_error("Could not readCID()",0);
            return false;
        }

        // unsigned char mid;                  // manufacturer id
        // char oid[2];                        // OEM/Application ID
        // char pnm[5];                        // product name
        // unsigned char prv_m : 4;            // Product revision least significant digit
        // unsigned char prv_n : 4;            // Product revision most significant digit
        // uint32_t psn;                       // Product serial number
        // unsigned char mdt_year_high : 4;    //  Manufacturing date year high digit
        // unsigned char mdt_month : 4;        // Manufacturing date month
        // unsigned char mdt_year_low : 4;     // Manufacturing date year low digit

        display(0,"fileSystem: Mfr(0x%02x) OEM_ID(0x%02x,0x%02x) Product(%c%c%c%c%c) Version(%d.%d) SN(0x%08x)",
            cid.mid,
            cid.oid[0],cid.oid[1],
            cid.pnm[0],cid.pnm[1],cid.pnm[2],cid.pnm[3],cid.pnm[4],
            cid.prv_n,cid.prv_m,
            cid.psn);
        display(0,"fileSystem: Manufacture Date: %d-%d",
            (2000 + cid.mdt_year_low + 10 * cid.mdt_year_high),
            cid.mdt_month);
    #endif  // SHOW_CID

    #if 0   // SHOW_SIZE_DETAILS
        uint8_t fat_type = SD.fatType();
        uint32_t cluster_count = SD.clusterCount();
        int32_t free_cluster_count = SD.freeClusterCount();
        uint8_t blocks_per_cluster = SD.blocksPerCluster();
        uint32_t block_count = SD.volumeBlockCount();
        display(0,"fileSystem: fat_type(%d),clusters(%d) free(%d) blocks_per(%d) block_count(%d)",
            fat_type,
            cluster_count,
            free_cluster_count,
            blocks_per_cluster,
            block_count);
    #endif  // SHOW_SIZE_DETAILS

    #if  1  // SHOW_HUMAN_READABLE_SPACE_USED ('real' GB same as Windows)
        uint32_t gb_free = 10 * getFreeMB() / 1024;
        uint32_t gb_total = 10 * getTotalMB() / 1024;
        uint32_t gb_used = gb_total - gb_free;
        display(0,"FileSystem: %d.%d/%d.%dGB Used",
            gb_used/10,gb_used%10, gb_total/10, gb_total%10);
    #endif

    #if LIST_DIRECTORY_AT_STARTUP
        myFileType_t root = SD.open("/");
        printDirectory(root);
        root.close();
    #endif


    return true;
}



//------------------------------------------------------------
// API
//------------------------------------------------------------

// static
uint32_t fileSystem::getFreeMB()
{
	#if USE_OLD_FAT
		uint32_t cluster_count = SD.freeClusterCount();
		uint8_t blocks_per_cluster = SD.blocksPerCluster();
		return (cluster_count * blocks_per_cluster) / (1024*2);
	#else
		uint32_t cluster_count32 = SD.freeClusterCount();
		uint32_t bytes_per_cluster32 = SD.bytesPerCluster();
		delay(100);
		display(0,"free  count=%d  per=%d  shift=%d",cluster_count32,bytes_per_cluster32,SD.bytesPerClusterShift());
		uint64_t cluster_count = cluster_count32;
		uint64_t bytes_per_cluster = bytes_per_cluster32;
		uint64_t total_bytes = cluster_count * bytes_per_cluster;
		total_bytes /= 1024 * 1024;
		return total_bytes;
	#endif
}


// static
uint32_t fileSystem::getTotalMB()
	// NOWHERE is there an example of how to use these to determine the size of the file system.
	// bytesPerClusterShift() is really unclear.
	// On my 32GB card, cluserCount() returns 953948 and bytesPerCluster returns 32768.
	// Interestingly, multiplying these gives 31,258,968,064, which agrees with the
	// total bytes for the volume given by the Properties in Windows, without using
	// bytesPerClusterShift(), wtf ever that is.  Then dividing by 1024^2 gives
	// 'real' MB's.
{
	#if USE_OLD_FAT
		uint32_t cluster_count = SD.clusterCount();
		uint8_t blocks_per_cluster = SD.blocksPerCluster();
		return (cluster_count * blocks_per_cluster) / (1024*2);
	#else
		uint32_t cluster_count32 = SD.clusterCount();
		uint32_t bytes_per_cluster32 = SD.bytesPerCluster();
		delay(100);
		display(0,"total count=%d  per=%d  shift=%d",cluster_count32,bytes_per_cluster32,SD.bytesPerClusterShift());
		uint64_t cluster_count = cluster_count32;
		uint64_t bytes_per_cluster = bytes_per_cluster32;
		uint64_t total_bytes = cluster_count * bytes_per_cluster;
		total_bytes /= 1024 * 1024;
		return total_bytes;
	#endif
}


//---------------------------------------------
// private utilities
//---------------------------------------------

static const char *getTimeStamp(myFileType_t *file)
{
    myDir_t dir_entry;
    static_timestamp[0] = 0;

    if (!file->dirEntry(&dir_entry))
    {
		char filename[255];
		file->getName(filename, sizeof(filename));
        my_error("Could not get dir_entry for %s",filename);
        return "";
    }

	#if USE_OLD_FAT 	// PRH PRH PRH - this needs to be reworked
		uint16_t year = FAT_YEAR(dir_entry.creationDate);
		uint16_t month = FAT_MONTH(dir_entry.creationDate);
		uint16_t day = FAT_DAY(dir_entry.creationDate);
		uint16_t hour = FAT_HOUR(dir_entry.creationTime);
		uint16_t minute = FAT_MINUTE(dir_entry.creationTime);
		uint16_t second = FAT_SECOND(dir_entry.creationTime);
		uint16_t hundredths = dir_entry.creationTimeTenths;
			// according to comment this actually hundredths ...
		if (hundredths >= 100)
		{
			hundredths -= 100;
			second += 1;
		}

		sprintf(static_timestamp,"%d-%02d-%02d %02d:%02d:%02d",
			year,month,day,hour,minute,second);
	#endif

    return static_timestamp;
}


static const char *getTimeStamp(const char *path)
{
	myFileType_t file = SD.open(path);
	if (!file)
	{
		my_error("Could not open %s to getTimeStamp",path);
		return "";
	}
	const char *rslt = getTimeStamp(&file);
	file.close();
	return rslt;
}


#if 0

static void setTimeStamp(myFileType_t the_file, char *ts)
{
    char *year = &ts[0];
    ts[4] = 0;

    char *month = &ts[5];
    ts[7] = 0;

    char *day = &ts[8];
    ts[10] = 0;

    char *hour = &ts[11];
    ts[13] = 0;

    char *minute = &ts[14];
    ts[16] = 0;

    char *second = &ts[17];
    ts[19] = 0;

    display_level(dbg_ts,4,"setTimeStamp(%s,%s,%s,%s,%s,%s)",year,month,day,hour,minute,second);

    the_file.timestamp(
        T_ACCESS | T_CREATE | T_WRITE,
        atoi(year),
        atoi(month),
        atoi(day),
        atoi(hour),
        atoi(minute),
        atoi(second));
}



static void dtCallback(uint16_t* date, uint16_t* time)
    // this call back function must be used on diretories
    // instead of setTimeStamp() above. It overuses the
    // global write_timestamp variable to hold the value for
    // the callback.
{
    char *ts = write_timestamp;

    char *year = &ts[0];
    ts[4] = 0;

    char *month = &ts[5];
    ts[7] = 0;

    char *day = &ts[8];
    ts[10] = 0;

    char *hour = &ts[11];
    ts[13] = 0;

    char *minute = &ts[14];
    ts[16] = 0;

    char *second = &ts[17];
    ts[19] = 0;

    display_level(dbg_ts+1,4,"dtCallback(%s,%s,%s,%s,%s,%s)",year,month,day,hour,minute,second);

    *date = FAT_DATE(atoi(year),atoi(month),atoi(day));
    *time = FAT_TIME(atoi(hour),atoi(minute),atoi(second));

}

#endif


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


//---------------------------------------------
// atomic commands
//---------------------------------------------

static void doList(Stream *fsd, int req_num, const char *dir)
{
	display_level(dbg_cmd,2,"fileSystem::doList(%s)",dir);

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

	display_level(dbg_cmd,2,"fileSystem::makeDir(%s,%s) path=%s",dir,name,path);

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


	const char *ts = getTimeStamp(path);
	fileReply(fsd,req_num,1,0,ts,name);
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

	display_level(dbg_cmd,2,"fileSystem::renameObj(%s,%s,%s) path1=%s path2=%s",dir,name1,name2,path1,path2);

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
    display_level(dbg_cmd,2,"fileSystem::deleteOne(%s)",path);
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
// parse entries lists
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

	if (!**ptr || **ptr=='\r')
		return 0;

	// we set all 3 fields or fail
	// there is a tab after each entry

	int num_params = 0;
	char *out = the_entry->size;

	while (**ptr)
	{
		if (**ptr == '\t')
		{
			*out = 0;
			(*ptr)++;
			num_params++;
			if (num_params == 1)
				out  = the_entry->ts;
			else if (num_params == 2)
				out = the_entry->entry;
			else if (num_params == 3 && *(out-2) == '/')
				// get rid of terminating '/' on dir entries
			{
				the_entry->is_dir = 1;
				*(out-2) = 0;
			}
		}
		else if (**ptr == '\r')
		{
			(*ptr)++;
			break;
		}
		else
		{
			*out++ = *(*ptr)++;
		}
	}

	bool ok = 1;
	if (num_params != 3)
	{
		ok = 0;
		fileReplyError(fsd,req_num,"Incorrect number of fields(%d) in fileEntry",num_params);
	}

	// if (last)
	// 	*last = !**ptr || **ptr=='\r';

	return ok;
}



//-------------------------------------------------------
// handleFileCommand
//-------------------------------------------------------


void fileSystem::handleFileCommand(void *buf)
	// the buf we are passed must be freed when done!!
{
	Stream *fsd = ACTIVE_FILE_SYS_DEVICE;

	int len = strlen((char *)buf);
    display_level(dbg_hdr,1,"handleFileCommand() command=%d bytes",len);

	// buf is pointing to the req_num

	int req_num = 0;
	char *entries = (char *) buf;
	const char *req_num_ptr = entries;
	while (*entries && *entries != ')') entries++;

	if (*entries != ')')
	{
		my_error("Could not find closing paren in file_command",0);
		free(buf);
		return;
	}

	*entries++ = 0;		// null terminate the request_number
	req_num = atoi(req_num_ptr);
	display(0,"got request_number=%d",req_num);
	if (*entries++ != ':')
	{
		my_error("expected colon after (request_number)",0);
		free(buf);
		return;
	}


	//------------------------------
	// parse the command line
	//------------------------------

	char *command = entries;

	int num_params = 0;
	const char *param[3];
	while (*entries && *entries != '\r')
	{
		if (*entries == '\t' && num_params<3)
		{
			*entries++ = 0;
			param[num_params++] = entries;
		}
		entries++;
	}
	if (*entries == '\r')
		*entries++ = 0;

	for (int i=num_params; i<3; i++)
	{
		param[i] = "";
	}

    display_level(dbg_hdr,1,"handleFileCommand(%d,%s) param0(%s) param1(%s) param2(%s)",
		req_num,
		command,
		param[0],
		param[1],
		param[2]);


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
		free(buf);
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
		else	// process entry list
		{
			sendProgressADD(fsd,req_num,num_dirs,num_files);
			ptr = entries;
			int rslt = getNextEntry(fsd,req_num,&the_entry,&ptr);
			bool last = !*ptr || *ptr == '\r';
			while (rslt == 1)
			{
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
	display_level(dbg_hdr,1,"handleFileCommand(%d,%s) returning",req_num,command);
	free(buf);

}	// handleFileCommand


// end of fileSystem.cpp
