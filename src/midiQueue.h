//----------------------------------------------
// midiQueue.h
//----------------------------------------------
#pragma once

#define FTP_ACTIVE_PORT (   \
    prefs.SPOOF_FTP ? MIDI_PORT_HOST1 : \
    prefs.FTP_PORT == 1 ? MIDI_PORT_USB1 : \
    prefs.FTP_PORT == 2 ? MIDI_PORT_HOST1 : 0 )


extern void enqueueMidi(uint8_t port, uint32_t msg32);
extern void enqueueMidi(uint8_t port, const uint8_t *bytes);

extern void dequeueMidi();
    // Dequeue any input messages and send them through the monitor logic for display

extern void sendMidiProgramChange(uint8_t port, uint8_t channel, uint8_t prog_num);
extern void sendMidiControlChange(uint8_t port, uint8_t channel, uint8_t cc_num, uint8_t value);

extern bool sendFTPCommandAndValue(uint8_t ftp_port, uint8_t cmd, uint8_t val, bool wait = 1);
