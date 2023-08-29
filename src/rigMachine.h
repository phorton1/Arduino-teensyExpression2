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
	uint8_t     active;		// the rig_pointer stack_ptr + 1 that owns this (1..16)
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

	void loadDefaultRig();
	bool loadRig(const char *name);

	bool loaded()		{ return m_rig_loaded; }
	const char *rigName()	{ return m_rig_name; }

	void updateUI();
    void onButton(int row, int col, int event);
	void onMidiCC(int port, int channel, int cc_num, int value);
		// port is an enum
		// channel is one based


private:


	bool m_rig_loaded;
	char m_rig_name[MAX_RIG_NAME + 1];

	rigState_t m_rig_state;
	evalResult_t m_param_values[MAX_PARAMS];

	bool pushRig(const rig_t *rig, const char *name);
	void popRig(bool exec_prev);

	bool startRig(const rig_t *rig, bool cold);

	bool executeStatementList(const rig_t *rig, int statement_num);
	bool executeStatement(const rig_t *rig, uint16_t *offset, uint16_t last_offset);
	bool evalParam(const rig_t *rig, evalResult_t *rslt,int arg_type, const uint8_t *code, uint16_t *offset);
	bool evalCodeExpression(const rig_t *rig, evalResult_t *rslt, const char *what, uint16_t offset);
	bool evalExpression(const rig_t *rig, evalResult_t *rslt, const char *what, const uint8_t *code, uint16_t *offset);

	bool expression(const rig_t *rig, evalResult_t *rslt, const uint8_t *code, uint16_t *offset);
	bool evaluate(const rig_t *rig, const uint8_t *code, uint16_t *offset);
	bool getAtom(const rig_t *rig, const uint8_t *code, uint16_t *offset);

	void rigDisplay(uint16_t area_num, uint16_t color, const char *text);

};	// class rigMachine



extern rigMachine rig_machine;


// end of rigMachine.h
