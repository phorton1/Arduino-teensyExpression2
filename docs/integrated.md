# Integrated teensy 4.1 system

I have decided to take a quantum leap backwards and forwards at the same time.

## OLD SYSTEM

The previous highest quality solution I could come up with was arrived at by
building the system using discreet commercial devices, then removing the the
PCB's from those devices and repackaging them in 3D printed boxes, combined
with custom programming of 3 separate MPUs.

- IRigHD2 for the USB Audio interface
- 4/6 channel USB 3.2 Hub to combine the USB Audio IO with the FTP and Pedal Midi
- Apple USB to Lightning dongle for USB IO and charging power to older iPad

The custom MPUs included

- rPi 3B+ for the looper
- teensy 3.6 for the Pedal
- teensy 3.2 for to combine Serial IO and reboot the rPi

Which resulted in 3 separate power supplies and many cables/connectors:

- Power supply for the rPi and teensy 3.2
- Power supply for the USB hub, which powered the Pedal and FTP dongle
- Power supply for the iPad that went to the Apple dongle
- USB Cable between the Looper (Hub) and the Pedal
- Serial Cable between the Looper (teensy 3.2) and the Pedal
- Internal USB Cable between the Hub and the IRigHD2
- Audio cable between the IRigHD2 and the Octo Sound Card
- USB3.2 AND power to the Apple Dongle which HAD to be near the iPad

With the big, bulky, 6 input, and 8 output Octo Soundcard RCA
adapters crammed into the looper box, even though I only ever
used a stereo pair of them.




## New System

When trying to cram all of this into the new teensyExpression3 box
I reached a breaking point.  There is just too much "stuff" that is
not needed.  The rPi 3B+, with it's unused USB and ethernet ports
form factor is not needed. The breakthrough realization was that
I can do my own USB Audio device using the teensy, and with dual
I2S, more properly use a rPi zero as the Looper device.  If I need
a quad core rPi, there is now the rPI Zero 2W.

The new system will be based on a teensy 4.x

Some experimentation may be needed to get to where I want to be,
particularly with multiple ILI9488 displays and FTP latency.
However, the basics are as follows:

- Instead of using the IRig HD 2 for a USB audio interface,
  I will use the teensy 4.x for the USB Audio IO
- Instead of using a USB hub for the FTP,
  I will solely rely on the USB host on the teensy 4.x
- I will use the teensyQuad device to interface to
  an SGTL5000 (teensy audio shield) and and RPI
  simultaneously.

One of the main concerns is how much a teensy 4.1 can
handle.  Previously the teensy 3.6 was responsible for

- Polling the Pedal buttons
- Polling the Pedal rotaries
- Polling the Expression Pedals
- implementing a (Midi) Host Device that was not mainline functionality
- generating Midi Output via USB
- generating Serial Midi via a UART
- writing the WS2812b LEDs
- writing to the TFT device for the UI
- implementing the User Interface
- doing Midi Monitoring
- and so on ...

In addition there is a temptation to try to get rid of the
teensy3.2 (teensyPiLooper) device and wrap that up into
the teensy 4.1 as well.

Even though the 4.1 is faster, I'm not sure I can just lump the
additional USB Audio and Dual I2S processing, and THEN the
teensyPiLooper responsibilities, on top of it.

I suspect I will still need another teensy in the system.
In fact, I might want to move functionality OFF of the
I2S/Audio/Midi MPU and ONTO a separate MPU.


## Phased approach with incremental development.

The new list of most critical functionality is for the
teensy 4.x to be the USB Audio and Midi device with as
good of a (midi) HOST port as possible.  I'm not at all
sure about wrapping all the other functionality into
the 4.1x device.

- midi HOST port for FTP
- USB audio device for iPad
- MIDI device(s) for iPad

If I throw the other things onto the 4.1 they would include:

- Polling Pedal Buttons
- Polling Pedal Rotaries
- Polling the Expression Pedals
- outputting to the WS2812B LEDs
- outputting the ILI9488_T3 screen
- implementing the User Interface

None of that stuff is intensly time critical.

I think I will design a PCB "as if" the 4.1 was
going to do everything, with the exception of
the teensyPiLooper functionality (coordinating
serial ports, rebooting the pi, handling uploads
to the pi).  I can then build it and test it
incrementally.

The main functions will be to implement the teensyQuad
device to interface with the rPi and provide IO.




## Later

- Better Sound than SGTL500
- iPad USBC and general power situation


## NEW ARCHITECTURE

teensy4.0 bottom connections:

- gpio32 to VBat pin
- D- to Program pin
- D+ to 3.3V pin

Requirements

- teey4.0 is THE USB Audio and USB Midi Device
- It is also a Serial Device for debugging
- Processes LINE_IN from the SGTL500 on teensyQuad channels 1&2
- Sends the LINE_IN to the USB Audio_OUT (to the iPad)
- Receives USB Audio IN (from the iPad) and sends it to the rPI
- Sends USB Audio IN to rPI on teensyQuad channels 3&4.
- Recieves teensyQuad channels 3&4 from rPI and sends them to SGTL500
- Sends to SGTL on teensyQuad channels 1&2

Midi Host

- Is a USB MIDI Host
- Critical - as fast as possible, sends FTP midi to iPad Synth for mixing there.

Serial Midi
- Has less critical Serial serial midi handler
- Recieves Serial MIDI on Serial4 UART
- Intercepts serial MIDI API to all relevant SGTL5000 functions
- Forwards all other Serial MIDI in both directions over Serial4

Serial Debugging
- Normally outputs debugging to Serial4
- Note protocol where MIDI USB cannot be confused for text by recipients
- Can optionally use USB Serial Port for debug output

Does NOT have a direct Serial connection to the rPi.
I think there will be a teensy4.1 that IS the loop pedal processor,
which will ALSO absorb the previous teensyPiLooper functionality,
rebooting the Pi, handling the SD Card File system, and so on.


## Proof of Concept

It is a pain in the ass to have to modify the 4.0, but I think I must,
or *might as well*.  I will stack the SGTL5000 on top of it, and for
now will use my RevB adapter PCB to get it to work.

There will be a PCB that connects the teensy4 to the rPi.

That PCB must also provide connections for:
- TX_ACTIVE, RX_ACTIVE, RPI_RUN, RPI_SERIAL

, these will be mounted on a purble breadboard, to provide
access to the Host USB pins, Ground, and 5V, and access to the Ground
TX and RX for Serial4.

With a old single core rPI zero I don't think I'll be able to do the UI.
Nonetheless, I think I will design a circuit board that provides the
connectors for the display, and the teensy 4.1 "motherboard".


