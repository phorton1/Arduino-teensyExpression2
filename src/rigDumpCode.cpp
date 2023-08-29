//-------------------------------------------------------
// rigDumpCode.cpp
//-------------------------------------------------------

#include "rigDump.h"		// by convention
#include <myDebug.h>
#include "rigParser.h"
#include "fileSystem.h"

#define MAX_DUMP_BUF 255

static myFileType_t rig_file;

static int print_level = 0;

//-------------------------------------
// display primitives
//-------------------------------------

static void displayIndent()
{
	for (int i=0; i<print_level; i++)
		rig_file.print("    ");
}
static void displayNum(const char *name,uint16_t value)
{
	dbgSerial->print(".");
	displayIndent();
	rig_file.print(name);
	rig_file.print(" = ");
	rig_file.print(value);
	rig_file.println(",");
}
static void displayAssign(const char *name,const char *prefix)
{
	dbgSerial->print(".");
	displayIndent();
	rig_file.print(".");
	rig_file.print(name);
	rig_file.print(" = ");
	rig_file.print(prefix);
	rig_file.print("_");
	rig_file.print(name);
	rig_file.println(",");
}
static void dumpDisplay(const char *line)
{
	dbgSerial->print(".");
	displayIndent();
	rig_file.println(line);
}
static void displayNameHeader(const char *name)
{
	dbgSerial->print(".");
	displayIndent();
	rig_file.print(name);
	rig_file.println(" = {");
}


//------------------------------------------
// aggregate dumpers
//------------------------------------------


static void dumpArray16(const rig_t *rig, const char *name, const uint16_t *ptr, uint16_t numA, uint16_t numB)
	// numB must be less than 8 due to line_buf size
{
	displayNameHeader(name);
	print_level++;
	char dump_buf[MAX_DUMP_BUF];
	for (int i=0; i<numA; i++)
	{
		sprintf(dump_buf,"{ ");
		for (int j=0; j<numB; j++)
		{
			sprintf(&dump_buf[strlen(dump_buf)],"0x%04x, ",*ptr++);
		}
		sprintf(&dump_buf[strlen(dump_buf)]," },");
		dumpDisplay(dump_buf);
	}
	print_level--;
	dumpDisplay("},");
}


static void dumpCharStrings(const char *rig_name, const char *name, const char *ptr, uint16_t pool_len)
{
	char dump_buf[MAX_DUMP_BUF];

	displayIndent();
	rig_file.print("const char *");
	rig_file.print(rig_name);
	rig_file.print("_rig");
	rig_file.print(name);
	rig_file.println(" = ");

	print_level++;
	uint16_t offset = 0;
	while (offset < pool_len)
	{
		dbgSerial->print("s");
		int len = strlen(ptr);
		sprintf(dump_buf,"/* %-5d */  \"%s",offset,ptr);
		ptr += len + 1;
		offset += len + 1;
		if (offset < pool_len)
		{
			sprintf(&dump_buf[strlen(dump_buf)],"\\0\"");
		}
		else
			sprintf(&dump_buf[strlen(dump_buf)],"\";");
		dumpDisplay(dump_buf);
	}
	print_level--;
}



static void dumpListNum(
	bool is16,
	bool hex,
	bool end_with_semi_colon,
	const char *name,
	const void *ptr,
	uint16_t num,
	bool stop_on_zero = false)
{
	char dump_buf[MAX_DUMP_BUF];
	displayNameHeader(name);
	print_level++;

	bool more = 1;
	dump_buf[0] = 0;

	int width = is16 ? 6 :
		hex ? 5 : 4;
	char format_buf[8];
	sprintf(format_buf,"%%-%ds ",width);

	uint8_t *ptr8 = (uint8_t *) ptr;
	uint16_t *ptr16 = (uint16_t *) ptr;
	int num_per_line = is16 ? 10 : 16;

	for (uint16_t i=0; i<num && more; i++)
	{
		if (i % num_per_line == 0)
		{
			if (dump_buf[0])
				dumpDisplay(dump_buf);
			dump_buf[0] = 0;
		}

		char num_buf[10];
		if (is16)	// no hexes on these
		{
			sprintf(num_buf,"%d,",*ptr16++);
			more = !stop_on_zero || *ptr16;
		}
		else
		{
			if (hex)
				sprintf(num_buf,"0x%02x,",*ptr8++);
			else
				sprintf(num_buf,"%d,",*ptr8++);
			more = !stop_on_zero || *ptr8;
		}

		sprintf(&dump_buf[strlen(dump_buf)],format_buf,num_buf);

	}
	if (dump_buf[0])
		dumpDisplay(dump_buf);
	print_level--;

	if (end_with_semi_colon)
		dumpDisplay("};");
	else
		dumpDisplay("},");
}


static void dumpPool(const char *rig_name, const char *pool_name, const void *ptr, uint16_t num)
{
	char name_buf[80];
	sprintf(name_buf,"const uint8_t %s%s%s[] ",rig_name,"_rig",pool_name);
	dumpListNum(0,1,1,name_buf,ptr,num);
}

//------------------------------------------------
// main
//-------------------------------------------------

// extern
void dumpRigCode(const rig_t *rig, const char *prefix)
{
	char filename[80];
	sprintf(filename,"%s.rig.h",prefix);
	display(0,"dumpRigCode(%s) = %s",prefix,filename);

    SD.remove(filename);
	rig_file = SD.open(filename, FILE_WRITE);
	if (!rig_file)
	{
		rig_error("Could not open %s for writing ..",filename);
		return;
	}

	print_level = 0;

	rig_file.println("//------------------------------------------------------------------------------");
	rig_file.print("// ");
	rig_file.print(prefix);
	rig_file.println(".rig.h");
	rig_file.println("//------------------------------------------------------------------------------");
	rig_file.println();
	rig_file.println("#pragma once ");
	rig_file.println();
	rig_file.println();

	dumpCharStrings(prefix,"_define_pool",rig->define_pool,rig->define_pool_len);
	rig_file.println();
	rig_file.println();
	dumpCharStrings(prefix,"_string_pool",rig->string_pool,rig->string_pool_len);
	rig_file.println();
	rig_file.println();
	dumpPool(prefix,"_statement_pool",rig->statement_pool,rig->statement_pool_len);
	rig_file.println();
	rig_file.println();
	dumpPool(prefix,"_expression_pool",rig->expression_pool,rig->expression_pool_len);
	rig_file.println();
	rig_file.println();
	rig_file.println();

	rig_file.print("const rig_t ");
	rig_file.print(prefix);
	rig_file.println("_rig = { ");

	print_level = 1;

	displayNum(".rig_type", rig->rig_type | RIG_TYPE_SYSTEM);
	displayNum(".num_statements",rig->num_statements);
	displayNum(".define_pool_len",rig->define_pool_len);
	displayNum(".string_pool_len",rig->string_pool_len);
	displayNum(".statement_pool_len",rig->statement_pool_len);
	displayNum(".expression_pool_len",rig->expression_pool_len);

	dumpListNum(1,0,0,".define_ids",rig->define_ids,RIG_NUM_DEFINES-1);
	dumpListNum(0,0,0,".define_values",rig->define_values,RIG_NUM_DEFINES-1);
	dumpListNum(1,0,0,".statements",rig->statements,MAX_STATEMENTS+1,true);
	dumpArray16(rig,".button_refs",(const uint16_t *)rig->button_refs,NUM_BUTTONS,NUM_SUBSECTIONS);
	dumpListNum(1,0,0,".strings",rig->strings,RIG_NUM_STRINGS);

	displayAssign("define_pool",prefix);
	displayAssign("string_pool",prefix);
	displayAssign("statement_pool",prefix);
	displayAssign("expression_pool",prefix);

	print_level = 0;

	dumpDisplay("};");	// end the rig_t
	rig_file.println();
	rig_file.println();

	rig_file.print("// end of ");
	rig_file.print(prefix);
	rig_file.println("_rig.h");
	rig_file.println();
	rig_file.println();
	rig_file.close();

	dbgSerial->println();
	display(0,"dumpRigCode(%s) finished",filename);

}
