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
    reset_prefs();
    prefs.DEBUG_DEVICE = OUTPUT_DEVICE_USB;
    prefs.FILE_SYS_DEVICE = OUTPUT_DEVICE_USB;
    prefs.FTP_PORT = 0; // 2;     // enum
    // prefs.SPOOF_FTP = 1;    // on

    // save_prefs();

    //-------------------------------------
    // Start the serial portS
    //-------------------------------------
    // start the external port

    Serial3.begin(115200);
    elapsedMillis serial_started = 0;
    while (serial_started<1000 && !Serial3) {}
    if (Serial3)
    {
        delay(1000);
        Serial3.println("hello from teensyExpressionsetup() " TEENSY_EXPRESSION_VERSION " Serial3 port");
    }

    // set the debug output to Serial3, or possibly nothing

    uint8_t debug_device = prefs.DEBUG_DEVICE;
    if (debug_device == OUTPUT_DEVICE_SERIAL)
    {
        dbgSerial = &Serial3;
    }
    else if (debug_device == OUTPUT_DEVICE_OFF)
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
    serial_started = 0;
    while (serial_started<1000 && !Serial) {}

    // output the initial message

    // delay(400);
    // mem_check("early in setup()");
        // at start of day 2023-08-05
        // mem used       8604
        // heap_used      0
        // free           253459
        // stack_used     81
        //
        // with buttons:
        // mem used       9564
        // heap_used      0
        // free           252499
        // stack_used     81
        //
        // with bare bones system
        // mem used       9596
        // heap_used      0
        // free           252475
        // stack_used     73
        //
        // added midiQueue, buttons, pedals, and rotaries
        // mem used       55216
        // heap_used      0
        // free           206855
        // stack_used     73

    display(0,"teensyExpression.ino " TEENSY_EXPRESSION_VERSION " setup() started",0);

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

    #if 0       // test all font sizes
        // Before calling all the fonts
        //
        // Sketch uses 184860 bytes (17%) of program storage space. Maximum is 1048576 bytes.
        // Global variables use 137280 bytes (52%) of dynamic memory, leaving 124864 bytes for local variables. Maximum is 262144 bytes.
        //
        // After calling all fonts:
        //
        // Sketch uses 221876 bytes (21%) of program storage space. Maximum is 1048576 bytes.
        // Global variables use 137280 bytes (52%) of dynamic memory, leaving 124864 bytes for local variables. Maximum is 262144 bytes.
        //
        // Fonts don't take too much room

        const ILI9341_t3_font_t *test_fonts[20] =
        {
            &Arial_12,
            &Arial_12_Bold,
            &Arial_14,
            &Arial_14_Bold,
            &Arial_16,
            &Arial_16_Bold,
            &Arial_18,
            &Arial_18_Bold,
            &Arial_20,
            &Arial_20_Bold,
            &Arial_24,
            &Arial_24_Bold,
            &Arial_28,
            &Arial_28_Bold,
            &Arial_32,
            &Arial_32_Bold,
            &Arial_40,
            &Arial_40_Bold,
            &Arial_48,
            &Arial_48_Bold,
        };

        for (int i=0; i<20; i++)
        {
            mylcd.fillScreen(TFT_BLACK);
            mylcd.setFont(*test_fonts[i]);
            mylcd.setCursor(5,5);
            mylcd.print("teensyExpression ");
            mylcd.print(TEENSY_EXPRESSION_VERSION);
            mylcd.println(" started ... ");
            delay(500);
        }
        mylcd.fillScreen(TFT_BLACK);
        mylcd.setFont(Arial_16);
        mylcd.setCursor(0,40);
    #endif  // test all fonts

    // write critical debugging messages to screen
    // with variable delays

    int do_delay = 2000;
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
    else if (debug_device == OUTPUT_DEVICE_SERIAL)
    {
        const char *msg = "    DEBUG_OUTPUT to hardware Serial3!";
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
        // Uses 3684 bytes of heap

    clearLEDs();
    showLEDs();
    setLEDBrightness(prefs.BRIGHTNESS);
    LEDFancyStart();

    // delay before clearing LEDs and screen

    if (do_delay)
        delay(do_delay);
    clearLEDs();
    showLEDs();
    mylcd.fillScreen(TFT_BLACK);

    //-----------------------------------
    // start the system
    //-----------------------------------

    display(0,"initializing system ...",0);
    the_system.begin();

    display(0,"initilizing midiHost",0);
    midi_host.init();

    display(0,"teensyExpression.ino setup() completed.",0);
    // mem_check("at end of setup()");
        // at start of day 2023-08-05
        // mem used       8604
        // heap_used      3684
        // free           249775
        // stack_used     81
        //
        // with buttons
        // mem used       9564
        // heap_used      2724      less??
        // free           249775    same!!
        // stack_used     81
        //
        // with bare bones system
        // mem used       9596
        // heap_used      2692      less?
        // free           249783    more?
        // stack_used     73
        //
        // added midiQueue, buttons, pedals, and rotaries
        //
        // mem used       55216     way more
        // heap_used      2128      less again!
        // free           204727    lost a lot
        // stack_used     73

}   // setup()




//-----------------------------------------
// loop
//-----------------------------------------

void loop()
{
    the_system.loop();
}
