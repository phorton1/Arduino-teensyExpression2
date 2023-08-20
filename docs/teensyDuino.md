# teensyExpression2 - upgrade to Teensy 4.0

After likely cable problems using the Arduino 3.5" TFT Shield.
and noting my lack of extra teensy 3.6's, coupled with the fact
that they are being discontinued, I took the bold step of upgrading
teensyExpresion2 to the teensy 4.1.

The project is a mess at this point.  I was in the middle of implementing
the rigMachine when the TFT stopped displaying stuff.  So I disassembled
the device, pulled out the circuit board and display, and

I made a new 8 pin power cable to the LCD, with no joy.  Nothing I did
would make the TFT display stuff.

- maybe the data cable is bad
- the dupont connectors on the perf board circuit may be bad
- the teensy 3.6 itself may be bad
- maybe it's the code.

In this mess, I decided to cleanup the LCDWIKI stuff and put it in a **new
library called myLcd**.  That was a significant effort.  It didn't help,
but now I have two versions of the same code, and though I am trying
to keep the old TeensyExpression project workable, that is starting to
become impossible.

However, I WAS able to plug the Arduino 3.5" shield into an actual Uno,
and get a couple of test programs working with both myLcd and the
old LCDWIKI stuff.  So, it's not the display itself.


## Issues with using the Arduino 3.5" shield

- The way the connectors are aligned, it does not fit well into the 3D
printed display mount.  This puts stress on the connectors, and may have
led to the connectors not working.
- It uses 16 pins. On the tiny footprint for the teensy motherboard,
it would be difficult to put JST connectors in for the display.
In fact I want to put JST connectors for everything and do away with
using Dupont connectors for serious things.
- It is not bread-boardable.  The stupid Arduino footprint has a
1/2 spacing on one side of the connector, and those pins are offset
by 1/2 of a space.  So, if I want to even make a test situation where
I eliminate the cables, I would have to design and make my own PCB
just to support the stupid Arduino connector spacing.


## modified **platform_setup.pm**

Although I have 3 of those blue shields, I have several orange 3.5"
displays that are based on the same ILI9486 as the cheap blue ones.
They both use the ILI9486, although the blue ones are parallel and
the orange ones are serial, so I would have to come up with a way to
do serial ILI9486 on the teensy.

As a result I then started wondering how I would get the Serial SPI
TFT functionality onto the teensy platform.  I had three basic approaches
available to me.

- modify myLcd to do Serial comms based on some define
- modify the branched ILI9341_t3 library to work with the ILI9486 (based on some define)
- try using the TFT_eSPI library on the teensy (based on some define)

So, at that point I got the bright notion to modify **setup_plaform.pm** to allow it
to setup defines for Arduino and teensy platforms as well as the esp32, which I did,
before realizing that the *platform.txt* for those boards is in the read-only
*Program Files (x86)* directory, so although the code works, the functionality doesn't.

I even tested it against the esp32 projects, added a $TFT_DEFINES in platform_setup.pm,
passsed them to the TFT_eSPI library and verified that it works.  However, I *could*
have accomplished what I'm trying to do by merely modifying the prh_Setup.h file to
notice it's on a teensy platform using the __MK66FX1M0__ or __IMXRT1062__ defines.
I backed out the usage of $TFT_DEFINES in platform_setup.pm, so at this time it
works just as it used to, though it is somewhat cleaner and potentially more capable.


## Thoughts on a better Display library architecture.

I don't like the way these libraries generally work, using #defines to drive detailed
functionality.  It makes some of the code horrendously complicated.

I believe that all these libraries are missing a level of abstraction.

- **base class** - I think there should be a base class that generically knows how to
draw things, and that is independent of the *driver* or *platform*.
- **driver** - this layer knows how to communicate with various chips, thus far
only the ILI9341 or ILI9486 in my case.
- **platform** - this layer knows how to implement parallel or serial IO
to the chips.

The **platform** layers would be kept in separate subdirectories in the libraries
folder so that the Arduino ID would not try to compile them when using the more
generic base_clas or driver libraries.

The **platform** layers, would in turn provide the end user highest level defined
display classes that are actually instantiated in a program. IE:

- myLcdEsp32_9486Serial - for the existing CNC projects
- myLcdTeensy_9846Parallel - for the imaginary previous teensyExpression
- myLcdTeensy_9486Serial - for the new teensyExpression.


## The old teensyExpression is hosed

Although I have been valiantly trying to keep the old teensyExpression build
working, I am now in the situtation where it is impractical to continue using
the old box.

The existing motherboard footprint is too small for JST connectors, and
I'm **not** going to build another board for this project that uses *dupont*
connnectors.

One Idea is to just remake the back portion of the box, with the desired
footprints, even though it would not be orange and silver PLA.

Another Idea is to change the **base** of the machine to user *1/2" (11.5mm)
plywood*.  This would require redesigning and printing all the **button supports**
so that they screwed *down* onto the wood, while trying to keep all the *tops*
the same and having them screw *up* from the bottom through countersunk holes
in the wood. I would also need to provide **3d printed gaskets** that would
fit between the wood and the tops around the outside to make up for the missing
*lib* that is on the current 3D printed base.

*I am trying to keep the tops and buttons assembies for fear of running out of clear PLA,
and because they work*

**I MUST KEEP THE OLD HARNESS** - I'm effing NOT gonna make a new one.

The *final idea* is to redesign and reprint the whole damned thing, which
is what I will probably end up doing.

However, for now I am stuck in the midde of this big mess.


## First try with teensy 4.1

So the first thing I tried to do was just recompile the existing teensyExpression2
against the teensy 4.1 board that exists in my Arduino development environment.

Teensy Defines:

- **3.2** = __MK20DX256__
- **3.6** = __MK66FX1M0__
- **4.0** and **4.1** = __IMXRT1062__

### usb_desc.c and usb_desc_prh.h

Off the bat the first thing is that my low level override of the usb_desc.c from
the teensy libraries did not work.  I will have to later figure out how to make
the MIDI ports and device match what I want.

For now I just **moved usb_desc.c and h to the /data folder** so that they will
not be compiled.


### Modify myLCD for teensy 4.1 define

I previously branched a lot of the code based on the *__MK66FX1M0__* define,
and had a separate WITH_ILI9431_FONTS define which included the font stuff used
on the teensy.

I wrapped all of that up into a new **__LCD_TEENSY__** define which drives
both the low level device access code AND the includsion of the ILI9341_FONTS.

*ps. I made, and reverted, a change where I moved the **font type** definition
from ILI9341_t3.h into a separate H file called **ILI9341_font.h** in the
*ILI9341_fonts* library so that I could use the fonts without including
the device.  I *may* put that back in and/or move the fonts to myLcd
directly*

### my SdFat library did not support the teensy 4.1

This is where it really starts to get weird.  My fork of the SdFat library
was old and did not support the teensy 4.1.  So I saved off my local library,
deleted it from the libraries folder, and cloned the newest version of SdFat
which clearly does support the Teensy 4.1.

There were **major API changes** to the library!!  In fact it is a total
rewrite with all new identifier names.  I have yet to make heads or tails
out of it.  In **fileSystem.h** I made a stab at replacing the SdFatSdIO
class I was using with a new class that I think I need to use:

```
// update to SdFat v2
extern SdFat32 SD;
// extern SdFatSdio SD;
```

Then ran into problems in fileSystem.cpp because there is apparently
no longer a **dir_t** which has apparently been renamed to **diDirFat_t**
though I get compile errors not finding that identifier.

The last stupid change I made was to replace all instances of the
**File** type with **File32**.

Then, as I was reading online about using SdFat with the teensy,
I came across mentions that using an external SdFat library is no
longer recommended, and that it is built in to the later versions
of the **teensyDuino** (board) environment.

Sheesh, mother-fucker, digression city.


## prep for teensyDuino upgrade

That's why this file is called teensyDuino.md.

I am about to try to upgrade my teensyDuino platform.

For that I need to remember which **modules** I included
during the installation, or in otherwords, everything that
I want to ever use on a teensy again.

- the One Wire Ws2812b Demo project used the FastLed library
- I don't think teensyPi abd teensyLooper make use
  of any special teensyDuino things.

A review of old/obsolete projects

- Tumbller and old robot - I think for a moment I used a teensy
  on the tumbller project, and may have used one on the first
  robot I build, but Tumblerr is now defined to be ESP only,
  and I'll never build that old_robot again (it's not in my
  current source tree)
- teensySoundCard - is only used in circle
- USB sniffers - those no longer exist and I think they were Arduino
  based with external host ports.

So, apart from teensyLooper (and implicitly teensyPi), as far as I can tell,
the teensyExpression is the only thing I am really using teensyDuino for.
Nonetheless I will try to determine what modules I previously installed.
I know it's written down somewhere.

What I previously installed with teensyDuino (found in /zip/LenovoLedgionSetup.docm)

- uncheck all boxes
- check Audio
- check SerialFlash
- check WS2812Serial

I think I allow the Audio library because I *might* someday use it again
(I think I used it in early attempts at the looper, and clearly as the basis
for the AudioSystem portions of circle), so it will not hurt to have it installed.

I don't think I ever used the SerialFlash library which *"provides a file system
like approach to the flash memory"*.  I think I just use EEPROM.


## teensyDuino upgrade to version 1.58

I am currently using teensyDuinoInstall-1.53.exe from
C:\zip\_arduino\drivers_and_programs.

Now I just hope I can re-install teensyDuino without re-installing Arduino itself.

I am currently using Arduino 1.8.13
I am currently using teensyDuino 1.53


## download and run teensyDuinoInstall-1.58.exe

C:\zip\_arduino\drivers_and_programs.

There is no SdFat checkbox.

hmmmmm ... I notice a ILI9488_t3 library, and when I check
the 3.5 orange LCD's I bought actually use the 9488, so I will
check that too.  However, 4" uses the ST7796S which does not
have a teensyDuino library, but apparently has an Adafruit
library available for it.

- uncheck all boxes
- check Audio
- check ILI9488_t3
- check WS2812Serial

and see what happens ...
