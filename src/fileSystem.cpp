//----------------------------------------------------------
// fileSystem.cpp
//----------------------------------------------------------
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

#define dbg_tfs    2
    // This debugging *may* affect timing.
	// 1 = show file commands
	// 0 = show file operations

#if USE_OLD_FAT
	SdFatSdio SD;
#else
	SdFat32 SD;
#endif



// state of current put

myFileType_t write_file;
uint32_t write_size;
uint32_t write_offset;
uint32_t write_checksum;
char write_ts[30];  // overused for dtCallback

// buffer for use by getDateTimeStamp

char static_timestamp[32];



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
// private methods
//---------------------------------------------

const char *getDateTimeStamp(myFileType_t *entry, const char *filename)
{
    myDir_t dir_entry;
    static_timestamp[0] = 0;
    if (!entry->dirEntry(&dir_entry))
    {
        my_error("Could not get dir_entry for %s",filename);
        return static_timestamp;
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



void setTimeStamp(myFileType_t the_file, char *ts)
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

    display(dbg_tfs,"setTimeStamp(%s,%s,%s,%s,%s,%s)",year,month,day,hour,minute,second);

    the_file.timestamp(
        T_ACCESS | T_CREATE | T_WRITE,
        atoi(year),
        atoi(month),
        atoi(day),
        atoi(hour),
        atoi(minute),
        atoi(second));
}



void dtCallback(uint16_t* date, uint16_t* time)
    // this call back function must be used on diretories
    // instead of setTimeStamp() above. It overuses the
    // global write_ts variable to hold the value for
    // the callback.
{
    char *ts = write_ts;

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

    display(dbg_tfs,"dtCallback(%s,%s,%s,%s,%s,%s)",year,month,day,hour,minute,second);

    *date = FAT_DATE(atoi(year),atoi(month),atoi(day));
    *time = FAT_TIME(atoi(hour),atoi(minute),atoi(second));

}


//==================================================
// handleFileCommand()
//==================================================

void doListRecursive(Stream *fsd, char *full_name, bool recurse, int level, myFileType_t the_dir)
{
	char dir_name[255];
	the_dir.getName(dir_name, sizeof(dir_name));
	display(dbg_tfs,"doListRecursive(%s,%d,%d,%s)",full_name,recurse,level,dir_name);

	const char *ts = getDateTimeStamp(&the_dir,dir_name);

	fsd->print("file_reply:Directory Listing ");
	fsd->print(ts);
	fsd->print(",");
	fsd->println(full_name);

	myFileType_t entry = the_dir.openNextFile();
	while (entry)
	{
		char filename[255];
		entry.getName(filename, sizeof(filename));

		if (entry.isDirectory())
		{
			const char *ts = getDateTimeStamp(&entry,filename);

			fsd->print("file_reply:");
			fsd->print(ts);
			fsd->print(",");
			fsd->print(filename);
			fsd->print("/");
			fsd->print("\r\n");
		}
		else
		{
			uint32_t size = entry.size();
			const char *ts = getDateTimeStamp(&entry,filename);

			fsd->print("file_reply:");
			fsd->print(ts);
			fsd->print(',');
			fsd->print(size);
			fsd->print(',');
			fsd->print(filename);
			fsd->print("\r\n");
		}

		entry = the_dir.openNextFile();

	}   // while (entry)

	// rewind if recurse

	if (recurse)
	{
		the_dir.rewindDirectory();
		myFileType_t entry = the_dir.openNextFile();
		while (entry)
		{
			if (entry.isDirectory())
			{
				entry.getName(dir_name, sizeof(dir_name));

				char full_path[255];
				strcpy(full_path,full_name);
				strcat(full_path,"/");
				strcat(full_path,dir_name);

				doListRecursive(fsd,full_path,recurse,level+1,entry);
			}
			entry = the_dir.openNextFile();
		}

	}
}


#define MAX_DECODED_BUF   10240
    // the decoded base64 will always be less than the raw base64
    // and 10240 is agreed upon between console.pm and teensyExpression.
unsigned char decode_buf[MAX_DECODED_BUF];


void fileSystem::handleFileCommand(const char *command, const char *param)
{
    // DIRECTORY LIST

	Stream *fsd = ACTIVE_FILE_SYS_DEVICE;
	if (!fsd)
	{
		warning(0,"FILE_SYS_DEVICE is off in handleFileCommand()!!!",0);
		return;
	}

    display(dbg_tfs-1,"handleFileCommand %s",command);

    if (!strcmp(command,"list"))
    {
        // send a blank response if the directory does not exist

		char *recurse = (char *) param;
		char *dir_name = recurse;
		while (*dir_name && *dir_name != ',') dir_name++;
		if (*dir_name == ',') *dir_name++ = 0;

		display(dbg_tfs,"list command %s - %s",recurse,dir_name);

		myFileType_t the_dir = SD.open(dir_name);
		if (the_dir)
		{
			doListRecursive(fsd,dir_name,*recurse=='1',0,the_dir);
		}

    }   // list command


    // MAKE DIRECTORY

    else if (!strcmp(command,"mkdir"))
    {
        char *path = (char *) param;
        char *ts = path;
        while (*ts && *ts != ',') ts++;
        if (*ts == ',') *ts++ = 0;

        display(dbg_tfs,"fileSystem::mkdir(%s,%s)",path,ts);
        if (SD.exists(path))
        {
            fsd->print("file_reply:");
            fsd->print("ERROR - directory_already_exists ");
            fsd->print(path);
            fsd->print("\r\n");
        }
        else
        {
            display(dbg_tfs,"fileSystem::mkdir(%s) calling SD.mkdir()",param);

            // this snippet is how you have to set the timestamp on a directory

            strcpy(write_ts,ts);

			#if USE_OLD_FAT
				FatFile::dateTimeCallback(dtCallback);
				int rslt = SD.mkdir(path);
				FatFile::dateTimeCallbackCancel();
			#else
				FsDateTime::setCallback(dtCallback);
				int rslt = SD.mkdir(path);
				FsDateTime::clearCallback();
			#endif

            if (rslt)
            {
                #if 0   // as this does not work
                    myFileType_t the_dir = SD.open(path);
                    if (the_dir)
                    {
                        setTimeStamp(the_dir,ts);
                        the_dir.close();
                    }
                #endif

                fsd->print("file_reply:");
                fsd->print("OK - created_directory ");
            }
            else
            {
                fsd->print("file_reply:");
                fsd->print("ERROR - could_not_create_directory ");
            }
            fsd->print(param);
            fsd->print("\r\n");
        }
    }   // mkdir command


    // GET FILE

    else if (!strcmp(command,"get"))
    {
        const char *filename = param;
        display(dbg_tfs,"fileSystem::get(%s)",filename);
        myFileType_t the_file = SD.open(filename);

        // The result will be
        // file_reply:FILE $ts,$size,$entry and a bunch of
        // file_reply:BASE64_ENCODED_FILE lines
        // file_reply:CHECKSUM %d
        // file_reply_end

        if (the_file)
        {
            char filename[255];
            the_file.getName(filename, sizeof(filename));
            const char *ts = getDateTimeStamp(&the_file,filename);
            uint32_t size = the_file.size();

            fsd->print("file_reply:FILE ");
            fsd->print(ts);
            fsd->print(",");
            fsd->print(size);
            fsd->print(",");
            fsd->print(filename);
            fsd->print("\r\n");

            #define BUF_BYTES   80
            #define BASE64_BYTES   120      // at least 4/3ds the size of buf bytes

            unsigned char buf[BUF_BYTES];
            char b64_buf[BASE64_BYTES];

            uint32_t file_off = 0;
            uint32_t check_sum = 0;
            bool ok = 1;

            while (size)
            {
                uint32_t bytes_to_read = size;
                if (bytes_to_read > BUF_BYTES) bytes_to_read = BUF_BYTES;
                uint32_t got_bytes = the_file.read(buf,bytes_to_read);

                // got_bytes = 2;
                // to test error handling

                if (got_bytes != bytes_to_read)
                {
                    my_error("got %d bytes while attempting to read %d bytes at offset %d",got_bytes,bytes_to_read,file_off);
                    fsd->print("file_reply:ERROR got ");
                    fsd->print(got_bytes);
                    fsd->print(" bytes while attempting to read ");
                    fsd->print(bytes_to_read);
                    fsd->print(" bytes at offset ");
                    fsd->print(file_off);
                    fsd->print("\r\n");
                    ok = 0;
                    break;
                }

                for (uint32_t i=0; i<got_bytes; i++)
                {
                    check_sum += buf[i];
                }

                int enc_len = base64_encode(b64_buf,(char *)buf,bytes_to_read);
                b64_buf[enc_len] = 0;

				delay(20);
                fsd->print("file_reply:");
                fsd->print(b64_buf);
                fsd->print("\r\n");

                size -= bytes_to_read;
                file_off += bytes_to_read;
            }

            the_file.close();

            if (ok)
            {
                fsd->print("file_reply:CHECKSUM ");
                fsd->print(check_sum);
                fsd->print("\r\n");
            }
        }
        else
        {
            display(0,"error could not open file: %s - returning empty result",filename);
        }
    }   // get command


    //-----------------------------------------------
    // put is handled in 3 parts
    //-----------------------------------------------
    // file_command:put
    // file_command:base_64 and
    // file_command:checksum

    else if (!strcmp(command,"put"))
    {
        // parse out the ts, size, dir, and full filename

        char *ts = (char *) param;
        char *sz = ts;
        while (*sz && *sz != ',') sz++;
        if (*sz == ',')
            *sz++ = 0;
        char *dir = sz;
        while (*dir && *dir != ',') dir++;
        if (*dir == ',')
            *dir++ = 0;
        char *filename = dir;
        while (*filename && *filename != ',') filename++;
        if (*filename == ',')
            *filename++ = 0;

        display(dbg_tfs,"put ts=%s sz=%s dir=%s filename=%s",ts,sz,dir,filename);

        write_size = atoi(sz);
        write_offset = 0;
        write_checksum = 0;
        strcpy(write_ts,ts);

        display(dbg_tfs,"write_size=%d",write_size);

        // make sure the directory exists

        bool ok = 1;

        if (*dir)
        {
            if (SD.exists(dir))
            {
                myFileType_t the_dir = SD.open(dir);
                if (!the_dir.isDirectory())
                {
                    my_error("NOT A DIRECTORY (is a file): %s",dir);
                    fsd->print("file_reply:ERROR NOT A DIRECTORY .. is a file - ");
                    fsd->print(dir);
                    fsd->print("\r\n");
                    ok = 0;
                }
                else
                {
                    display(dbg_tfs,"directory %s exists",dir);
                }
                the_dir.close();
            }
            else
            {
                // prepare for setting timestamp in below call
                // cannot call setTimeStamp() on dirs

                strcpy(write_ts,ts);

				#if USE_OLD_FAT
					FatFile::dateTimeCallback(dtCallback);
					int rslt = SD.mkdir(dir);
					FatFile::dateTimeCallbackCancel();
				#else
					FsDateTime::setCallback(dtCallback);
					int rslt = SD.mkdir(dir);
					FsDateTime::clearCallback();
				#endif

				if (!rslt)
                {
                    my_error("COULD NOT CREATE DIRECTORY: %s",dir);
                    fsd->print("file_reply:ERROR - COULD NOT CREATE DIRECTORY ");
                    fsd->print(dir);
                    fsd->print("\r\n");
                    ok = 0;
                }
            }
        }

        // OPEN the new file

        if (ok)
        {
            SD.remove(filename);

            write_file = SD.open(filename, FILE_WRITE);
            if (!write_file)
            {
                my_error("COULD NOT CREATE FILE %s",filename);
                fsd->print("file_reply:ERROR - COULD NOT OPEN FILE ");
                fsd->print(filename);
                fsd->print("\r\n");
                ok = 0;
            }
        }

        // return the first OK

        if (ok)
        {
            fsd->print("file_reply:OK PUT STARTED");
            #if 0
                fsd->print(ts);
                fsd->print(",");
                fsd->print(size);
                fsd->print(",");
                fsd->print(filename);
            #endif

            fsd->print("\r\n");
        }
    }   // part 1 of 3 of put command

    else if (!strcmp(command,"BASE64"))
    {
        display(dbg_tfs,"base64 offset=%d",write_offset);

        int decoded_size = base64_decode((char *)decode_buf,(char *)param,strlen(param));
        int bytes_written = write_file.write(decode_buf,decoded_size);

        if (bytes_written != decoded_size)
        {
            my_error("write fail got(%d) expected(%d) offset(%d)",bytes_written,decoded_size,write_offset);
            fsd->print("file_reply:ERROR - write fail got=");
            fsd->print(bytes_written);
            fsd->print(" expected=");
            fsd->print(decoded_size);
            fsd->print(" at offset=");
            fsd->print(write_offset);
            fsd->print("\r\n");
        }
        else
        {
            for (int i=0; i<decoded_size; i++)
                write_checksum += decode_buf[i];
            write_offset += decoded_size;

            fsd->print("file_reply:OK PUT CONTINUE offset=");
            fsd->print(write_offset);
            fsd->print("\r\n");
        }
    }   // BASE64 - part 2 of 3 of put command

    else if (!strcmp(command,"CHECKSUM"))
    {
        uint32_t got = atoi(param);
        display(dbg_tfs,"CHECKSUM got=%d calculated=%d",got,write_checksum);
        if (got == write_checksum)
        {
            fsd->print("file_reply:OK CHECKSUM ");
            fsd->print(got);
            fsd->print("==");
            fsd->print(write_checksum);
            fsd->print("\r\n");
        }
        else
        {
            fsd->print("file_reply:ERROR CHECKSUM FAIL got(");
            fsd->print(got);
            fsd->print(") != calculated(");
            fsd->print(write_checksum);
            fsd->print(")\r\n");
        }

        setTimeStamp(write_file,write_ts);
        write_file.close();

    }   // CHECKSUM - part 3 of 3 of put command


    // DELETE FILE OR DIRETORY

    else if (!strcmp(command,"delete"))
    {
        display(dbg_tfs,"delete %s",param);
        myFileType_t the_file = SD.open(param);
        if (the_file)
        {
            bool rslt;
            bool is_dir = the_file.isDirectory();

            if (is_dir)
            {
                rslt = the_file.rmRfStar();
            }
            else
            {
                the_file.close();
                rslt = SD.remove(param);
            }

            if (rslt)
            {
                fsd->print("file_reply:OK DELETE ");
                fsd->print(is_dir);
                fsd->print("\r\n");
            }
            else
            {
                fsd->print("file_reply:ERROR - ");
                fsd->print(is_dir?"rmRfStar(":"remove(");
                fsd->print(param);
                fsd->print(") failed!");
                fsd->print("\r\n");
            }
        }
        else
        {
            fsd->print("file_reply:ERROR - could not open '");
            fsd->print(param);
            fsd->print("'for deletion");
            fsd->print("\r\n");
        }

    }   // delete command

    // RENAME file or directory

	else if (!strcmp(command,"rename"))
    {
        char *old_path = (char *) param;
        char *new_path = old_path;
        while (*new_path && *new_path != ',') new_path++;
        if (*new_path == ',') *new_path++ = 0;
        display(dbg_tfs,"rename(%s) to '%s'",old_path,new_path);

        myFileType_t the_file = SD.open(old_path);
        if (the_file)
        {
            uint32_t size = the_file.size();
            bool is_dir = the_file.isDirectory();
            const char *ts = getDateTimeStamp(&the_file,old_path);
            the_file.close();

            display(dbg_tfs,"rename is_dir=%d size=%d ts=%s",is_dir,size,ts);

            if (SD.rename(old_path,new_path))
            {
                fsd->print("file_reply:OK RENAME ");
                fsd->print(is_dir);
                fsd->print(",");
                fsd->print(size);
                fsd->print(",");
                fsd->print(ts);
                fsd->print("\r\n");
            }
            else
            {
                fsd->print("file_reply:ERROR - rename '");
                fsd->print(old_path);
                fsd->print("' to '");
                fsd->print(new_path);
                fsd->print("' FAILED");
                fsd->print("\r\n");
            }
        }
        else
        {
            fsd->print("file_reply:ERROR - could not open '");
            fsd->print(old_path);
            fsd->print("' for renaming");
            fsd->print("\r\n");
        }
    }

    fsd->print("file_reply_end");
    fsd->print("\r\n");

}	// handleFileCommand


// end of fileSystem.cpp
