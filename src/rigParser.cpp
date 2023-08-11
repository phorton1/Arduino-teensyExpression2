//-------------------------------------------------------
// rigParser.cpp
//-------------------------------------------------------
// Implements the class that parses Rig Text files
// and creates Rig memory images

#include <myDebug.h>
#include "rigToken.h"
#include "myTFT.h"		// for TFT sizes
#include "theSystem.h"	// for client rect


#define dbg_parse 	0

#define MAX_RIG_CODE 	16384

#define AREA_CLIENT_HEIGHT    (client_rect.ye - client_rect.ys + 1)



static int rig_code_err;
static int rig_code_len;
static uint8_t rig_code[MAX_RIG_CODE];

#define rigTokenString		rigTokenToString(rig_token.id)

#define dbgMethod(s)	if (!dbg_parse) display(0,"%s(%s)",s,rigTokenString)


//-----------------------------------------
// low level utils
//-----------------------------------------

static bool addRigCode(uint8_t byte)
{
	if (rig_code_len >= MAX_RIG_CODE)
	{
		rig_error("CODE TOO LONG");
		rig_code_err = 1;
		return false;
	}
	rig_code[rig_code_len++] = byte;
	return true;
}

static bool addRigText()
{
	int len = strlen(rig_token.text);
	if (rig_code_len >= MAX_RIG_CODE - len - 1)
	{
		rig_error("CODE(STRING) TOO LONG");
		rig_code_err = 1;
		return false;
	}
	for (int i=0; i<len; i++)
	{
		if (!addRigCode(rig_token.text[i]))
			return false;
	}
	if (!addRigCode(0))
		return false;
	return true;
}

static bool addRigNumber()
{
	if (rig_code_len >= MAX_RIG_CODE - 2)
	{
		rig_error("CODE(STRING) TOO LONG");
		rig_code_err = 1;
		return false;
	}
	uint8_t *p = (uint8_t *) &rig_token.int_value;
	if (!addRigCode(p[0]))
		return false;
	if (!addRigCode(p[1]))
		return false;
	return true;
}


static bool lexRig()
{
	dbgSerial->print("# ");
	dbgSerial->print(rig_name);
	dbgSerial->println(".rig");
	dbgSerial->println();

	bool at_newline = 1;	// whether we are at a newline
	int in_subsection = 0;	// whether we are in a subsection
	bool colon_line = 0;		// print a new line after the next colon
	bool started = 0;		// we have encountered the first button

	int rig_code_ptr = 0;

	while (rig_code_ptr < rig_code_len)
	{
		int tt = rig_code[rig_code_ptr++];

		if (tt == RIG_TOKEN_BASERIG ||
			tt == RIG_TOKEN_OVERLAY)
		{
			dbgSerial->print(rigTokenToText(tt));
			dbgSerial->println();
			dbgSerial->println();
			continue;
		}
		if (!started && (
			tt == RIG_TOKEN_LISTEN ||
			tt == RIG_TOKEN_STRING ||
			tt == RIG_TOKEN_AREA ))
		{
			colon_line = 0;
		}
		else if (tt == RIG_TOKEN_BUTTON )
		{
			started = 1;
			colon_line = 1;					// cr after next colon
			in_subsection = 0;
			dbgSerial->println();			// extra cr
		}
		else if (
			tt == RIG_TOKEN_COLOR ||
			tt == RIG_TOKEN_BLINK ||
			tt == RIG_TOKEN_PRESS ||
			tt == RIG_TOKEN_CLICK ||
			tt == RIG_TOKEN_LONG ||
			tt == RIG_TOKEN_RELEASE ||
			tt == RIG_TOKEN_REPEAT )
		{
			in_subsection = 1;
			colon_line = 1;				// cr + 8 spaces after next colon
			dbgSerial->print("    ");
		}


		if (at_newline)
		{
			at_newline = 0;
			if (in_subsection == 2)
				dbgSerial->print("        ");
		}


		if (tt == RIG_TOKEN_TEXT)
		{
			int len = strlen((char *) &rig_code[rig_code_ptr]);
			dbgSerial->print("\"");
			dbgSerial->print((char *) &rig_code[rig_code_ptr]);
			dbgSerial->print("\"");
			rig_code_ptr += len + 1;
		}
		else if (tt == RIG_TOKEN_NUMBER)
		{
			int16_t *p = (int16_t *) &rig_code[rig_code_ptr];
			dbgSerial->print(*p);
			rig_code_ptr += 2;
		}
		else
		{
			if ((!colon_line && tt == RIG_TOKEN_COLON) ||
				tt == RIG_TOKEN_ASSIGN ||
				tt == RIG_TOKEN_PLUS ||
				tt == RIG_TOKEN_BITWISE_OR ||
				tt == RIG_TOKEN_BITWISE_AND ||
				tt == RIG_TOKEN_LOGICAL_OR ||
				tt == RIG_TOKEN_LOGICAL_AND ||
				tt == RIG_TOKEN_QUESTION_MARK)
				dbgSerial->print(" ");
			dbgSerial->print(rigTokenToText(tt));
			if (tt == RIG_TOKEN_COMMA ||
				tt == RIG_TOKEN_NOT ||
				tt == RIG_TOKEN_COLON ||
				tt == RIG_TOKEN_ASSIGN ||
				tt == RIG_TOKEN_PLUS ||
				tt == RIG_TOKEN_BITWISE_OR ||
				tt == RIG_TOKEN_BITWISE_AND ||
				tt == RIG_TOKEN_LOGICAL_OR ||
				tt == RIG_TOKEN_LOGICAL_AND ||
				tt == RIG_TOKEN_QUESTION_MARK)
				dbgSerial->print(" ");
			if (tt == RIG_TOKEN_SEMICOLON)
			{
				at_newline = 1;
				dbgSerial->println();
			}
		}

		if (colon_line && tt == RIG_TOKEN_COLON)
		{
			dbgSerial->println();
			colon_line = 0;
			at_newline = 1;
			if (in_subsection)
				in_subsection = 2;
		}
	}

	dbgSerial->println();
	dbgSerial->println();
	dbgSerial->print("# end of ");
	dbgSerial->print(rig_name);
	dbgSerial->println(".rig");
	dbgSerial->println();

	return 1;
}



// extern
void parseRig()
	// generates the intermediate structure of a
	// syntactically valid set of bytes that are
	// RIG_ID's, and with inline TEXT and int_values;

{
	display(0,"ParseRig()",0);

	rig_code_len = 0;

	if (openRigFile(rig_name))
	{
		int tt = getRigToken();
		while (tt && !rig_code_err)
		{
			if (!addRigCode(tt))
				break;
			if (tt == RIG_TOKEN_NUMBER)
				addRigNumber();
			else if (tt == RIG_TOKEN_TEXT)
				addRigText();
			tt = getRigToken();
		}

		closeRigFile();

		if (!rig_code_err)
			rig_code_err = !lexRig();
		//if (!rig_code_err)
		//	dumpRigCode(rig_code_len, rig_code);

	}
}



