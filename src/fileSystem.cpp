// abstracted file system for use with serial IO protocol
//
// We are using the sdFat library downloaded from https://github.com/greiman/SdFat
// which is in our Arduino/libraries directory, NOT either of the ones from the
// teensyDuino or original Arduino installations.
//
// A block is 512 bytes and "kBytes" is block_count/2


#include "fileSystem.h"
#include "prefs.h"
#include <myDebug.h>
#include <Base64.h>

#define dbg_tfs    2
    // This debugging *may* affect timing.
    // It "works" when this is on (0), but "not" when 1 (debugging turned off)



// update to SdFat v2
SdFat32 SD;
// extern SdFatSdio SD;
    // SdFatSdio uses a traditional DMA SDIO implementation.
    // SdFatSdioEX SD; // SdFatSdioEX uses extended multi-block transfers without DMA.
    // Note the difference is speed and busy yield time.
    // Teensy 3.5 & 3.6 SDIO: 4 to 5 times the throughput using the 4-bit SDIO mode compared to the 1-bit SPI mode
    //
    // prh - I was getting an error when starting the system from a cold power up.
    // it was working ok from a compile (ctrl-I in Komodo) right after the TeensyLoader
    // put the program on the teensy, but not on a subsequent cold-powerup.
    // I "fixed" it by increasing BUSY_TIMEOUT_MICROS from 500000 to 1000000
    // in /Arduino/libraries/SdFat/src/SdCard/SdioTeensy.cpp.

Stream *s_Serial = 0;
    // The serial port to send commands to.
    // Determined by preference.


// state of current put

File32 write_file;
uint32_t write_size;
uint32_t write_offset;
uint32_t write_checksum;
char write_ts[30];  // overused for dtCallback

// buffer for use by getDateTimeStamp

char static_timestamp[32];



//---------------------------------------------
// debugging
//---------------------------------------------

#define LIST_DIRECTORY_AT_STARTUP  0
#if LIST_DIRECTORY_AT_STARTUP

    void printDirectory(File32 dir, int numTabs)
    {
        while(true)
        {
            File32 entry = dir.openNextFile();
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
            display(0,"dir_entry   cdate(%d) ctime(%d) c10ths(%d)",
                dir_entry.creationDate,dir_entry.creationTime,dir_entry.creationTimeTenths);
            display(0,"            adate(%d) wdate(%d) wtime(%d)",
                dir_entry.lastAccessDate,dir_entry.lastWriteDate,dir_entry.lastWriteTime);
            display(0,"            attr(0x%02x), size(%d)",
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

            display(0,"date(%02d,%02d,%02d) time(%02d:%02d:%02d.%02d)",
                year,
                month,
                day,
                hour,
                minute,
                second,
                hundredths);

            for (uint8_t i=0; i<numTabs; i++)
            {
                Serial.print('\t');
            }

            if (entry.isDirectory())
            {
                display(0,"%s/",filename);
                printDirectory(entry, numTabs+1);
            }
            else
            {
                display(0,"\t\t%s    %d",
                    filename,
                    entry.size());
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
	uint8_t fs_device = prefs.FILE_SYS_DEVICE;

    if (fs_device == OUTPUT_DEVICE_OFF)
    {
        warning(0,"FILE_SYS_DEVICE is OFF",0);
        s_Serial = 0;
    }
    else if (fs_device == OUTPUT_DEVICE_USB)
    {
        warning(0,"FILE_SYS_DEVICE is USB",0);
        s_Serial = &Serial;
    }
    else if (fs_device == OUTPUT_DEVICE_SERIAL)
    {
        display(0,"FILE_SYS_DEVICE is Serial",0);
        s_Serial = &Serial3;
    }

    if (!SD.begin())
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

    #if  1  // SHOW_HUMAN_READABLE_SPACE_USED
        uint32_t gb_free = 10 * getFreeMB() / 1024;
        uint32_t gb_total = 10 * getTotalMB() / 1024;
        uint32_t gb_used = gb_total - gb_free;
        display(0,"FileSystem: %d.%d/%d.%dGB Used",
            gb_used/10,gb_used%10, gb_total/10, gb_total%10);
    #endif

    #if LIST_DIRECTORY_AT_STARTUP
        File32 root = SD.open("/");
        printDirectory(root, 0);
        root.close();
    #endif


    return true;
}



//------------------------------------------------------------
// API
//------------------------------------------------------------

uint32_t fileSystem::getFreeMB()
{
    uint32_t free_cluster_count = SD.freeClusterCount();
    uint16_t bytes_per_cluster = SD.bytesPerCluster();
	// prh - may have to use uint8_t bytesPerClusterShift () const
    if (free_cluster_count > 0)
        return (free_cluster_count * bytes_per_cluster) / (1024*2);
    return 0;
}


uint32_t fileSystem::getTotalMB()
{
    uint32_t cluster_count = SD.clusterCount();
    uint16_t bytes_per_cluster = SD.bytesPerCluster();
	// prh - may have to use uint8_t bytesPerClusterShift () const
    return (cluster_count * bytes_per_cluster) / (1024*2);
}


//---------------------------------------------
// private methods
//---------------------------------------------

const char *getDateTimeStamp(File32 *entry, const char *filename)
{
    DirFat_t dir_entry;
    static_timestamp[0] = 0;
    if (!entry->dirEntry(&dir_entry))
    {
        my_error("Could not get dir_entry for %s",filename);
        return static_timestamp;
    }

	#if 0 	// PRH PRH PRH - this needs to be reworked
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



void setTimeStamp(File32 the_file, char *ts)
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

#define RECURSIVE_LIST   1

#if RECURSIVE_LIST



    void doListRecursive(char *full_name, bool recurse, int level, File32 the_dir)
    {
        char dir_name[255];
        the_dir.getName(dir_name, sizeof(dir_name));
        display(dbg_tfs,"doListRecursive(%s,%d,%d,%s)",full_name,recurse,level,dir_name);

        const char *ts = getDateTimeStamp(&the_dir,dir_name);

        s_Serial->print("file_reply:Directory Listing ");
        s_Serial->print(ts);
        s_Serial->print(",");
        s_Serial->println(full_name);

        File32 entry = the_dir.openNextFile();
        while (entry)
        {
            char filename[255];
            entry.getName(filename, sizeof(filename));

            if (entry.isDirectory())
            {
                const char *ts = getDateTimeStamp(&entry,filename);

                s_Serial->print("file_reply:");
                s_Serial->print(ts);
                s_Serial->print(",");
                s_Serial->print(filename);
                s_Serial->print("/");
                s_Serial->print("\r\n");
            }
            else
            {
                uint32_t size = entry.size();
                const char *ts = getDateTimeStamp(&entry,filename);

                s_Serial->print("file_reply:");
                s_Serial->print(ts);
                s_Serial->print(',');
                s_Serial->print(size);
                s_Serial->print(',');
                s_Serial->print(filename);
                s_Serial->print("\r\n");
            }

            entry = the_dir.openNextFile();

        }   // while (entry)

        // rewind if recurse

        if (recurse)
        {
            the_dir.rewindDirectory();
            File32 entry = the_dir.openNextFile();
            while (entry)
            {
                if (entry.isDirectory())
                {
                    entry.getName(dir_name, sizeof(dir_name));

                    char full_path[255];
                    strcpy(full_path,full_name);
                    strcat(full_path,"/");
                    strcat(full_path,dir_name);

                    doListRecursive(full_path,recurse,level+1,entry);
                }
                entry = the_dir.openNextFile();
            }

        }
    }
#endif  // RECURSIVE_LIST




#define MAX_DECODED_BUF   10240
    // the decoded base64 will always be less than the raw base64
    // and 10240 is agreed upon between console.pm and teensyExpression.
unsigned char decode_buf[MAX_DECODED_BUF];


void fileSystem::handleFileCommand(const char *command, const char *param)
{
    // DIRECTORY LIST

     display(dbg_tfs-1,"handleFileCommand %s - %s",command);

    if (!strcmp(command,"list"))
    {
        // send a blank response if the directory does not exist

        #if RECURSIVE_LIST

            char *recurse = (char *) param;
            char *dir_name = recurse;
            while (*dir_name && *dir_name != ',') dir_name++;
            if (*dir_name == ',') *dir_name++ = 0;

            display(dbg_tfs,"list command %s - %s",recurse,dir_name);

            File32 the_dir = SD.open(dir_name);
            if (the_dir)
            {
                doListRecursive(dir_name,*recurse=='1',0,the_dir);
            }

        #else
            const char *dir_name = param;
            File32 the_dir = SD.open(dir_name);
            if (the_dir)
            {
                const char *ts = getDateTimeStamp(&the_dir,dir_name);

                s_Serial->print("file_reply:Directory Listing ");
                s_Serial->print(ts);
                s_Serial->print(",");
                s_Serial->println(dir_name);

                File32 entry = the_dir.openNextFile();
                while (entry)
                {
                    char filename[255];
                    entry.getName(filename, sizeof(filename));

                    if (entry.isDirectory())
                    {
                        const char *ts = getDateTimeStamp(&entry,filename);

                        s_Serial->print("file_reply:");
                        s_Serial->print(ts);
                        s_Serial->print(",");
                        s_Serial->print(filename);
                        s_Serial->print("/");
                        s_Serial->print("\r\n");
                    }
                    else
                    {
                        uint32_t size = entry.size();

                        const char *ts = getDateTimeStamp(&entry,filename);
                        s_Serial->print("file_reply:");
                        s_Serial->print(ts);
                        s_Serial->print(',');
                        s_Serial->print(size);
                        s_Serial->print(',');
                        s_Serial->print(filename);
                        s_Serial->print("\r\n");
                    }

                    entry = the_dir.openNextFile();

                }   // while (entry)
            }   // if the_dir

        #endif  // !RECURSIVE_LIST
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
            s_Serial->print("file_reply:");
            s_Serial->print("ERROR - directory_already_exists ");
            s_Serial->print(path);
            s_Serial->print("\r\n");
        }
        else
        {
            display(dbg_tfs,"fileSystem::mkdir(%s) calling SD.mkdir()",param);

            // this snippet is how you have to set the timestamp on a directory

            strcpy(write_ts,ts);

			FsDateTime::setCallback(dtCallback);
            // FatFile::dateTimeCallback(dtCallback);
            int rslt = SD.mkdir(path);
			FsDateTime::clearCallback();
            // FatFile::dateTimeCallbackCancel();

            if (rslt)
            {
                #if 0   // as this does not work
                    File32 the_dir = SD.open(path);
                    if (the_dir)
                    {
                        setTimeStamp(the_dir,ts);
                        the_dir.close();
                    }
                #endif

                s_Serial->print("file_reply:");
                s_Serial->print("OK - created_directory ");
            }
            else
            {
                s_Serial->print("file_reply:");
                s_Serial->print("ERROR - could_not_create_directory ");
            }
            s_Serial->print(param);
            s_Serial->print("\r\n");
        }
    }   // mkdir command


    // GET FILE

    else if (!strcmp(command,"get"))
    {
        const char *filename = param;
        display(dbg_tfs,"fileSystem::get(%s)",filename);
        File32 the_file = SD.open(filename);

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

            s_Serial->print("file_reply:FILE ");
            s_Serial->print(ts);
            s_Serial->print(",");
            s_Serial->print(size);
            s_Serial->print(",");
            s_Serial->print(filename);
            s_Serial->print("\r\n");

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
                    s_Serial->print("file_reply:ERROR got ");
                    s_Serial->print(got_bytes);
                    s_Serial->print(" bytes while attempting to read ");
                    s_Serial->print(bytes_to_read);
                    s_Serial->print(" bytes at offset ");
                    s_Serial->print(file_off);
                    s_Serial->print("\r\n");
                    ok = 0;
                    break;
                }

                for (uint32_t i=0; i<got_bytes; i++)
                {
                    check_sum += buf[i];
                }

                int enc_len = base64_encode(b64_buf,(char *)buf,bytes_to_read);
                b64_buf[enc_len] = 0;

                s_Serial->print("file_reply:");
                s_Serial->print(b64_buf);
                s_Serial->print("\r\n");

                size -= bytes_to_read;
                file_off += bytes_to_read;
            }

            the_file.close();

            if (ok)
            {
                s_Serial->print("file_reply:CHECKSUM ");
                s_Serial->print(check_sum);
                s_Serial->print("\r\n");
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
                File32 the_dir = SD.open(dir);
                if (!the_dir.isDirectory())
                {
                    my_error("NOT A DIRECTORY (is a file): %s",dir);
                    s_Serial->print("file_reply:ERROR NOT A DIRECTORY .. is a file - ");
                    s_Serial->print(dir);
                    s_Serial->print("\r\n");
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
				FsDateTime::setCallback(dtCallback);
				// FatFile::dateTimeCallback(dtCallback);
				int rslt = SD.mkdir(dir);
				FsDateTime::clearCallback();
				// FatFile::dateTimeCallbackCancel();

                if (!rslt)
                {
                    my_error("COULD NOT CREATE DIRECTORY: %s",dir);
                    s_Serial->print("file_reply:ERROR - COULD NOT CREATE DIRECTORY ");
                    s_Serial->print(dir);
                    s_Serial->print("\r\n");
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
                s_Serial->print("file_reply:ERROR - COULD NOT OPEN FILE ");
                s_Serial->print(filename);
                s_Serial->print("\r\n");
                ok = 0;
            }
        }

        // return the first OK

        if (ok)
        {
            s_Serial->print("file_reply:OK PUT STARTED");
            #if 0
                s_Serial->print(ts);
                s_Serial->print(",");
                s_Serial->print(size);
                s_Serial->print(",");
                s_Serial->print(filename);
            #endif

            s_Serial->print("\r\n");
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
            s_Serial->print("file_reply:ERROR - write fail got=");
            s_Serial->print(bytes_written);
            s_Serial->print(" expected=");
            s_Serial->print(decoded_size);
            s_Serial->print(" at offset=");
            s_Serial->print(write_offset);
            s_Serial->print("\r\n");
        }
        else
        {
            for (int i=0; i<decoded_size; i++)
                write_checksum += decode_buf[i];
            write_offset += decoded_size;

            s_Serial->print("file_reply:OK PUT CONTINUE offset=");
            s_Serial->print(write_offset);
            s_Serial->print("\r\n");
        }
    }   // BASE64 - part 2 of 3 of put command

    else if (!strcmp(command,"CHECKSUM"))
    {
        uint32_t got = atoi(param);
        display(dbg_tfs,"CHECKSUM got=%d calculated=%d",got,write_checksum);
        if (got == write_checksum)
        {
            s_Serial->print("file_reply:OK CHECKSUM ");
            s_Serial->print(got);
            s_Serial->print("==");
            s_Serial->print(write_checksum);
            s_Serial->print("\r\n");
        }
        else
        {
            s_Serial->print("file_reply:ERROR CHECKSUM FAIL got(");
            s_Serial->print(got);
            s_Serial->print(") != calculated(");
            s_Serial->print(write_checksum);
            s_Serial->print(")\r\n");
        }

        setTimeStamp(write_file,write_ts);
        write_file.close();

    }   // CHECKSUM - part 3 of 3 of put command


    // DELETE FILE OR DIRETORY

    else if (!strcmp(command,"delete"))
    {
        display(dbg_tfs,"delete %s",param);
        File32 the_file = SD.open(param);
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
                s_Serial->print("file_reply:OK DELETE ");
                s_Serial->print(is_dir);
                s_Serial->print("\r\n");
            }
            else
            {
                s_Serial->print("file_reply:ERROR - ");
                s_Serial->print(is_dir?"rmRfStar(":"remove(");
                s_Serial->print(param);
                s_Serial->print(") failed!");
                s_Serial->print("\r\n");
            }
        }
        else
        {
            s_Serial->print("file_reply:ERROR - could not open '");
            s_Serial->print(param);
            s_Serial->print("'for deletion");
            s_Serial->print("\r\n");
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

        File32 the_file = SD.open(old_path);
        if (the_file)
        {
            uint32_t size = the_file.size();
            bool is_dir = the_file.isDirectory();
            const char *ts = getDateTimeStamp(&the_file,old_path);
            the_file.close();

            display(dbg_tfs,"rename is_dir=%d size=%d ts=%s",is_dir,size,ts);

            if (SD.rename(old_path,new_path))
            {
                s_Serial->print("file_reply:OK RENAME ");
                s_Serial->print(is_dir);
                s_Serial->print(",");
                s_Serial->print(size);
                s_Serial->print(",");
                s_Serial->print(ts);
                s_Serial->print("\r\n");
            }
            else
            {
                s_Serial->print("file_reply:ERROR - rename '");
                s_Serial->print(old_path);
                s_Serial->print("' to '");
                s_Serial->print(new_path);
                s_Serial->print("' FAILED");
                s_Serial->print("\r\n");
            }
        }
        else
        {
            s_Serial->print("file_reply:ERROR - could not open '");
            s_Serial->print(old_path);
            s_Serial->print("' for renaming");
            s_Serial->print("\r\n");
        }
    }


    s_Serial->print("file_reply_end");
    s_Serial->print("\r\n");

}




// end of fileSystem.cpp
