# integrated.md

Integrating the Looper2 box and the teensyExpression2 box into one box,
eliminating cables in favor of built-in connectors, and consolidating
to a single power supply.

I would like to have the TE work with only the following connections:

- A single external power supply or an AC input
- Guitar Input
- Four Pedals Plug in
- Single USBC-L cable to iPad
- RCA phono pair to PA

Thus eliminating:

- 2 of 3 power supplies to rPi, Hub, and iPad
- serial cable between TeensyExpression and Looper Box (teensy3.6 to teensy3.2)
- USB cable between teensyExpression and Looper (teensy3.6 to USB Hub)

The rebuild would also more carefully modify a iRigHD2

- the LED (4 pins, 5V, RGB pulled to ground)
- Gain Knob (two pins, floating I think)
- Volume Knob (three pins, or maybe 2 plus GND)
- possibly the Guitar Plug
- and if so, or maybe even if not, the Pad Connection





## Current

- rPi Looper
  - rPiConnector
  - Audio In
  - Audio Out
  - Display Connector
  - POWERED BY 3A MICRO-USB
- teensyPiLooper
  - led/switch board
  - separate micro-USB port
  - POWERED BY RPI

- USBHub
  - USB 3.2 IO
  - POWERED BY SEPARATE 5V POWER SUPPLY
- teensyExpression2 motherboard
  - switch harness
  - rotary board
  - expression pedal inputs
  - display
  - POWERED BY USB HUB
- iRig2HD
  - USB IO  (incl power)
  - Audio Out
  - Guitar In
  - LED
  - Gain Control
  - Volume Control
  - Pad
  - POWERED BY USB HUB
- FTP Dongle
  - POWERED BY USB HUB

- iPad USB 'Camera' Adapter
  - NEEDS TWO CABLES TO THE IPAD
  - USB 3.2
  - SEPARATE APPLE POWER SUPPY


### iPad charging and data issues

L = Lightning

The USB+L->L camera adapter appears to be needed because the
existing USB2/3 hubs that I have don't charge the iPad using any
USBA-Lightning cables I have.

However, the newer 30W USBC power supply DOES charge the iPad with
the two good USB-L cables I have.  And it also charges the iPad with
the single USBC-L cable I have. AND it also charges with the USBC-L
with the short, but not the long, L-L extension cable.

Therefore there is *hope* that a USBC "power delivery" hub *may* solve
the problem by allowing me to power the hub from the USBC charger, snd
then using it to combine the three needed USB2/3 devices (the iRigHD2,
FTP, and teensyExpression) into one signal going over a single cable to
the iPad.

However, no combination of power supplies and hubs I currently have
will do this and allow me to run a single cable to the iPad.



## Main Ideas


- Power the whole thing with one USBC plug
- Combine the teensy3.2 and teensy3.6 onto one PCB
- Switch to TE to i2C display
- Looper unchanged
- USB Port relationship unchanged


## Displays

I want to eliminate the big 7" rPI touchscreen from the Looper in favor of
two smaller 3.5" 480x320 screens.  In addition, I would entirely eliminate
the 'Touch' functionality from both the Looper and the teensyExpression code

- the Touch was never implemented to do anything on the TE
- the Touch is only nominally used on the Looper and is not needed
- the Touch on the 3.5" screens is resistive and doesn't work well on rPi

For the TE part I *may* (likely) use the ILI9488_t3 library with the
3.5" Orange displays I have.  Interestingly I bought 5 WITHOUT touch
screens, they are NOT marked XX, in a baggy in the Extra Displays
tupperware.  I *started* to modify my Arduino/libraries/myLCD library
and the corresponding Arduino/_test/myLcd_test.ino program to
for an SPI implementation (of what is currently only parallel)
myLCD library.  The test ALSO works with the ILI9488_t3 library,
showing that I just need to change a few API calls to use it,
which is highly optimized to use DMA, instead of re-inventing
the wheel with my own, new, likely bare-bones, implmentation.

For the rPI part, it is worth noting that I already have
circle/_prh/ ILI9846 device that works with the BLUE
"3.5" RPi Display" 480x320 device that have xpt2406 touch
sensors. . There are two of them, marked XX in separate
bubble wrap package s in the Extra Displays Tupperware.
It seems like it would be less work, if I wanted, to add an
ILI9488 to that than it would be to add it to myLCD, inasmuch
as my rPi ILI9486 is a small piece of code, and I would probably
only need to implement the configuration and a few other minor
routines.

REMEMBER that Circle is NOT Raspian!  Plugging the
"3.5" RPi Display* into a rPi does not make it just
up and work under Raspian.  There is probably some other
configuration needed that I don't have documented
right now.


## SoundCard

I located the schematics for the breakout boards for the
AudioInjector Octo.  I believe I can eliminate the included
2x6 and 2x8 RCA connectors in favor of a single 2xRCA output
connector, and an internal Audio In board that works with the
audio from the IRig2 HD.


## 2 Wire Switch Array Sensing

I believe I can free up 7 pins on the Teens3.6 by using
the new "row_board" design, which uses 74C595 serial to
parallel data converters on each board, using a clock
and data line to "turn on" each swtich, and a single common
line back from all of them for sensing.




## Checking stuff in

I have added a lot of stuff that is only "test" in nature,
as I have explored this.  Because I am tkaing my machine to
Panama City next week, I am going to go ahead and check it
in.  I can remove stuff I don't want later.
