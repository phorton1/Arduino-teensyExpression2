//----------------------------------------------
// midiQueue.h
//----------------------------------------------
#pragma once

extern void enqueueMidi(uint8_t port, uint32_t msg32);
extern void enqueueMidi(uint8_t port, const uint8_t *bytes);


extern void dequeueMidi();
    // Dequeue any input messages and send them through the monitor logic for display

extern void sendMidiProgramChange(uint8_t port, uint8_t channel, uint8_t prog_num);
extern void sendMidiControlChange(uint8_t port, uint8_t channel, uint8_t cc_num, uint8_t value);

extern bool sendFTPCommandAndValue(uint8_t cmd, uint8_t val, bool wait = 1);
