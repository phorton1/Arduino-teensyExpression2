//----------------------------------------------
// midiQueue.h
//----------------------------------------------
#pragma once


#define MIDI_PORT_USB1        0x00
#define MIDI_PORT_USB2        0x10
#define MIDI_PORT_USB3        0x20
#define MIDI_PORT_USB4        0x30
#define MIDI_PORT_HOST1       0x40
#define MIDI_PORT_HOST2       0x50
#define MIDI_PORT_SERIAL      0x60

#define MIDI_PORT_NUM_MASK    0xf0

#define MIDI_ENUM_TO_PORT(i)  (i) << 4
#define MIDI_PORT_TO_ENUM(p)  (p) >> 4
    // converts port as enumerated in prefs,statements, to a port number

extern void enqueueMidi(uint8_t port, uint32_t msg32);
extern void enqueueMidi(uint8_t port, const uint8_t *bytes);


extern void dequeueMidi();
    // Dequeue any input messages and send them through the monitor logic for display

extern void sendMidiProgramChange(uint8_t port, uint8_t channel, uint8_t prog_num);
extern void sendMidiControlChange(uint8_t port, uint8_t channel, uint8_t cc_num, uint8_t value);

extern bool sendFTPCommandAndValue(uint8_t cmd, uint8_t val, bool wait = 1);
