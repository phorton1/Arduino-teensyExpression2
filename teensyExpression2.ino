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


// Uses slightly modified _usbDev.c that allows me to defer
// usb_init() call until I am ready, and _usbNames.c that
// works to allow overrides of teensy USB descriptors.

extern "C" {
    extern void my_usb_init();          // in usb_dev.c
    extern void setFTPDescriptors();    // _usbNames.c
}


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
    // mem_check("after serial port started");

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

	if (!fileSystem::init())
	{
        const char *msg = "    COULD NOT START FILE SYSTEM!!";
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

    // mem_check("at end of setup()");

}   // setup()




//-----------------------------------------
// loop
//-----------------------------------------

void loop()
{
    the_system.loop();
}
