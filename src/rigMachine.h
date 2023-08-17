//-------------------------------------------------------
// rigMachine.h
//-------------------------------------------------------

#pragma once

#include "rigParser.h"

#define MAX_RIG_NAME   		31
#define MAX_PARAMS			8		// from AREA statement


typedef struct
{
	uint8_t		font_size;
	uint8_t     font_type;		// NORMAL, BOLD
	uint8_t		font_just;		// LEFT, CENTER, RIGHT
	int16_t		xs;
	int16_t		ys;
	int16_t		xe;
	int16_t		ye;
} rigArea_t;	// 10 bytes


typedef struct
{
	uint8_t     active;		// 1 if this is an active listen
	uint8_t 	port;		// MIDI, SERIAL, etc
	uint8_t		channel;	// 0..MIDI_MAX_CHANNEL
	uint8_t		cc;			// 0..MIDI_MAX_VALUE
} rigListen_t;


typedef struct
{

	rigArea_t	areas[RIG_NUM_AREAS];
	uint8_t		values[RIG_NUM_VALUES];
	rigListen_t	listens[RIG_NUM_LISTENS];
} rigState_t;


typedef struct
{
	bool is_string;
	union {
		uint16_t	value;
		const char *text;
	};

} evalResult_t;



class rigMachine
{
public:

	bool loadRig(const char *name);

	bool loaded()		{ return m_rig_loaded; }
	const char *rigName()	{ return m_rig_name; }

	void updateUI();
    void onButton(int row, int col, int event);
	void onMidiCC(int port, int channel, int cc_num, int value);	// not yet called
	void onSerialMidi(int cc_num, int value);

private:


	bool m_rig_loaded;
	char m_rig_name[MAX_RIG_NAME + 1];

	rigState_t m_rig_state;
	evalResult_t m_param_values[MAX_PARAMS];

	bool startRig();
	bool executeStatementList(int statement_num);
	bool executeStatement(uint16_t *offset, uint16_t last_offset);
	bool evalParam(evalResult_t *rslt,int arg_type, const uint8_t *code, uint16_t *offset);
	bool evalCodeExpression(evalResult_t *rslt, const char *what, uint16_t offset);
	bool evalExpression(evalResult_t *rslt, const char *what, const uint8_t *code, uint16_t *offset);

	bool expression(evalResult_t *rslt, const uint8_t *code, uint16_t *offset);
	bool evaluate(const uint8_t *code, uint16_t *offset);
	bool getAtom(const uint8_t *code, uint16_t *offset);

	void rigDisplay(uint16_t area_num, uint16_t color, const char *text);

};	// class rigMachine



extern rigMachine rig_machine;


// end of rigMachine.h
