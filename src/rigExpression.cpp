//-------------------------------------------------------
// rigExpression.cpp
//-------------------------------------------------------
// Contains the implementation of the rig Expression Code
// genearator and interpreter.

#include <myDebug.h>
#include "rigExpression.h"
#include "rigCode.h"
#include "rigToken.h"
#include "rigParser.h"

#define MAX_EXPRESSION  1024

static uint8_t exp_buf[MAX_EXPRESSION];

static bool skipExpression()
{
	while (rig_token.id != RIG_TOKEN_COMMA &&
		   rig_token.id != RIG_TOKEN_RIGHT_PAREN &&
		   rig_token.id != RIG_TOKEN_SEMICOLON)
	{
		if (!getRigToken())
			return false;
	}
	return true;
}



// extern
uint8_t *rigNumericExpression(int tt)
{
	// display(0,"NUM_EXPRESSION",0);
	skipExpression();
	return exp_buf;
}


// extern
uint8_t *rigStringExpression(int tt)
{
	// display(0,"STRING_EXPRESSION",0);
	skipExpression();
	return exp_buf;
}


// extern
uint8_t *rigLedColorExpression(int tt)
{
	// display(0,"LED_COLOR_EXPRESSION",0);
	skipExpression();
	return exp_buf;
}


// extern
uint8_t *rigDisplayColorExpression(int tt)
{
	// display(0,"DISPLAY_COLOR_EXPRESSION",0);
	skipExpression();
	return exp_buf;
}
