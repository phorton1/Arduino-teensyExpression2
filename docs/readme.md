# teensyExpression2 - a Consumer End User Product

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



