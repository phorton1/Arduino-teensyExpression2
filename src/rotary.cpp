//--------------------------------------------------------------------------
// rotary.cpp
//-------------------------------------------------------------------------
// Rotaries are initialized from prefs, but can then be modified,
// initRotary() can be called more than once

#include <myDebug.h>
#include "rotary.h"
#include "prefs.h"
#include "midiQueue.h"  // for sendCC methods


#define dbg_rotary   1
#define dbg_midi_cc  0



#define INCS_PER_REV        40.00
#define ROTARY_INC_DEC     (128.00 / (0.80 * INCS_PER_REV))


typedef struct
{
    int pinA;           // the pin for A polling
    int pinB;           // the pin for B polling
}   rotaryPin_t;


typedef struct
{
    char    name[MAX_PEDAL_NAME + 1];
    uint8_t port;
    uint8_t channel;      // zero based
    uint8_t cc;
    bool    listen;
    int     pollA;        // the last value polled for the A part of the switch
    float   value;        // the current value
}   rotary_t;



const rotaryPin_t rotary_pin[NUM_ROTARY] =
{
    { ROTARY_1A, ROTARY_1B },
    { ROTARY_2B, ROTARY_2A },
    { ROTARY_3A, ROTARY_3B },
    { ROTARY_4B, ROTARY_4A },
};


rotary_t rotary[NUM_ROTARY];



void initRotary()
    // The system is intended to be used with a rig.
    // We assign a default name, port, channel, and CC
    // that are not really meaningful
{
    for (int i=0; i<4; i++)
    {
        sprintf(rotary[i].name,"Rotary%d",i);

        rotary[i].port    = MIDI_PORT_USB1;
        rotary[i].channel = 0;
        rotary[i].cc      = 11 + i;
        rotary[i].listen  = 0;

        pinMode(rotary_pin[i].pinA,INPUT);  // _PULLDOWN);
        pinMode(rotary_pin[i].pinB,INPUT);  // _PULLDOWN);

        rotary[i].value  = 0.0;
        rotary[i].pollA = digitalRead(rotary_pin[i].pinA);
    }
}


int getRotaryValue(int i)
{
    return rotary[i].value;
}


void setRotaryValue(int num, int value)
    // note that the stored value is a float
    // but the returned value is a truncated int
{
    if (value > MIDI_MAX_VALUE)
        value = MIDI_MAX_VALUE;
    if (value < 0)
        value = 0;
    rotary[num].value = value;
}


void setRotary(int num, const char *name, uint8_t port, uint8_t channel, uint8_t cc, bool listen)
{
    display(dbg_rotary,"setRotary(%d)(0x%02x,%d,0x%02x) listen=%d",num,port,channel,cc,listen);
    memcpy(rotary[num].name,name,MAX_PEDAL_NAME);
    rotary[num].name[MAX_PEDAL_NAME] = 0;
    rotary[num].port    = port;
    rotary[num].channel = channel;
    rotary[num].cc      = cc;
    rotary[num].listen  = listen;
}


void onRotaryMidiCC(const msgUnion &msg)
{
    // display(0,"onRotaryMidiCC(0x%02x,%d,0x%02x,0x%02x)",
    //     msg.port(), msg.channel(), msg.param1(), msg.param2(), msg.param2());

    for (int i=0; i<NUM_ROTARY; i++)
    {
        //display(0,"checking(%d,0x%02x,%d,0x%02x) listen=%d",
        //    i,
        //    rotary[i].port,
        //    rotary[i].channel,
        //    rotary[i].cc,
        //    rotary[i].listen);

        if (!msg.isOutput() &&
            rotary[i].listen &&
            rotary[i].port == msg.port() &&
            rotary[i].channel == msg.channel()-1 &&
            rotary[i].cc == msg.param1())
        {

            display(dbg_midi_cc,"---> onRotaryMidiCC(%d,0x%02x,%d,0x%02x,0x%02x) setting value to %d",
                i, msg.port(), msg.channel(), msg.param1(), msg.param2(), msg.param2());
            setRotaryValue(i,msg.param2());
        }
    }
}


static bool _pollRotary(int i)
{
    int aval = digitalRead(rotary_pin[i].pinA);
    if (rotary[i].pollA == aval)
        return false;

    // only do something if A has changed

    rotary[i].pollA = aval;

    int bval = digitalRead(rotary_pin[i].pinB);
    if (aval == bval)
    {
        if (rotary[i].value + ROTARY_INC_DEC > MIDI_MAX_VALUE)
            rotary[i].value = MIDI_MAX_VALUE;
        else
            rotary[i].value += ROTARY_INC_DEC;
    }
    else
    {
        if (rotary[i].value - ROTARY_INC_DEC < 0)
            rotary[i].value = 0;
        else
            rotary[i].value -= ROTARY_INC_DEC;
    }

    if (dbg_rotary <= -2) delay(50);
    display(dbg_rotary+2,"rotary(%d) aval=%d bval=%d   value=%3.1f",i,aval,bval,rotary[i].value);
    return true;
}


void pollRotary()
{
    for (int i=0; i<NUM_ROTARY; i++)
    {
        if (_pollRotary(i))
        {
            display(dbg_rotary+1,"rotary(%d) sending(0x%02x,%d,0x%02x,%d)",
                i,
                rotary[i].port,
                rotary[i].channel+1,
                rotary[i].cc,
                rotary[i].value);

            sendMidiControlChange(
                rotary[i].port,
                rotary[i].channel + 1,
                rotary[i].cc,
                rotary[i].value);
        }
    }
}
