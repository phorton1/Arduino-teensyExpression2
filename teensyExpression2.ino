//---------------------------------------------------
// teensyExpression2.ino
//---------------------------------------------------
#include <myDebug.h>
#include "src/defines.h"
#include "src/myTFT.h"
#include "src/prefs.h"
#include "src/myLeds.h"
#include "src/theSystem.h"
#include "src/fileSystem.h"
#include "src/midiHost.h"


//--------------------------------------------------
// teensyThreads teensy 3.x specific malloc fix
//--------------------------------------------------
// sbrk() and STACK_MARGIN define copied from
// 		C:\Program Files (x86)\Arduino\hardware\teensy\avr\cores\teensy3\mk20dx128.c
// need to include <errno.h> and declare extern char* __brkval;

extern "C" {

	#include <errno.h>

	#ifndef STACK_MARGIN
		#if defined(__MKL26Z64__)
			#define STACK_MARGIN  512
		#elif defined(__MK20DX128__)
			#define STACK_MARGIN  1024
		#elif defined(__MK20DX256__)
			#define STACK_MARGIN  4096
		#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
			#define STACK_MARGIN  8192
		#else
			#define STACK_MARGIN  1024
		#endif
	#endif

	extern char* __brkval;      // current end (top) of heap

	void * _sbrk(int incr)
		// copied from
	{
		char *prev, *stack;
		prev = __brkval;
		if (incr != 0) {

			// this is the only change, really:
			// instead of following asm which gets sp register
			// 	__asm__ volatile("mov %0, sp" : "=r" (stack) ::);
			// we get the msp ('main' thread) stack register like this:
			__asm__ volatile("mrs %0, msp" : "=r" (stack) ::);
			if (prev + incr >= stack - STACK_MARGIN) {
				errno = ENOMEM;
				return (void *)-1;
			}
			__brkval = prev + incr;
		}
		return prev;
	}

}	// extern "C"



void setup()
{
    bool prefs_reset = read_prefs();

    // things I might want to do:
    //
    // reset_prefs();
    // prefs.DEBUG_DEVICE = DEBUG_DEVICE_USB;
    // prefs.FTP_PORT = 0; // 2;     // enum
    // prefs.SPOOF_FTP = 1;    // on
    // save_prefs();

    //-------------------------------------
    // Start the external serial port
    //-------------------------------------
    // start the

    SERIAL_DEVICE.begin(115200);
    delay(500);

    // elapsedMillis serial_started = 0;
    // while (serial_started<1000 && !SERIAL_DEVICE) {}
    // if (SERIAL_DEVICE)
    // {
    //     delay(1000);
    //     SERIAL_DEVICE.println("hello from teensyExpressionsetup() " TEENSY_EXPRESSION_VERSION " Serial port");
    // }

    // set the debug output to SERIAL_DEVICE, or possibly nothing

    if (prefs.DEBUG_DEVICE == DEBUG_DEVICE_SERIAL)
    {
        dbgSerial = &SERIAL_DEVICE;
    }
    else if (prefs.DEBUG_DEVICE == DEBUG_DEVICE_OFF)
    {
        dbgSerial = 0;      // turns off output in myDebug.cpp
    }

    //----------------------------------------
    // start the USB
    ///---------------------------------------
    // optionally call setFTPDescriptors()

    if (prefs.SPOOF_FTP)
        setFTPDescriptors();
    my_usb_init();
    delay(1000);

    //----------------------------------------
    // initialize the main USB serial port
    //----------------------------------------

    Serial.begin(115200);
    delay(500);
    // serial_started = 0;
    // while (serial_started<1000 && !Serial) {}

    display(0,"teensyExpression.ino " TEENSY_EXPRESSION_VERSION " setup() started",0);
	// mem_check("at start of setup()");

    //-----------------------------------------------
    // start the TFT display
    //-----------------------------------------------

    initMyTFT();

    mylcd.setTextBackColor(0);
    mylcd.setTextColor(TFT_WHITE);
    mylcd.setFont(Arial_16);
    mylcd.setCursor(5,5);
    mylcd.print("teensyExpression ");
    mylcd.print(TEENSY_EXPRESSION_VERSION);
    mylcd.println(" started ... ");

    // write critical debugging messages to screen

    int do_delay = 0;
    mylcd.setTextColor(TFT_YELLOW);

    if (prefs_reset)
    {
        const char *msg = "    PREFS WERE AUTOMATICALLY RESET!!";
        warning(0,"%s",msg);
        mylcd.println(msg);
        do_delay = 5000;
    }
    if (prefs.SPOOF_FTP)
    {
        const char *msg = "    SPOOFING FTP!!";
        warning(0,"%s",msg);
        mylcd.println(msg);
        do_delay = 5000;
    }
    if (!dbgSerial)
    {
        const char *msg = "    NO SERIAL PORT IS ACTIVE!!";
        warning(0,"%s",msg);
        mylcd.println(msg);
        do_delay = 5000;
    }
    else if (prefs.DEBUG_DEVICE == DEBUG_DEVICE_SERIAL)
    {
        const char *msg = "    DEBUG_OUTPUT to SERIAL_DEVICE!";
        warning(0,"%s",msg);
        mylcd.println(msg);
        do_delay = 5000;
    }

    #if !defined(USB_MIDI4_SERIAL)
        const char *msg = "    NOT COMPILED WITH USB_MIDI4_SERIAL !!!";
        warning(0,"%s",msg);
        mylcd.println(msg);
        do_delay = 5000;
    #endif

    //----------------------------------
    // start the LEDs
    //----------------------------------
    // clear and show a fancy pattern

    display(0,"initializing LEDs ...",0);

    initLEDs();
    clearLEDs();
    showLEDs();
    setLEDBrightness(prefs.BRIGHTNESS);
    LEDFancyStart();

    //--------------------------------
    // start the file system
    //--------------------------------
	// this warning message, and placement of initFileSystem()
	// is slightly different between TE1 and TE1
	
	uint8_t dd = prefs.DEBUG_DEVICE;
	uint8_t fsd = prefs.FILE_SYS_DEVICE;
    warning(0,"FILE_SYS_DEVICE %s",
		fsd == OUTPUT_DEVICE_SERIAL ? "is SERIAL" :
		fsd == OUTPUT_DEVICE_USB 	? "is USB" :
		fsd == OUTPUT_DEVICE_OFF 	? "is OFF!!" :
		dd == DEBUG_DEVICE_SERIAL 	? "follows DEBUG_DEVICE which is SERIAL" :
		dd == DEBUG_DEVICE_USB 		? "follows DEBUG_DEVICE which is USB" :
		"follows DEBUG_DEVICE which is OFF" );

	initFileSystem();
	if (!hasSDCard())
	{
        const char *msg = "    NO SD CARD FOUND!!";
        warning(0,"%s",msg);
        mylcd.println(msg);
        do_delay = 5000;
	}
	else if (!hasFileSystem())
	{
        const char *msg = "    COULD NOT START FILE SYSTEM!";
        warning(0,"%s",msg);
        mylcd.println(msg);
        do_delay = 5000;
	}

    //-----------------------------------
    // start the system
    //-----------------------------------

    display(0,"initializing system ...",0);
    the_system.begin();

    display(0,"initilizing midiHost",0);
    midi_host.init();


    //--------------------------
    // done
    //--------------------------
    display(0,"teensyExpression.ino setup() completed.",0);

    if (do_delay)
        delay(do_delay);

    clearLEDs();
    showLEDs();
    mylcd.fillScreen(TFT_BLACK);
	//	mem_check("at end of setup()");

}   // setup()




//-----------------------------------------
// loop
//-----------------------------------------

void loop()
{
	freeFileCommands();
    the_system.loop();
}
