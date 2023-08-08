# A language for describing Rigs

See TeensyExpression2.docx for a specification of the Language

Comments are anything after a pound sign (#)

The name of the rig is the filename without the .rig extension

Statements may occur in any order in a Text file.

They will be in a deterministic order in an output file.

Declared Strings are always CONSTANTS

Values are always VARIABLES and initialized to zero on the program start

Features that are not currently implemented:

- QuickMode
- songMachine
- Looper LOOP_IMMEDIATE and the LOOP_START commands



```

#--------------------------------------------
# Constants used in this program
#--------------------------------------------

# Synth Constants
#
# NUM_PATCH_ROWS = 3
# NUM_PATCH_COLS = 4
# NUM_BANK_PATCHES = 12
# NUM_PATCHES = 24
# NUM_BANKS = 2
# MULTI_OFFSET = 16
# FIRST_PATCH_SHORT_NAME_STRING_INDEX = 0
# FIRST_PATCH_LONG_NAME_STRING_INDEX = 24
# SYNTH_PROGRAM_CHANNEL = 7

# Guitar Effect Constants
#
# GUITAR_EFFECTS_CHANNEL 		= 9;
# GUITAR_DISTORTION_EFFECT_CC 	= 26
# GUITAR_WAH_EFFECT_CC 			= 27
# GUITAR_CHORUS_EFFECT_CC 		= 29
# GUITAR_ECHO_EFFECT_CC 		= 30

# Looper Constants
#
# TRACK_STATE_EMPTY               = 0  = 0x00;
# TRACK_STATE_RECORDING           = 1  = 0x01;
# TRACK_STATE_PLAYING             = 2  = 0x02;
# TRACK_STATE_STOPPED             = 4  = 0x04;
# TRACK_STATE_PENDING_RECORD      = 8  = 0x08;
# TRACK_STATE_PENDING_PLAY        = 16 = 0x10;
# TRACK_STATE_PENDING_STOP        = 32 = 0x20;
# TRACK_STATE_PENDING			  = 56 = 0x38  TRACK_STATE_PENDING_RECORD | TRACK_STATE_PENDING_PLAY | TRACK_STATE_PENDING_STOP;
#
# LOOP_COMMAND_NONE               = 0  = 0x00;
# LOOP_COMMAND_CLEAR_ALL          = 1  = 0x01;
# LOOP_COMMAND_STOP_IMMEDIATE     = 2  = 0x02;
# LOOP_COMMAND_STOP               = 3  = 0x003
# LOOP_COMMAND_DUB_MODE           = 4  = 0x04;     	# the dub mode is handled by rPi and modeled here
# LOOP_COMMAND_TRACK_BASE         = 16 = 0x10;     	# the four used "track" buttons are 0x10..0x13 (although the protocol can support upto 8 buttons 0..7)
#
# LOOP_STOP_CMD_STATE_CC 		  = 38  = 0x26;		# recv     send        the value is 0, LOOP_COMMAND_STOP or STOP_IMMEDIATE
# LOOP_DUB_STATE_CC      		  = 37  = 0x25;		# recv     send        value is currently only the DUB state
# LOOP_COMMAND_CC        		  = 36  = 0x24;		# send     recv        the value is the LOOP command
# TRACK_STATE_BASE_CC    		  = 20  = 0x14;		# recv     send        for 0..3 tracks, value is track state

#----------------------------------------
# String Constants
#----------------------------------------
# Synth String Constants

STRING(0, "BASS1"  );				# PATCH_SHORT_NAME{0}
STRING(1, "BASS-F" );				# PATCH_SHORT_NAME{1}
STRING(0, "BASS1-G" );				# PATCH_SHORT_NAME{2}
# and so on ...

STRING(24, "MM Bass Finger", ANY);	# PATCH_LONG_NAME{0}
STRING(25, "MM Bass Finger", ANY);	# PATCH_LONG_NAME{1}
STRING(26, "MM Bass Finger", ANY);	# PATCH_LONG_NAME{2}
# and so on ...

# Guitar String Constants (unused at this time)
#
# STRING(49,	"    ");			# BLANK GUITAR EFFECT NAME
# STRING(50,	"DIST");			# GUITAR EFFECT[0] NAME
# STRING(51,	"WAH ");			# GUITAR EFFECT[1] NAME
# STRING(52,	"CHOR");			# GUITAR EFFECT[2] NAME
# STRING(53,	"ECHO");			# GUITAR EFFECT[3] NAME


#----------------------------------------
# Variables
#----------------------------------------

# Synth Variables
#
# VALUE[1] = BANK number (0..1)
# VALUE[2] = PATCH_BUTTON number (0..3, 5..8, 10..13)
# VALUE[3] = ACTUAL_PATCH number (0..23) sent to Synth

# Guitar Effect Variables
#
# VALUE[11] = DIST_EFFECT_ON
# VALUE[12] = WAH_EFFECT_ON
# VALUE[13] = CHOR_EFFECT_ON
# VALUE[14] = ECHO_EFFECT_ON

# Looper Variables
#
# VALUE[20] = DUB_MODE
# VALUE[21] = STOP_BUTTON_COMMAND
# VALUE[31] = TRACK1 STATE
# VALUE[32] = TRACK2 STATE
# VALUE[33] = TRACK3 STATE
# VALUE[34] = TRACK4 STATE


#-------------------------------------
# Listen Statements
#-------------------------------------

LISTEN(1, 37, 20)
	# Listen on the Serial Midi Port (1) for
	# LOOP_DUB_STATE_CC=37 messages and stick
	# their values in the DUB_MODE VALUE[20]

LISTEN(1, 38, 21)
	# Listen on the Serial Midi Port (1) for
	# LOOP_STOP_CMD_STATE_CC=38 messages and stick
	# their values in the STOP_BUTTON_COMMAND VALUE[21]

# Listen on the Serial Midi Port (1) for
# TRACK_STATE_BASE_CC(20..23) messages
# and stick their values into the appropriate
# TRACK_STATE VALUES[31..34] slots

LISTEN(1, 20, 31)
LISTEN(1, 21, 32)
LISTEN(1, 22, 33)
LISTEN(1, 23, 34)



#=======================================================
# LEDs and Buttons
#========================================================

#-----------------------
# Synth Bank Button
#-----------------------
# If the BANK value is set, the LED is BLUE, otherwise its CYAN.
# A press of the button toggles the BANK value

LED(4):
	color: VALUE[1] ? LED_BLUE : LED_CYAN;
BUTTON(4):
    press: VALUE[1] = NOT VALUE[1];


#--------------------------------
# Synth Patch LEDs and Buttons
#--------------------------------
#
# IF the LED number matches the local PATCH_BUTTON VALUE[2] THEN
# 	IF the ACTUAL_PATCH VALUE[3] is larger than 11 THEN
#       LED color = BLUE
#	ELSE IF the ACTUAL_PATCH VALUE[3] is less than 12 THEN
#		LED color = CYAN
# 	ELSE
#		LED color = BLACK
#
# If the BUTTON number matches the local PATCH_BUTTON VALUE[2] THEN
#   the PATCH_BUTTON is set to the BUTTON number
#   the ACTUAL_PATCH VALUE[3] is set to the correct logical value
#   and the ACTUAL_PATCH VALUE[3] plus the MULTI_OFFSET constant
#   is sent via a ProgramChange message to the Midi Port (0),
#   on the SYNTH_PROGRAM_CHANNEL=7


LED(0):
	color: VALUE[2] == 0 ? VALUE[1] ? LED_BLUE : LED_CYAN : LED_BLACK;
BUTTON(0):
	press:
		VALUE[2] = 0;								# set PATCH_BUTTON value
		VALUE[3] = VALUE[1] * 12 + 0;				# set ACTUAL_PATCH number
		sendProgramChange(0, 7, 16 + VALUE[3]);		# send MULTI_OFFSET + ACTUAL_PATCH number
		display(AREA_UPPER_LEFT,  CYAN,   STRING[VALUE[3]]);				# show the short Patch Name
		display(AREA_SECOND_LINE, PURPLE, 24 + STRING[24 + VALUE[3]]);		# show the short Patch Name

LED(1):
	color: VALUE[2] == 1 ? VALUE[1] ? LED_BLUE : LED_CYAN : LED_BLACK;
BUTTON(1):
	press:
		VALUE[2] = 1;								# set PATCH_BUTTON value
		VALUE[3] = VALUE[1] * 12 + 1;				# set ACTUAL_PATCH number
		sendProgramChange(0, 7, 16 + VALUE[3]);		# send MULTI_OFFSET + ACTUAL_PATCH number
		display(AREA_UPPER_LEFT,  CYAN,   STRING[VALUE[3]]);				# show the short Patch Name
		display(AREA_SECOND_LINE, PURPLE, 24 + STRING[24 + VALUE[3]]);		# show the short Patch Name

LED(2):
	color: VALUE[2] == 2 ? VALUE[1] ? LED_BLUE : LED_CYAN : LED_BLACK;
BUTTON(2):
	press:
		VALUE[2] = 2;								# set PATCH_BUTTON value
		VALUE[3] = VALUE[1] * 12 + 2;				# set ACTUAL_PATCH number
		sendProgramChange(0, 7, 16 + VALUE[3]);		# send MULTI_OFFSET + ACTUAL_PATCH number
		display(AREA_UPPER_LEFT,  CYAN,   STRING[VALUE[3]]);				# show the short Patch Name
		display(AREA_SECOND_LINE, PURPLE, 24 + STRING[24 + VALUE[3]]);		# show the short Patch Name

LED(3):
	color: VALUE[2] == 3 ? VALUE[1] ? LED_BLUE : LED_CYAN : LED_BLACK;
BUTTON(3):
	press:
		VALUE[2] = 3;								# set PATCH_BUTTON value
		VALUE[3] = VALUE[1] * 12 + 3;				# set ACTUAL_PATCH number
		sendProgramChange(0, 7, 16 + VALUE[3]);		# send MULTI_OFFSET + ACTUAL_PATCH number
		display(AREA_UPPER_LEFT,  CYAN,   STRING[VALUE[3]]);				# show the short Patch Name
		display(AREA_SECOND_LINE, PURPLE, 24 + STRING[24 + VALUE[3]]);		# show the short Patch Name

# Second Row
# Note that henceforth there is a difference between the PATCH_BUTTON value
# and the ACTUAL_PATCH calculation

LED(5):
	color: VALUE[2] == 5 ? VALUE[1] ? LED_BLUE : LED_CYAN : LED_BLACK;
BUTTON(5):
	press:
		VALUE[2] = 5;								# set PATCH_BUTTON value
		VALUE[3] = VALUE[1] * 12 + 4;				# set ACTUAL_PATCH number
		sendProgramChange(0, 7, 16 + VALUE[3]);		# send MULTI_OFFSET + ACTUAL_PATCH number
		display(AREA_UPPER_LEFT,  CYAN,   STRING[VALUE[3]]);		# show the short Patch Name
		display(AREA_SECOND_LINE, PURPLE, STRING[24 + VALUE[3]]);	# show the short Patch Name

LED(6):
	color: VALUE[2] == 6 ? VALUE[1] ? LED_BLUE : LED_CYAN : LED_BLACK;
BUTTON(6):
	press:
		VALUE[2] = 6;								# set PATCH_BUTTON value
		VALUE[3] = VALUE[1] * 12 + 5;				# set ACTUAL_PATCH number
		sendProgramChange(0, 7, 16 + VALUE[3]);		# send MULTI_OFFSET + ACTUAL_PATCH number
		display(AREA_UPPER_LEFT,  CYAN,   STRING[VALUE[3]]);		# show the short Patch Name
		display(AREA_SECOND_LINE, PURPLE, STRING[24 + VALUE[3]]);	# show the short Patch Name

LED(7):
	color: VALUE[2] == 7 ? VALUE[1] ? LED_BLUE : LED_CYAN : LED_BLACK;
BUTTON(7):
	press:
		VALUE[2] = 7;								# set PATCH_BUTTON value
		VALUE[3] = VALUE[1] * 12 + 6;				# set ACTUAL_PATCH number
		sendProgramChange(0, 7, 16 + VALUE[3]);		# send MULTI_OFFSET + ACTUAL_PATCH number
		display(AREA_UPPER_LEFT,  CYAN,   STRING[VALUE[3]]);		# show the short Patch Name
		display(AREA_SECOND_LINE, PURPLE, STRING[24 + VALUE[3]]);	# show the short Patch Name

LED(8):
	color: VALUE[2] == 8 ? VALUE[1] ? LED_BLUE : LED_CYAN : LED_BLACK;
BUTTON(8):
	press:
		VALUE[2] = 8;								# set PATCH_BUTTON value
		VALUE[3] = VALUE[1] * 12 + 7;				# set ACTUAL_PATCH number
		sendProgramChange(0, 7, 16 + VALUE[3]);		# send MULTI_OFFSET + ACTUAL_PATCH number
		display(AREA_UPPER_LEFT,  CYAN,   STRING[VALUE[3]]);		# show the short Patch Name
		display(AREA_SECOND_LINE, PURPLE, STRING[24 + VALUE[3]]);	# show the short Patch Name

# Third Row

LED(10):
	color: VALUE[2] == 10 ? VALUE[1] ? LED_BLUE : LED_CYAN : LED_BLACK;
BUTTON(10):
	press:
		VALUE[2] = 10;								# set PATCH_BUTTON value
		VALUE[3] = VALUE[1] * 12 + 8;				# set ACTUAL_PATCH number
		sendProgramChange(0, 7, 16 + VALUE[3]);		# send MULTI_OFFSET + ACTUAL_PATCH number
		display(AREA_UPPER_LEFT,  CYAN,   STRING[VALUE[3]]);		# show the short Patch Name
		display(AREA_SECOND_LINE, PURPLE, STRING[24 + VALUE[3]]);	# show the short Patch Name

LED(11):
	color: VALUE[2] == 11 ? VALUE[1] ? LED_BLUE : LED_CYAN : LED_BLACK;
BUTTON(11:
	press:
		VALUE[2] = 11;								# set PATCH_BUTTON value
		VALUE[3] = VALUE[1] * 12 + 9;				# set ACTUAL_PATCH number
		sendProgramChange(0, 7, 16 + VALUE[3]);		# send MULTI_OFFSET + ACTUAL_PATCH number
		display(AREA_UPPER_LEFT,  CYAN,   STRING[VALUE[3]]);		# show the short Patch Name
		display(AREA_SECOND_LINE, PURPLE, STRING[24 + VALUE[3]]);	# show the short Patch Name

LED(12):
	color: VALUE[2] == 12 ? VALUE[1] ? LED_BLUE : LED_CYAN : LED_BLACK;
BUTTON(12):
	press:
		VALUE[2] = 12;								# set PATCH_BUTTON value
		VALUE[3] = VALUE[1] * 12 + 10;				# set ACTUAL_PATCH number
		sendProgramChange(0, 7, 16 + VALUE[3]);		# send MULTI_OFFSET + ACTUAL_PATCH number
		display(AREA_UPPER_LEFT,  CYAN,   STRING[VALUE[3]]);		# show the short Patch Name
		display(AREA_SECOND_LINE, PURPLE, STRING[24 + VALUE[3]]);	# show the short Patch Name

LED(13):
	color: VALUE[2] == 13 ? VALUE[1] ? LED_BLUE : LED_CYAN : LED_BLACK;
BUTTON(13):
	press:
		VALUE[2] = 13;								# set PATCH_BUTTON value
		VALUE[3] = VALUE[1] * 12 + 11;				# set ACTUAL_PATCH number
		sendProgramChange(0, 7, 16 + VALUE[3]);		# send MULTI_OFFSET + ACTUAL_PATCH number
		display(AREA_UPPER_LEFT,  CYAN,   STRING[VALUE[3]]);		# show the short Patch Name
		display(AREA_SECOND_LINE, PURPLE, STRING[24 + VALUE[3]]);	# show the short Patch Name

'
#----------------------------------
# Guitar Effect LEDs and Buttons
#----------------------------------
# The LEDs are GREEN if the EFFECT VALUE[11..14] are on.
# A short click of the BUTTONS toggle the EFFECT VALUEs
#	between 0 and 127, and sends the EFFECT_VALUE to
# 	the appropriate GUITAR_EFFECT_CC(26,27,29,30)
# 	on the Midi Port (0) and the GUITAR_EFFECTS_CHANNEL=9,
# A long press of the 4th button clears them all.

LED(15):											# DIST effect
	color: VALUE[11] ? LED_GREEN : LED_BLACK;		# GREEN if EFFECT_ON, BLACK if not
BUTTON(15):
	click:
		VALUE[11] = VALUE[11] ? 0 : 127;			# Toggle EFFECT_ON between 0 and 127
		sendCC(0, 9, 26, VALUE[11]);				# MidiPort, Channel, CC, value

LED(16):											# WAH effect
	color: VALUE[12] ? LED_GREEN : LED_BLACK;		# GREEN if EFFECT_ON, BLACK if not
BUTTON(16):
	click:
		VALUE[12] = VALUE[12] ? 0 : 127;			# Toggle EFFECT_ON between 0 and 127
		sendCC(0, 9, 27, VALUE[12]);				# MidiPort, Channel, CC, value

LED(17):											# CHOR effect
	color: VALUE[13] ? LED_GREEN : LED_BLACK;		# GREEN if EFFECT_ON, BLACK if not
BUTTON(17):
	click:
		VALUE[13] = VALUE[13] ? 0 : 127;			# Toggle EFFECT_ON between 0 and 127
		sendCC(0, 9, 29, VALUE[13]);				# MidiPort, Channel, CC, value

LED(18):											# ECHO effect
	color: VALUE[14] ? LED_GREEN : LED_BLACK;		# GREEN if EFFECT_ON, BLACK if not
BUTTON(18):
	click:
		VALUE[14] = VALUE[14] ? 0 : 127;			# Toggle EFFECT_ON between 0 and 127
		sendCC(0, 9, 30, VALUE[14]);				# MidiPort, Channel, CC, value
	long:
		VALUE[11] = 0;								# Clear all EFFCT_ON values
		VALUE[12] = 0;
		VALUE[13] = 0;
		VALUE[14] = 0;
		sendCC(0, 9, 26, 0);						# Send all EFFECT OFF messages
		sendCC(0, 9, 27, 0);
		sendCC(0, 9, 29, 0);
		sendCC(0, 9, 30, 0);


#---------------------------------------
# Looper Dub and Stop LEDs and Buttons
#----------------------------------------
# Note that the Looper LEDs are dependent on VALUES
# that are sent via Serial Midi FROM the rPi Looper,
# on the Serial Midi Port (1) via the LISTEN
# statements above.
#
# The buttons merely send certain CCs and values to the rPi
# Looper over the Serial Midi Port.

# The Dub LED is ORANGE if DUB_MODE VALUE[20] if set,
# 	or black otherwise.
# A click of the Dub Button sends a LOOP_COMMAND_CC=36
#   message to the Serial Midi Port (1) on Channel 1,
#   with the value of LOOP_COMMAND_DUB_MODE=4
# A long click of the Dub Button sends a LOOP_COMMAND_CC=36
#   message to the Serial Midi Port (1) with the value
#   of LOOP_COMMAND_CLEAR_ALL=1

LED(24):
	color: 	VALUE[20] ? LED_ORANGE : LED_WHITE;
BUTTON(24):
	click: sendCC(1, 1, 36, 4);
	long:  sendCC(1, 1, 36, 1);

# The Stop LED is
#	RED if the STOP_BUTTON_COMMAND VALUE[21] is LOOP_COMMAND_STOP_IMMEDIATE=2,
#   WHITE if the STOP_BUTTON_COMMAND VALUE[21] is LOOP_COMMAND_STOP=3, and
#   BLACK otherwise.
# A click of the Stop Button sends a LOOP_COMMAND_CC=36
#   message to the Serial Midi Port (1) on Channel 1
#   with the value of the the STOP_BUTTON_COMMAND VALUE[21]
# A long click of the Dub Button sends a LOOP_COMMAND_CC=36
#   message to the Serial Midi Port (1) ON Channel 1
#   with the value of LOOP_COMMAND_CLEAR_ALL=1

LED(19):
	color:
		VALUE[21] == 2 ? LED_RED :		#  STOP_BUTTON_COMMAND == LOOP_COMMAND_STOP_IMMEDIATE ? LED_RED :
		VALUE[21] == 3 ? LED_WHITE :	#  STOP_BUTTON_COMMAND == LOOP_COMMAND_STOP ? LED_WHITE :
		LED_BLACK;
BUTTON(19):
	click: sendCC(1, 1, 36, VALUE[21]);
	long:  sendCC(1, 1, 36, 1);


#------------------------------------------
# Looper Track Buttons
#------------------------------------------
# The looper track LEDs are set based on the
# 	TRACK1..4 STATE VALUES[31..34]
#   They LEDS are set to:
#		RED if RECORDING or PENDING_RECORD (9)
#   	YELLOW if PLAYING or PENDING_PLAY (18)
#       GREEN if STOPPED or PENDING_STOP (36)
#       and BLACK if empty (otherwise)
#   They are set to BLINK if they are pending (56)
# A press of a Track Button sends a LOOP_COMMAND_CC=36
#   message to the Serial Midi Port (1) on Channel 1
#   with the value of the LOOP_COMMAND_TRACK_BASE plus
#   the track number (0..3)

LED(20):
	color:
		VALUE[30] & 9  ? LED_RED :
		VALUE[30] & 18 ? LED_YELLOW :
		VALUE[30] & 36 ? LED_GREEN :
		LED_BLACK;
	blink:
		VALUE[30] & 56;
BUTTON(20):
	press: sendCC(1, 1, 36, 16)

LED(21):
	color:
		VALUE[31] & 9  ? LED_RED :
		VALUE[31] & 18 ? LED_YELLOW :
		VALUE[31] & 36 ? LED_GREEN :
		LED_BLACK;
	blink:
		VALUE[31] & 56;
		VALUE[30] & 56;
BUTTON(21):
	press: sendCC(1, 1, 36, 17)

LED(22):
	color:
		VALUE[32] & 9  ? LED_RED :
		VALUE[32] & 18 ? LED_YELLOW :
		VALUE[32] & 36 ? LED_GREEN :
		LED_BLACK;
	blink:
		VALUE[32] & 56;
BUTTON(22):
	press: sendCC(1, 1, 36, 18)

LED(23):
	color:
		VALUE[33] & 9  ? LED_RED :
		VALUE[33] & 18 ? LED_YELLOW :
		VALUE[33] & 36 ? LED_GREEN :
		LED_BLACK;
	blink:
		VALUE[33] & 56;
BUTTON(22):
	press: sendCC(1, 1, 36, 19)

# end of rigLooper text file

```

**end of rig_language.md**
