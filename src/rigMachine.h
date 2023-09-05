//-------------------------------------------------------
// rigMachine.h
//-------------------------------------------------------

#pragma once

#include "rigParser.h"
#include "midiQueue.h"

#define MAX_PARAMS			8		// from AREA statement
#define MAX_RIG_STACK  		10

#define AREA_TYPE_NONE		0
#define AREA_TYPE_STRING	1
#define AREA_TYPE_VMETER	2
#define AREA_TYPE_HMETER    3


typedef struct
{
	uint8_t		type;

	uint8_t		font_size;
	uint8_t     font_type;		// NORMAL, BOLD
	uint8_t		font_just;		// LEFT, CENTER, RIGHT
	int16_t		xs;
	int16_t		ys;
	int16_t		xe;
	int16_t		ye;

	int			last_display_value;
	const char *last_display_string;

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



typedef struct
{
	const rig_t *rig;
	char name[MAX_RIG_NAME + 1];
	const char *define_pool;
	const char *string_pool;
	const uint8_t *statement_pool;
	const uint8_t *expression_pool;
} rigStack_t;




class rigMachine
{
public:

	void invalidateRig();
	void loadDefaultRig();
	bool loadRig(const char *name);

	void updateUI();
    void onButton(int row, int col, int event);
	void onMidiCC(const msgUnion &msg);
	uint16_t getListenMask()  { return m_listen_mask; }
		// returns a mask of 1<<msg.portEnum() of any ports
		// which have listen statements

	void restartRig();

private:

	bool m_rig_loaded;

	rigState_t m_rig_state;
	evalResult_t m_param_values[MAX_PARAMS];

	int m_stack_ptr;
	rigStack_t m_stack[MAX_RIG_STACK];

	uint16_t m_load_state;
	uint16_t m_listen_mask;
		// 7 bits for each port for more rapid processing

	void popRig();
	bool pushRig(const rig_t *rig, const char *name);
	bool startRig(const rig_t *rig, bool cold);
	void showRigName();

	bool executeStatementList(const rig_t *rig, int statement_num);
	bool executeStatement(const rig_t *rig, uint16_t *offset, uint16_t last_offset);
	bool evalParam(const rig_t *rig, evalResult_t *rslt,int arg_type, const uint8_t *code, uint16_t *offset);
	bool evalCodeExpression(const rig_t *rig, evalResult_t *rslt, const char *what, uint16_t offset);
	bool evalExpression(const rig_t *rig, evalResult_t *rslt, const char *what, const uint8_t *code, uint16_t *offset);

	bool expression(const rig_t *rig, evalResult_t *rslt, const uint8_t *code, uint16_t *offset);
	bool getAtom(const rig_t *rig, const uint8_t *code, uint16_t *offset);

	bool evaluate(const rig_t *rig, const uint8_t *code, uint16_t *offset);
	const uint16_t *inheritButtonRefs(int num, const rig_t **ret_context);

	void rigDisplay(uint16_t area_num, uint16_t color, const char *text);

};	// class rigMachine



extern rigMachine rig_machine;


// end of rigMachine.h
