// teensyExpression2.ino
//
// first files included in new project src/ are my modified
//     usb_desc_prh.h and usb_desc.c files which setup the
//     teensyExpression and teensyControl midi ports.
// It appears as if /src is a special directory where
//     code will be compiled from.  So other directories
//     can exist like /obsolete or /test etc, however,
//     you must do a clean build after moving files out
//     of /src.
// I am not including the ability to spoof the FTP in this
//     version.  Therefore I did NOT move the modified usb_dev.c
//     from the old teensyExpression project to this one,
//     and am not calling the externs it provides:
//     my_usb_init() or setFishmanFTPDescriptor(() which
//     must be called before my_usb_init() and before
//     the main Serial port is opened.
// I am not including the midi_host in teensyExprssion2
//     do not copying myMidiHost.h or cpp


#include <myDebug.h>
#include "src/defines.h"
#include "src/myTFT.h"
#include "src/prefs.h"
#include "src/myLeds.h"
#include "src/theSystem.h"



void setup()
{
    read_prefs();

    // things I might want to do:
    //
    reset_prefs();
    prefs.DEBUG_DEVICE = DEBUG_DEVICE_USB;
    save_prefs();

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
    if (debug_device == DEBUG_DEVICE_SERIAL)
    {
        dbgSerial = &Serial3;
        display(0,"debugging output redirected to Serial3",0);
    }
    else if (debug_device == DEBUG_DEVICE_OFF)
    {
        dbgSerial = 0;      // turns off output in myDebug.cpp
    }

    // initialize the main USB serial port

    Serial.begin(115200);
    serial_started = 0;
    while (serial_started<1000 && !Serial) {}

    // output the initial message

    delay(400);
    mem_check("early in setup()");
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
    read_prefs();

    //-----------------------------------------------
    // start the TFT display
    //-----------------------------------------------

    initMyTFT();

    mylcd.Set_Text_Back_colour(0);
    mylcd.Set_Text_colour(TFT_WHITE);
    mylcd.setFont(Arial_16);
    mylcd.Set_Text_Cursor(5,5);
    mylcd.print("teensyExpression ");
    mylcd.print(TEENSY_EXPRESSION_VERSION);
    mylcd.println(" started ... ");

    // write critical debugging messages to screen
    // with variable delays

    int do_delay = 2000;
    if (!dbgSerial)
    {
        mylcd.Set_Text_colour(TFT_YELLOW);
        mylcd.println("    NO SERIAL PORT IS ACTIVE!!");
        do_delay = 3000;
    }
    else if (debug_device == DEBUG_DEVICE_SERIAL)
    {
        mylcd.Set_Text_colour(TFT_YELLOW);
        mylcd.println("    DEBUG_OUTPUT to hardware Serial3!");
        if (!do_delay) do_delay = 1200;
    }

    #if !defined(USB_MIDI4_SERIAL)
        error("PROGRAM IS NOT COMPILED UNDER USB_MIDI4_SERIAL teensyDuino type!! Things may not work correctly!!!",0);
        mylcd.Set_Text_colour(TFT_YELLOW);
        mylcd.println("    NOT COMPILED WITH USB_MIDI4_SERIAL !!");
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
    mylcd.Fill_Screen(TFT_BLACK);

    //-----------------------------------
    // start the system
    //-----------------------------------

    display(0,"initializing system ...",0);
    theSystem.begin();

    display(0,"teensyExpression.ino setup() completed.",0);
    mem_check("at end of setup()");
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
    theSystem.loop();
}
