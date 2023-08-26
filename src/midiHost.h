//--------------------------------------------
// midiHost.h
//--------------------------------------------
// empty if !WITH_MIDI_HOST

#pragma once

#include "defines.h"

#if WITH_MIDI_HOST

    #include <Arduino.h>
    #include <USBHost_t36.h>

    class midiHost : public MIDIDevice
        // requires slightly modified USBHost_t36.h
    {
        public:

            midiHost(USBHost &host) : MIDIDevice(host)  {}

            void init();

            virtual void rx_data(const Transfer_t *transfer) override;
                // virtual override to handle USB irq,
                // write packet to teensyDuino device,
                // and enqueue packet for processing
    };

    extern midiHost midi_host;

#endif  // WITH_MIDI_HOST
