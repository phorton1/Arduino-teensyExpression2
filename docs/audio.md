# audio.md

Saga of the SGTL5000.

In the middle of designing/implementing TE3, after having
bailed on using a teensy 4 for the USB Audio and Midi device,
as well as a USB Host for the FTP, and already decided to go
back to the iRig2HD, and an external USB hub into which I will
also plug the FTP, I **thought** I had settled on using the
SGTL5000 teensyAudio shield with an rPi Zero 2W for the
Looper.  I had already extensivly not only tested the
quality of this SGTL5000, and found it sufficient, but had
also completely reworked the API to support serial Midi.

Blowing off both the TE3 and TE3_hub repositories, I went
back to this teensyExpression2 for kicad stuff and added the
esp32_PiLooper repo, where I painstakingly implemented an
ESP32 based replacement for the teensyPiLooper, because the
ESP32 *might* allow me to have a webUI and/or use telnet
to work with the foot pedal instead of requiring a usb
connection for configuring (uploading rig files to the pedal)
and programming (uploading Loooper kernel7.img files to the
rPi), That is working "so-so" compared to the previous teensy
which was more or less bullet proof.  But it works.

## SGTL5000 (teensy RevD board) initial try

So then I finally got to try hooking the SGTL5000 up to the
rPi.  Forgot that the teensyAudio shield has no MCLK, and that
the rPi has to provide it to get anything to work.  The
01-StereoPassThru example referenced my audio system components
that I removed ages ago.  First was a reference to a no longer
existing AudioControlSGTL5000master object that no longer exists.
Second were references to AudioInputI2Sslave and AudioOutputI2Sslave
objects that no longer exist.

Have pretty conclusively determined that when I deleted the
AudioControlSGTL5000master object that, in fact, I had renamed
it to the 'main' AudioControlSGTL5000 object and actually
deleted the previous 'main' SGTL5000 object.  These names
are misleading and hard to understand.  The connotation of
'master' arose because the SGTL5000 was providing the BCLK
and FCLK signals to the bcm_pcm, which is all it really cares
about.  So, from the audio system's point of view it was the
'master' of the bcm_pcm.  However, the rPi had to provide the
MCLK to the SGTL5000, which in truth, made it the ultimate
master, the sgtl an intermediate master, and the bcm_remained
a 'slave'.

It looks like the enable() method for the prh/audio system
"normal" AudioControlSGTL5000 object was just a raw, unchanged
copy of Pauls enable() method for the teensy, in which the
teensy IS indeed the MCLK master (like the rPi), and it seems,
ALSO the "i2s" master for the BCLK and FCLK signals (from the
built int teensy i2s device).

In the last existing copy of my old 'master; sgtl5000 device,
apart from starting the rPi 11.89 Mhz MCLK for the sgtl out
to rPi GPIO4, the only other change was that I added the
0x80 bit to the sgtl CHIP_I2S_CTRL register to make the
SGTL the producer of the "Master LRCLK and SCLK" (FCLK & BCLK).
This bit was also set in a vestigal function of Paul's sgtl5000
with the extMCLK parameter to support an early teensy LC mpu,
but in his version he used the PLL (phase locked loop) on the
SGTL to get the MCLK for use by the sgtl5000 from whatever
he sent to the SGTL5000.  On the rPi I was not using the PLL
because I generate a 11.89 mHz clock which is an exact 256*fs
for the the MCLK.

In any case subsequent testing did not go well. I started
wit a brand new RevD (definitely NOT RevD2) board that had
tested so well in the TE3_hub experiments.  Nada.
The bcm_pcm got maybe a few interrupts, but was definitely
not working.  After double checking my i2s RX and TX indicator
leds were working, and also on the rPi screen with the recently
implemented AudioMonitor, I could see that nothing good was
happening.


## Measure the clocks on the RevD board

So with my dismal old broken toy oscilloscope, which barely
works, I started to probe the clocks.  The scope power button
is worn out and it reboots if you look at it. It seems to
not worked when plugged into the laptop USB power supply,
but does when plugged into an external one.  The auto
function freezes and hangs the thing,  Because of the
lack of auto I have to diddle the scope frequency and
voltage ranges and scales to get to see anything interesting.

Nonetheless, I was not super pleased with what I saw,
esp to the degree that other tests (with the wm8731)
show very clear, and pretty square, clocks on the scope.

Starting with the RevD board I got a round wave 1.3v to 1.65v p-p
at 11.2 Mhz for the MCLK from the rPi on the 125ns (fastest)
or 250ns scale on the scope.  The exact frequency
should be 11289600 for 44.1khz, so that's okish, but the
p-p voltage level is dismal.

Exact desired frequencies

- FCLK: 44100 (44.1khz) - use 10us scale on scope
- BCLK: 64*44.1khz = 2822400 (2.82mHz) - use 500-120ns scale on scope
- MCLK: 256*44.1khz = 11289600 (11.28mhz) - use 250-125na (fastest) scale on scope

Then I tried to measure the BCLK and FCLK signals from the SGTL5000.
At times I got a a barely perceptible 0.5v p-p at 2.8 Mhzfor BCLK,
but usually nothing. Once I got a nice, squarish 2.90v p-p at 44.1 kHz
for FCLK but only once. Could not get it back.  And like I said, nothing
was happening in the bcm_pcm.

I even went so far as to try to modify sgtl500::enable()
to output more power to the BCLK/FCLK pads by poking the
CHIP_PAD_STRENGTH register, but it did not seem to help.

## Switch to RevB board

So I switched to the teensy RevB board, which worked better, but not
well enough to drive the bcm_pcm.  With the RevB board I got a pretty
nice square 3.5v p-p at 44.1 kHz for FCLK, and a lesser, but stable
1.85v p-p at 2.81 mHz for BLCK.

The bcm_pcm would run, but apparently only sporadically, as instead
of seeing 300+ interrupts per second, I was seeing more like 5-10.
Slow enough so that after 10-20 seconds it finally triggred my
I2S RX/TX indicator leds, which are supposed to flash about 3-4
times a second.  I can see on the rPi screen that there are interrupts
happening, but they are not happening often enough.  It then stops
working completely after about 500 interrupts.

In otherwords BLCK and/or FCLK are not doing the trick.



## Try wm8731 (AudioInjector Stereo sound card)

So in utter frustration tore off the SGTL5000, dug up the, my only,
AudioInjector stereo sound card, stacked it obn top of the rPi Zero 2W,
put the breakout board, with the ILI9488 connectors on top of that,
recompiled the example program for the wm8731 and fired it up.

The ILI9488 doesn't work.  Probably the CD pin, or some other pin
being used by the AudioInjector, maybe i2c pullups.

But the i2S indicator leds flash nicely, the screen AudioMonitor shows
a busy, apparently happy, bcm_pcm, and the clock signals are clear as
a bell on the junk scope, although they are at the wrong (unexpected)
frequencies.  The wm8731 has been running for hours as I typed this.

The measured frequencies, and p-p voltages, for the wm8731, which
is acting as the i2S master to the bcm_pcm, are as follows:

- FCLK 46.8khz 3.45-3.85v p-p - square with noise
- BCLK 3.00mHz 4.2v p-p (?!?) - squarish


## What to do?

Do I:

- Hook up audio to the wm8731 and see how it sounds and if it
  can be made "good enough" for use in TE3?
- Try to figure out why the RevB worked better than the RevD?
- Try to get some other clock for the SGTL5000?

As I said in the opening, I was really planning on using the
SGTL5000 cuz I've put a ton of work into it, testing the quality,
tuning it to the iRig and PA, making the whole midi API, and
so on.  It's also signficanly smaller than the injector,
which is almost as big as the octo, and another main design
idea behind TE3 is to shrink the electronics footprint and
complexity.

Grmph.  It being monday at 4, 3 days before I have to go to the
USA to get my driver's license renewed, a bunch of stuff backed
up at Island Express, grrr, the silly "electronics class" I'm
not teaching very well, and I fucking forgot to do my laundry
today which means it HAS to be done tomorrow and I HAVE to get
it on Thu morning if I want clean underwear for the trip.

It being monday at 4, I'm gonna have a shot of rum and a beer.




