# teensyExpression2 - a Consumer End User Product

I am making this project public at this time because it is an important
step in the devlopment of the
[**vGuitar rig**](https://github.com/phorton1/phorton1/).
The documentation here is a mess!

The sotftware herein has been tested, and is working on the original
[**teensyExpression**](https://github.com/phorton1/Arduino-teensyExpression)
hardware using a new motherboard PCB that is
included in that repository.

This repository is a major revision to the *teensyExpression*, a complete
rework, that provides most of the previous functionality, and a completely
different way of **configuring the pedal**, via **preference (rig) files**
that can be transferred to and from the device via a serial interface
and the
[Buddy](https://github.com/phorton1/base-apps-buddy) and
[fileClient](https://github.com/phorton1/base-apps-fileClient) applications,
or copied to the SDCard on the teensy in an any other manner.
This new way of configuring the pedal is intended to make it
completely generalized, and usable for any purpose, without the
need to re-compile or re-install the firmware, and yet, to be, by
default, completely configured for use within the gVuitar rig.

During this re-development effort, all new Fusion 360 3D designs for the
Pedal were produced, though never printed.  During that process a serious
review of the electronics involved in the entire vGuitar rig led to a
re-thinking of its entire architecture, and the role of the teensyExpression
pedal within that architecture, leading to the notion of a new, as yet
un-created, **teensyEpression3 repository** that will contain the streamlined
and well documented source code, 3D files, kicad circuits, and so on.

I am in the *proof of concept* stage with this next step in the
vGuitar rig journey.  For anyone following along, that next step
is the new
teensyPiAudio
repository that I am initializing today as
a public repository, which will contain the work that I am doing
on a day to day basis these days.

Please feel free to peruse this repository and/or see the new
work I am doing at


**Patrick** - April 6, 2024

**p.s. - in order to do this I am pushing a large amount of uncoordinated
and unstreamlined stuff to github, particularly a bunch of new components
and footprints for **kicad**. All that will need to be cleaned up too.



## OLD

Use the looper rig as an archetype of the types of rigs that can be programmed.

Most of the other functionality of the system will be maintained, including things like

- host port?
- ftp tuning and sensitvity
- midi monitoring at excruciating levels of detail
- serial port
- debugging output to serial port or USB
- midi monitoring to serial port or USB
- midi sequences sent to serial port or USB

Before going into excruciating detail, I want to see if I can specify a
language that will let me recreate my exist looper rig.


##  Rig Basics

There will be text files on the SD card that can contain *user defined rigs*.

An *example.rig* will be provided which just happens to generate the default
**iPadLooper** rig. I will have a way to that convert that to an H file containing
the necessary parsed representation for the default constant rig.

In my const version, the default rig will have the name of **iPadLooper** and will be
used on a *factory reset*. The *default rig name* of iPadLooper will be *reserved* for
use by the system.

All **rig names** shall be limited to the characters a-z, A-z, 0-9,
space, dash, and underscore.


## Load

*Load* shall present a list of legal rig names found on the SD card
(skipping the default rig name if happens to exist on disk),
with the default rig name at the top.

A rig loaded from disk will be checked for syntactical and
functional correctness.  A failure to Load the rig will not
change anything in the system (the current rig will remain
active).

Loading a rig will place it in memory and activate it.

It is not clear at this time if the current_rig name is
stored immediately in preferences, or if it will sommehow
be tied to the preferences system.

I suspect it will be stored immediately upon a load,
and not have anything to do with the Preferences.



## Boot

Upon booting the default rig will be set into memory.
If the current_rig preference is not the same as the
default rig name, an attempt will be made to load the
given rig from the SD card.

If the named rig is succesfully loaded, it will be placed
in memory and activated.   Otherwise, default rig will
be used.

The preference will not be changed as a result of a
failure to read a file that was previously successfully
saved, although an error message WILL be generated.



