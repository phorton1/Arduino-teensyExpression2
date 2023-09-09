//-------------------------------------------
// rotary.h
//-------------------------------------------
#include "midiQueue.h"

#pragma once


void initRotary();
void pollRotary();

int getRotaryValue(int i);
void setRotaryValue(int num, int value);
void setRotary(int num, const char *name, uint8_t port, uint8_t channel, uint8_t cc, bool listen=0);
    // set the Rotary output - note that channel is zero based
	// and that an actual port number is specified
void onRotaryMidiCC(const msgUnion &msg);
