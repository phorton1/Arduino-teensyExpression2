//-----------------------------------------
// midiHost.cpp
//-----------------------------------------
// whole file is not compiled if !WITH_MIDI_HOST

#include "midiHost.h"
#if WITH_MIDI_HOST

#include <myDebug.h>
#include "midiQueue.h"
#include "theSystem.h"

#define dbg_midi  1
    // show host: messages as they come in

// #define HOST_CABLE_BIT  0x80


USBHost myusb;
midiHost midi_host(myusb);


void midiHost::init()
{
    // Wait 1.5 seconds before turning on USB Host.  If connected USB devices
    // use too much power, Teensy at least completes USB enumeration, which
    // makes isolating the power issue easier.
    myusb.begin();
}


// made virtual in USBHost_t36.h
void midiHost::rx_data(const Transfer_t *transfer)
{
    uint32_t len = (transfer->length - ((transfer->qtd.token >> 16) & 0x7FFF)) >> 2;
    if (len)
    {
        bool any = 0;
        for (uint32_t i=0; i < len; i++)
        {
            uint32_t msg = rx_buffer[i];
            if (msg)
            {
                any = 1;
                usb_midi_write_packed(msg);

                if (msg !=  0x0000fe1f)     // for now
                {
                    int save_proc_level = proc_level;
                    proc_level = 1;
                    display(dbg_midi,"host:  0x%08x",msg);
                    proc_level = 2;

                    // port comes in as 0x00 or 0x10
                    // we bump it to MIDI_PORT_HOST1 = 0x40 or
                    // MIDI_PORT_HOST2 = 0x50;

                    enqueueMidi(MIDI_PORT_HOST1 | (msg & MIDI_PORT_NUM_MASK), msg);
                    proc_level = save_proc_level;
                }
            }
        }
        if (any)
            usb_midi_flush_output();
    }
    queue_Data_Transfer(rxpipe, rx_buffer, rx_size, this);
}




#if 0   //  not compiled yet

    bool passFilter(uint32_t iii)
    {
        // don't pass em if filter is not on

        msgUnion msg(iii);
        int type = msg.getMsgType();
        if (getPref8(PREF_PERF_FILTER))        // filter is on
        {
            // only accept messages from cable 0

            if (msg.isCable1())
                return 0;

            // only accept note on, note off, or pitchbends if the pref is cleared

            bool filter_bends = getPref8(PREF_PERF_FILTER_BENDS);
            if (type!=0x08 && type!=0x09 && (type!=0x0E || filter_bends))
                return 0;
        }

        // prh 2020-08-02 - removed initial hack of layer filters

        // send it to the teensyduino

        usb_midi_write_packed(msg.i);
        theSystem.midiActivity(INDEX_MASK_OUTPUT);
            // it IS port one, cable 0

        // if "monitor performanc" pref is set
        // enqueue it for display as PORT_INDEX_DUINO_OUTPUT0
        // with the PORT_MASK_PERFORM flag to display it differently

        if (getPref8(PREF_MONITOR_PERFORMANCE))
        {
            msg.i &= ~PORT_MASK;                            // clear the old port
            msg.i |= PORT_MASK_OUTPUT | PORT_MASK_PERFORM;  // output to teensyDuino0
            enqueueProcess(msg.i);
        }

        return 1;   // flush the usb_midi buffer
    }
#endif



#endif  // WITH_MIDI_HOST
