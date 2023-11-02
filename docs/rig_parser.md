# Rig Parser

- The file will be token oriented.
- The comment character will be #
- After consmments and blank lines are removed, it will be tokenized
- There will probably need to be a maximum program size and maximum variable usage.
- while parsing the program all strings outside of quotes are lowercased

The actual tokens are in lower case.
Case cannot be used to distinguish user defined constant or variable identifiers.


## A. Tokens

### Reserved Words

- CONSTANTS
- VARIABLES
- GRID_ROW is a variable number available in BUTTON_GRID sections
- GRID_COL is a variable number available in BUTTON_GRID sections
- cc_num is a variable number available in ON_SERIAL_MIDI sections
- cc_value is a variable number available in ON_SERIAL_MIDI sections
- rotary_num is a variable number available in ON_ROTARY sections
- rotary_value is a variable number available in ON_ROTARY sections
- IF
- THEN
- ELSE
- sendMidiTokenChange
- sendMidiCC
- sendSerialCC
- display
- SHORT_PATCH_NAME_AREA
- LONG_PATCH_NAME_AREA
- BUTTON
- BUTTON_GRID
- color
- flash
- short
- long
- ON_SERIAL_MIDI
- ON_ROTARY_EVENT
- PEDAL
- name
- midi_channel
- midi_cc
- serial_cc


### Symbols and Delimiters

- OPEN_PAREN
- CLOSE_PAREN
- LEFT_BRACKET
- RIGHT_BRACKET
- LEFT_PAREN
- RIGHT_PAREN
- LEFT_SQUIGGLE
- RIGHT_SQUIGGLE
- EQUALS
- COMMA
- COLON
- SEMI_COLON
- PLUS
- MINUS
- DIVIDED_BY
- TIMES
- QUESTION_MARK
- LOGICAL_OR (vertical bar)
- LOGICAL_AND (ampersand)
- BOOLEAN_OR (two vertical bars)
- BOOLEAN_AND (two ampersands bars)
- EQUALITY (two equals signs)

### LED_COLOR constants

These are enumerated values that correspond to the 24 bit LED colors defined in myLeds.h,
and are also *reserved words*.

- LED_BLUE
- LED_CYAN
- etc


### DISPLAY_COLOR constants

These are enumerated values that correspond to the 16 bit TFT colors defined in myTFT.h,
and are also *reserved words*.

- DISPLAY_BLUE
- DISPLAY_CYAN
- etc


### DISPLAY_AREA constants

The font sizes and justification are determined by the areas.
The opcodes will ensure that strings that are too long are not sent.
These were already noted to be *reserved words* above.

- SHORT_PATCH_NAME_AREA
- LONG_PATCH_NAME_AREA


### Numbers

NUMBER is a constant number that is either decimal or hexedecimal
as given by the following two RE's

- **^\[0-9]+$** is a decimal number
- **^0x\[0-9A-F]+$** is a hexedecimal number


### Strings

STRING is a constant quote delimited string.
They will probably need to be checked for appropriate lengths in usage.

- " any characters "

### Identifiers

Any other word matching ^\[a-z]1\[a-z0-9_]$ is an **identifier**.
It is illegal to define the same identifier more than once.

- IDENTIFIER

IDENTIFIER is a transient token type.  The actual type of a
identifier is determined by the context, and resolves to
one of the following.

- CONSTANT_ID
- CONSTANT_STRING_ID
- CONSTANT_ARRAYED_ID
- CONSTANT_ARRAYED_STRING_ID
- VARIABLE_ID
- VARIABLE_STRING_ID
- VARIABLE_ARRAYED_ID
- VARIABLE_ARRAYED_STRING_ID



## B. Overall Syntax and Structure

Overall the program is made of a few *section* types.
The sections can occur in any order with the following notes:

- constants must be defined before they are used
- variables must be defined before they are used
- a button may not appear in more than one BUTTON or GRID_BUTTON section

Certain internal variables may be used in certain sections.

- the internal GRID_ROW and GRID_COL variables are only available in BUTTON_GRID sections
- the internal cc_num and cc_value variables are only available in ON_SERIAL_MIDI sections
- the internal rotary_num and rotary_value variabls are only available in ON_ROTARY_EVENT sections

### Overall Structure

rig_definition = rig_section \[ rig_section ]\*

rig_section =
- CONSTANTS_section
- VARIABLES_section
- BUTTON_section
- BUTTON_GRID_section
- ON_SERIAL_MIDI_section
- ON_ROTARY_EVENT_section
- PEDAL_section



## C. Constants and Constant Expressions


CONSTANTS_section = CONSTANTS COLON constant_decl \[ constant_decl ]\*

constant_decl =
- CONSTANT_ID EQUALS constant_expression SEMI_COLON
- CONSTANT_STRING_ID EQUALS constant_string_expression SEMI_COLON
- CONSTANT_ARRAYED_ID EQUALS LEFT_BRACKET contant_expression \[ COMMA constant_expression ]\* RIGHT_BRACKET SEMI_COLON
- CONSTANT_ARRAAYED_STRING_ID EQUALS LEFT_BRACKET constant_string_expression \[ COMMA constant_string_expression ]\* RIGHT_BRACKET SEMI_COLON

constant_string_expression =
- STRING
- CONSTANT_STRING_ID
- CONSTANT_ARRAYED_STRING_ID LEFT_BRACKET constant_expression RIGHT_BRACKET
- LEFT_PAREN constant_string_expression RIGHT_PAREN

constant_expression =
- NUMBER
- CONSTANT_ID
- CONSTANT_ARRAYED_ID LEFT_BRACKET constant_expression RIGHT_BRACKET
- LEFT_PAREN constant_expression RIGHT_PAREN
- constant_expression PLUS constant_expression
- constant_expression MINUS constant_expression
- constant_expression TIMES constant_expression
- constant_expression DIVIDED_BY constant_expression
- constant_expression LOGICAL_OR constant_expression
- constant_expression LOGICAL_AND constant_expression
- constant_expression BOOLEAN_AND constant_expression
- constant_expression BOOLEAN_OR constant_expression

button_num = constant_exprssion
- where the value is limited to 0..23

grid_size = constant_expression
- where the value is limited to 1..5

pedal_num = constant_expression
- where the value is limited to 0..3


## D. Variables and Variable Expressions

VARIABLES_section = VARIABLES var_decl \[ var_decl ]\*

var_decl =
- VARIABLE_ID EQUALS constant_expression SEMI_COLON
- VARIABLE_STRING_ID EQUALS constant_string_expression SEMI_COLON
- VARIABLE_ARRAYED_ID EQUALS LEFT_BRACKET constant_expression \[ COMMA constant_expression ]\* RIGHT_BRACKET SEMI_COLON
- VARIABLE_ARRAAYED_STRING_ID EQUALS LEFT_BRACKET constant_string_expression \[ COMMA constant_string_expression ]\* RIGHT_BRACKET SEMI_COLON

var_string_expression =
- constant_string_expression
- VARIABLE_STRING_ID
- VARIABLE_ARRAYED_STRING_ID LEFT_BRACKET variable_expression RIGHT_BRACKET
- var_expression QUESTION_MARK var_string_expression COLON var_string_expression
- LEFT_PAREN var_string_expression RIGHT_PAREN

var_expression =
- constant_expression
- GRID_ROW
- GRID_COL
- cc_num
- cc_value
- rotary_num
- rotary_value
- VARIABLE_ID
- VARIABLE_ARRAYED_ID LEFT_BRACKET variable_expression RIGHT_BRACKET
- LEFT_PAREN var_expression RIGHT_PAREN
- var_expression PLUS var_expression
- var_expression MINUS var_expression
- var_expression TIMES var_expression
- var_expression DIVIDED_BY var_expression
- var_expression LOGICAL_OR var_expression
- var_expression LOGICAL_AND var_expression
- var_expression BOOLEAN_OR var_expression
- var_expression BOOLEAN_AND var_expression
- var_epression QUESTION_MARK var_expression COLON var_expression


### Color Expressions

Note that the terminal nodes are always color-constant tokens

led_color_expression =
- LED_COLOR
- LEFT_PAREN led_color_expression RIGHT_PAREN
- var_epression QUESTION_MARK led_color_expression COLON led_color_expression

display_color_expression =
- DISPLAY_COLOR
- LEFT_PAREN display_color_expression RIGHT_PAREN
- var_epression QUESTION_MARK display_color_expression COLON display_color_expression


## E. Statements

statement_list = statement \[ statement ]\*

statement =
- var_assignment_statement SEMI_COLON
- if_statement SEMI_COLON
- sendMidiProgramChange_statment SEMI_COLON
- sendMidiCC_statement SEMI_COLON
- sendSerialCC_statement SEMI_COLON
- sendSerialCC SEMI_COLON
- LEFT_SQUIGGLE statement_list RIGHT_SQUIGGLE

var_assignment_statement =
- VARIABLE_ID EQUALS var_expression |
- VARIABLE_STRING_ID EQUALS var_string_expression |
- VARIABLE_ARRAYED_LEFT_BRACKET var_expression RIGHT_BRACKET EQUALS var_expression |
- VARIABLE_ARRAAYED_STRING LEFT_BRACKET EQuALS var_string_expression

if_statement =
- IF LEFT_PAREN var_expression RIGHT_PAREN statement \[ ELSE statement ]

sendMidiProgramChange_statement =
- sendMidiProgramChange LEFT_PAREN var_expression COMMA var_expression RIGHT_PAREN

sendMidiCC_statement =
- sendMidiCC LEFT_PAREN var_expression COMMA var_expression COMMA var_expression RIGHT_PAREN

sendSerialCC_statement =
- sendSerialCC LEFT_PAREN var_expression COMMA var_expression RIGHT_PAREN

display_statement =
- display LEFT_PAREN DISPLAY_AREA COMMA display_color_expression COMMA var_string_expression RIGHT_PAREN






## F. Other Sections


### BUTTON_section

A BUTTON_section **must** include at least one of a color_decl, short_decl, and/or long_decl.
The button_num constant expression is limited to 0..24.
and cannot have been previously used in any BUTTON or BUTTON_GRID sections.

BUTTON_section = BUTTON LEFT_PAREN button_num RIGHT_BRACKET
- \[ color_decl \[flash_decl]]
- \[ short_decl ]
- \[ long_decl ]

color_decl = COLOR COLON led_color_expression SEMI_COLON

flash_decl = FLASH COLON number_expression SEMI_COLON

short_decl = SHORT COLON_statement_list

long_decl = SHORT COLON statement_list



### BUTTON_GRID_section

A BUTTON_GRID_section **must** include at least one of a color_decl, short_decl, and/or long_decl,
and may make use of the GRID_ROW and GRID_COL internal variables.
The grid_size constant expressions are limited to 1..5, and the button_num expressions atre limited to 0..24.
The button_num constant expressions cannot have been previously used in any BUTTON or BUTTON_GRID sections.


BUTTON_GRID_section = BUTTON_GRID LEFT_PAREN grid_size COMMA grid_size COMMA LEFT_BRACKET button_num \[ COMMA button_num]\* RIGHT_BRACKET RIGHT_PAREN COLON
- \[ color_decl \[flash_decl]]
- \[ short_decl ]
- \[ long_decl ]



### ON_SERIAL_MIDI_section

An ON_SERIAL_MIDI_section must include at least one statement,
and may make use of the cc_num and cc_value internal variables.

ON_SERIAL_MIDI_section = ON_SERIAL_MIDI COLON statement_list



### ON_ROTARY_EVENT_section

An ON_ROTARY_EVENT_section must include at least one statement,
and may make use of the rotary_num and rotary_value internal variables.

ON_ROTARY_EVENT_section = ON_ROTARY_EVENT COLON atatement_list



### PEDAL_section

A PEDAL_section must provide a pedal name, and
may provide midi_channel and midi_cc or serial_cc values
The pedal_num constant expression is limited to 0..4.
The midi_channel constant expression is limited to 0..9.
The midi_cc and serial_cc constant expressions are limited to 0..127.


PEDAL_section = PEDAL LEFT_PAREN pedal_num RIGHT_PAREN COLON
- name COLON constant_string_expression SEMI_COLON
- \\[ midi_channel COLON constant_expression SEMI_COLON ]
- \\[ midi_cc COLON constant_expression SEMI_COLON ]
- \\[ serial_cc COLON constant_expression SEMI_COLON ]



## G. Code Generation and Opcodes

Due to RAM memory constraints, parsing and generating code may require two passes.
On the first pass we would determine the number of bytes needed for variables and
constants.



This functionality will directly impact the songMachine's memory usage.
Both should draw from the same static pool of memory.
Currently the songMachine allocates a number of large static buffers.
