# A language for describing Rigs


There will be a text file format and a parser.

There will be user defined variables to implement things like
the 'bank' button and 'quick mode' as exist in my current LooperRig.

The pedal functionality and curve configuration will be global.
The names and CC's will be defined by the rig.

The interface to the song machine will have to change radically,
due to the binding of funcitonality (i.e. PATCH_BASS or GUITAR_DIST_ON)
to specific buttons within a given rig.   Songs will likely not
be portable between rigs for this reason and working backwards
from the rig to the song machine will be arduous.

I know that a long press of THE_SYSTEM_BUTTON will be reserved
to go into "configuration mode" and the idea that the 'quick mode'
button will somehow be described in the text file, but there
are other buttons that are somewhat system modal:

The song machine button (below THE_SYSTEM_BUTTON) requires
a long press to enter the mode. Once in the mode, a long
press of the bottom right button exits the mode, and a
short press restarts the song. Currently a long press of
THE_SONG_MACHINE_BUTTON brings up the song selection dialog,
but a short press, while a song is active, incorrectly acts
like a song_machine_button, and advances the song.
- the FTP Tuner button only works if there is an FTP
present and is currently the only function of that button.


## FIRST TRY

- long press of THE_CONFIG_BUTTON is reserved
- long press of THE_SONG_MACHINE_BUTTON is reserved
- long press of THE_FTP_TUNER_BUTTON is reserved
- Starting without QuickMode or the SongMachine

Buttons are numbered from 0..24.

SampleTank is setup on the iPad to receive on Patch changes on Channel 7,
with multi patch numbers 16 .. 39, and Volume changes on Channel 1 with
CC #7.

ToneStack is setup on the iPad to receive effect CC's on Channel 9,
with the below guitar effect CC's, and to receive Volume changes on Channel 7,
with the CC #7.

The indentation in the file is only for human readability.
Comments are things following #.

```

CONSTANTS:

	# Synth - SampleTank on iPad

	SYNTH_VOLUME_CHANNEL  = 1;
	SYNTH_VOLUME_CC	= 7;

	SYNTH_PROGRAM_CHANNEL = 7;
	MULTI_OFFSET = 16;

	NUM_PATCH_ROWS = 3;
	NUM_PATCH_COLS = 4;
	NUM_BANK_PATCHES = 12;

	PATCH_SHORT_NAME = [
		"BASS1",
		"BASS-F",
		"BASS-G", ... ];

	PATCH_LONG_NAME = [
		"MM Bass Finger",
		"MM Bass Finger",
		"MM Bass Finger"  ...];


	# Guitar Effects
	# ToneStack on iPad

	NUM_GUITAR_EFFECTS = 4;
	GUITAR_EFFECTS_CHANNEL = 9;

	GUITAR_WAH_CONTROL_CC 				= 11;
	GUITAR_REVERB_DEPTH_CC              = 20;		# not used
	GUITAR_DISTORTION_EFFECT_CC         = 26;
	GUITAR_WAH_EFFECT_CC                = 27;		# sent by Pedal
	GUITAR_FLANGER_EFFECT_CC            = 28;		# not used
	GUITAR_CHORUS_EFFECT_CC             = 29;
	GUITAR_ECHO_EFFECT_CC               = 30;

	GUITAR_VOLUME_CHANNEL               = 7;
	GUITAR_VOLUME_CC                    = 7;

	GUITAR_EFFECT_NAME = [
		"DIST",
		"WAH ",
		"CHOR",
		"ECHO" ];

	GUITAR_CCS = [
		GUITAR_DIST_CC,
		GUITAR_WAH__CC,
		GUITAR_CHOR_CC,
		GUITAR_ECHO_CC  ];

	# Looper - denormalized from commonDefines.h

	LOOPER_NUM_TRACKS = 4;
	LOOPER_NUM_LAYERS = 4;

	TRACK_STATE_EMPTY               = 0x0000;
	TRACK_STATE_RECORDING           = 0x0001;
	TRACK_STATE_PLAYING             = 0x0002;
	TRACK_STATE_STOPPED             = 0x0004;
	TRACK_STATE_PENDING_RECORD      = 0x0008;
	TRACK_STATE_PENDING_PLAY        = 0x0010;
	TRACK_STATE_PENDING_STOP        = 0x0020;
	TRACK_STATE_PENDING			    = TRACK_STATE_PENDING_RECORD | TRACK_STATE_PENDING_PLAY | TRACK_STATE_PENDING_STOP;

	LOOP_COMMAND_NONE               = 0x00;
	LOOP_COMMAND_CLEAR_ALL          = 0x01;
	# The following two are GIVEN to us by the looper in the LOOP_STOP_CMD_STATE message,
	# but never sent by constant from TE
	# LOOP_COMMAND_STOP_IMMEDIATE   = 0x02;     # stop the looper immediately
	# LOOP_COMMAND_STOP             = 0x03;     # stop at next cycle point
	LOOP_COMMAND_DUB_MODE           = 0x04;     # the dub mode is handled by rPi and modeled here
	# The following is currently presented only in QuickMode
	# LOOP_COMMAND_ABORT_RECORDING  = 0x06;     # abort the current recording if any
	# The folowing is the special 'loop immediate' functionality attached to teh
	LOOP_COMMAND_LOOP_IMMEDIATE     = 0x08;     # immediatly loop back to all clip starts ...
	LOOP_COMMAND_SET_LOOP_START     = 0x09;     # immediatly set the "restart point" for the clips in the track
	LOOP_COMMAND_CLEAR_LOOP_START   = 0x0A;     # immediatly set the "restart point" for the clips in the track
	LOOP_COMMAND_TRACK_BASE         = 0x10;     # the four used "track" buttons are 0x10..0x13 (although the protocol can support upto 8 buttons 0..7)
	# The following is currently presented only in Quick Mode
	# LOOP_COMMAND_ERASE_TRACK_BASE = 0x20;     # erase the given track (stops it if playing)
	# The following are for internal use only by the Looper
	# LOOP_COMMAND_RECORD           = 0x40;
	# LOOP_COMMAND_PLAY             = 0x50;


	# Looper Serial CC numbers             		# TE       rPi         descrip

	LOOP_CONTROL_BASE_CC   			= 0x65;   	# send     recv        for 0..LOOPER_NUM_CONTROLS the value is the volume control (Looper pedal == 0x67)
	LOOP_STOP_CMD_STATE_CC 			= 0x26;		# recv     send        the value is 0, LOOP_COMMAND_STOP or STOP_IMMEDIATE
	LOOP_DUB_STATE_CC      			= 0x25;		# recv     send        value is currently only the DUB state
	LOOP_COMMAND_CC        			= 0x24;		# send     recv        the value is the LOOP command
	TRACK_STATE_BASE_CC    			= 0x14;		# recv     send        for 0..3 tracks, value is track state

	# The following are only used in Quick Mode to mute or change clip levels
	# CLIP_VOL_BASE_CC       		= 0x30;		# send     recv        for 4 tracks * 3 clips - value is volume 0..127
	# CLIP_MUTE_BASE_CC      		= 0x50;		# both     both        for 4 tracks * 3 clips - value is mute state
	# The following is only used to notify the SongMachine about loop points
	# NOTIFY_LOOP            		= 0x64;     # recv     send        value=number of pending loop notifies

	# Looper Volume Control CC offsets from LOOP_CONTROL_BASE_CC
	# Four are represented as Rotary Controls

	RPI_CONTROL_INPUT_GAIN          = 0;
	RPI_CONTROL_THRU_VOLUME         = 1;
	RPI_CONTROL_LOOP_VOLUME         = 2;		# sent out by Pedal
	RPI_CONTROL_MIX_VOLUME          = 3;
	RPI_CONTROL_OUTPUT_GAIN         = 4;


# These are state variables that may change the
# colors of the buttons and/or the behavior of presses

VARIABLES:
	bank = 0;
	patch_num = 0;
	patch_row = 0;
	patch_col = 0;

	effect_on = [0,0,0,0];

	track_state = [0,0,0,0];
	dub_mode = 0;
	stop_button_cmd = 0;
	clip_mute = [0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0];


# System Buttons - there is only one here as we are
# currently not including hooks to FTP Tuner, Song Machine,
# or the Loop Immediate stuff

BUTTON(4) :
	color: bank ? LED_BLUE : LED_CYAN;
	short: bank = !bank;


# Patch Button Grid

BUTTON_GRID(NUM_PATCH_ROWS, NUM_PATCH_COLS, [0,1,2,3, 5,6,7,8, 10,11,12,13]) :
	color:
		patch_row == GRID_COL && patch_col == GRID_ROW ? bank == 0 ? LED_CYAN : LED_BLUE : LED_BLACK;
	short:
		patch_row = GRID_ROW;
		patch_col = GRID_COL;
		patch_num = bank * NUM_BANK_PATCHES + patch_row * NUM_PATCH_COLS + patch_col;
		display(SHORT_PATCH_NAME_AREA, DISPLAY_COLOR_CYAN, PATCH_SHORT_NAME[patch_num]);
		display(LONG_PATCH_NAME_AREA, DISPLAY_COLOR_PURPLE, PATCH_LONG_NAME[patch_num]);
		sendMidiProgramChange( MULTI_OFFSET + patch_num, SYNTH_PROGRAM_CHANNEL );


# Guitar Button Grid

BUTTON_GRID(1, NUM_GUITAR_EFFECTS, [15,16,17,18]) :
	color:
		effect_on[GRID_COL] ? GREEN : BLACK;
	short:
		effect_on[GRID_COL] = !effect_on[GRID_COL];
		sendMidiCC(
			GUITAR_CCS[GRID_COL],
			effect_on[GRID_COL] ? 0x7f : 0,
			GUITAR_EFFECTS_CHANNEL);
	long:
		if (GRID_COL == 3)
		{
			effect_on[0] = 0;
			effect_on[1] = 0;
			effect_on[2] = 0;
			effect_on[3] = 0;
			sendMidiCC(GUITAR_DIST_CC,0,GUITAR_EFFECTS_CHANNEL);
			sendMidiCC(GUITAR_WAH__CC,0,GUITAR_EFFECTS_CHANNEL);
			sendMidiCC(GUITAR_CHOR_CC,0,GUITAR_EFFECTS_CHANNEL);
			sendMidiCC(GUITAR_ECHO_CC,0,GUITAR_EFFECTS_CHANNEL);
		}




// Looper Behavior and Grid
//
// The Looper *should* notify us about any and all state changes.
// This includes aggregated individual state message when things like
// LOOP_COMMAND_CLEAR_ALL and LOOP_COMMAND_STOP_IMMEDIATE are executed

ON_SERIAL_MIDI:	# gets internal variables: cc_num and cc_value

	if (cc_num >= TRACK_STATE_BASE_CC && cc_num < TRACK_STATE_BASE_CC + LOOPER_NUM_TRACKS)
		track_state[cc_num - TRACK_STATE_BASE_CC] = cc_value;
	else if (cc_num == LOOP_DUB_STATE_CC)
		dub_mode = cc_value;
	else if (cc_num == LOOP_STOP_CMD_STATE_CC)
		stop_button_cmd = cc_value;
	else if (cc_num >= CLIP_MUTE_BASE_CC && cc_num < CLIP_MUTE_BASE_CC + LOOPER_NUM_TRACKS * LOOPER_NUM_LAYERS)
		clip_mute[cc_num - CLIP_MUTE_BASE_CC] = cc_value;

	// probably should be another case for volume here
	// although we only SEND that, and only in quick mode

	// And this is a weirdness.
	// The song machine is intimately linked to the looper
	//
	//	cc_num == NOTIFY_LOOP:
	//	theSongMachine->notifyLoop();




BUTTON_GRID(1, LOOPER_NUM_TRACKS, [20, 21, 22, 23])
	color :
		track_state[GRID_COL] & (TRACK_STATE_RECORDING | TRACK_STATE_PENDING_RECORD) ? RED :
		track_state[GRID_COL] & (TRACK_STATE_PLAYING | TRACK_STATE_PENDING_PLAY) ? YELLOW :
		track_state[GRID_COL] & (TRACK_STATE_STOPPED | TRACK_STATE_PENDING_STOP) ? GREEN :
		BLACK
	flash :
		track_state[GRID_COL] & TRACK_STATE_PENDING ? 1 : 0;
	short:
		sendSerialCC(LOOP_COMMAND_CC,LOOP_COMMAND_TRACK_BASE + GRID_COL);



// Dub mode really makes no sense when on an empty looper
// but it is allowed.

BUTTON(24) 		// Looper Dub Button
	color :
		dub_mode ? ORANGE : BLACK
	short :
		sendSerialCC(LOOP_DUB_STATE_CC,LOOP_COMMAND_DUB_MODE);
	long :
		sendSerialCC(LOOP_COMMAND_CC,LOOP_COMMAND_CLEAR_ALL)


BUTTON(19) 		// Looper Stop Button
	color :
		stop_button_cmd == LOOP_COMMAND_STOP_IMMEDIATE ? RED :
		stop_button_cmd == LOOP_COMMAND_STOP ? WHITE
		black;
	short :
		if (stop_button_cmd)
			sendSerialCC(LOOP_COMMAND_CC,stop_button_cmd);
	long :
		sendSerialCC(LOOP_COMMAND_CC,LOOP_COMMAND_CLEAR_ALL)


// The rotary controls have no names as far as the TE is concerned.
// The controls send out serial CC messages to the looper

ON_ROTARY_EVENT		# gets internal variables: rotary_num and rotary_value
	rotary_num = 0 :
		sendSerialCC(RPI_CONTROL_NUM_CC_OFFSET + RPI_CONTROL_INPUT_GAIN, value)
	rotary_num = 1 :
		sendSerialCC(RPI_CONTROL_NUM_CC_OFFSET + RPI_CONTROL_OUTPUT_GAIN, value)
	rotary_num = 2 :
		sendSerialCC(RPI_CONTROL_NUM_CC_OFFSET + RPI_CONTROL_THRU_VOLUME, value)
	rotary_num = 3 :
		sendSerialCC(RPI_CONTROL_NUM_CC_OFFSET + RPI_CONTROL_MIX_VOLUME, value)


// There is probably some subtle behavior in the Pedals having to
// do with pacing the messages sent out.  Here we merely specify the
// CC's and

PEDAL(0)		# implicitly uses internal pedal_value
	name: "Synth"
	midi_channel: SYNTH_VOLUME_CHANNEL
	midi_cc: SYNTH_VOLUME_CC

PEDAL(1)
	name: "Loop"
	serial_cc: LOOP_CONTROL_BASE_CC + RPI_CONTROL_LOOP_VOLUME

PEDAL(2)
	name: "Wah"
	midi_channel: GUITAR_EFFECTS_CHANNEL
	midi_cc: GUITAR_WAH_CONTROL_CC

PEDAL(3)
	name: "Guitar"
	midi_channel: GUITAR_VOLUME_CHANNEL
	midi_cc: GUITAR_VOLUME_CC
```


Note that the "old rig" which used Quantiloop instead of the looper pedal
has changes to the PEDAL and generally to the whole behavior of the loop buttons

I can see how I would change the language into a state machine.
Not so easy is to see how to change that state machine back into a language text file,
especially given the use of constants and user variables.

I think they can only be *written* with a text editor and placed on the SD,
where they can be loaded (and verified) and tested.  We would provide
one called "example" which maps to the LooperRig, which I would have a
special way of turning into constants to be stored in an H file or something.
