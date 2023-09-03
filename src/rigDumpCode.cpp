//-------------------------------------------------------
// rigDumpCode.cpp
//-------------------------------------------------------

#include "rigDump.h"		// by convention
#include <myDebug.h>
#include "rigParser.h"
#include "fileSystem.h"

#define MAX_DUMP_BUF 255

// limited macros to use dbgSerial if it is set

#define dbgPrintln()		{if (dbgSerial) dbgSerial->println(); }
#define dbgPrint(a)			{if (dbgSerial) dbgSerial->print(a); }


static myFileType_t rig_file;

static int print_level = 0;


//-------------------------------------
// display methods
//-------------------------------------

static void displayIndent()
{
	for (int i=0; i<print_level; i++)
		rig_file.print("    ");
}
static void displayNum(const char *name,uint16_t value)
{
	dbgPrint("=");
	displayIndent();
	rig_file.print(name);
	rig_file.print(" = ");
	rig_file.print(value);
	rig_file.println(",");
}
static void displayAssign(const char *name,const char *prefix)
{
	dbgPrint("<");
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
	dbgPrint(".");
	displayIndent();
	rig_file.println(line);
}


//------------------------------------------
// aggregate dumper
//------------------------------------------

#define DT_16			0x001		// otherwise it's 8 bits
#define DT_HEX			0x002		// otherwise it's a number
#define DT_CHAR			0x004		// line oriented on zeros; otherwise it's hex
#define DT_SEMI_COLON	0x008		// otherwise it's ends with a comma and followed with two blank lines
#define DT_STOP_ZERO	0x010		// stop on first zero found
#define DT_CONST_CHAR   0x020		// it's a const char * declaration
#define DT_CONST_UINT8  0x040		// it's a const uint8_t * declaration
#define DT_GROUP_BY7	0x080		// group button_refs by 7's


static void dumpArray(
	uint16_t how,
	const char *prefix,
	const char *name,
	const void *ptr,
	uint16_t num)
{
	dbgPrint("{");

	displayIndent();

	if (how & DT_CONST_CHAR)
		rig_file.print("const char ");
	else if (how & DT_CONST_UINT8)
		rig_file.print("const uint8_t ");

	if (how & (DT_CONST_CHAR | DT_CONST_UINT8))
		rig_file.print(prefix);

	rig_file.print(name);
	if (how & (DT_CONST_CHAR | DT_CONST_UINT8))
		rig_file.print("[]");
	rig_file.println(" = {");
	print_level++;

	int width =
		how & DT_CHAR ? 4 :
		how & DT_HEX ?
			how & DT_16 ? 8 : 6 :
			how & DT_16 ? 7 : 5 ;

	int num_per_line =
		how & DT_GROUP_BY7 ? 7 :
		how & DT_16 ? 10 : 16;

	char format1[12];
	char format2[12];
	if (how & DT_CHAR)
	{
		strcpy(format1,"'%c',");
		strcpy(format2,"%s");
	}
	else
	{
		if (how & DT_HEX)
			sprintf(format1,"0x%%0%dx,", how & DT_16 ? 4 : 2);
		else
			strcpy(format1,"%d,");
		sprintf(format2,"%%-%ds ",width);
	}

	char *cptr = (char *) ptr;
	uint8_t *ptr8 = (uint8_t *) ptr;
	uint16_t *ptr16 = (uint16_t *) ptr;

	bool more = 1;
	uint16_t offset = 0;
	char dump_buf[MAX_DUMP_BUF] = {0};

	while (offset < num && more)
	{
		dbgPrint(".");
		if (how & DT_CHAR)
		{
			displayIndent();
			while (*cptr)
			{
				sprintf(dump_buf,format1,*cptr++);
				rig_file.print(dump_buf);
				offset++;
				dump_buf[0] = 0;
			}
			rig_file.println("0,");
			cptr++;
			offset++;
		}
		else
		{
			char line_buf[12];
			if (offset % num_per_line == 0)
			{
				if (dump_buf[0])
				{
					if (how & DT_GROUP_BY7)
						sprintf(&dump_buf[strlen(dump_buf)],"},");
					rig_file.println(dump_buf);
					dump_buf[0] = 0;
				}
				dump_buf[0] = 0;
				displayIndent();
				if (how & DT_GROUP_BY7)
					rig_file.print("{ ");
			}

			if (how & DT_16)
			{
				sprintf(line_buf,format1,*ptr16++);
				more = *ptr16 || !(how & DT_STOP_ZERO);
			}
			else // how && DT_8
			{
				sprintf(line_buf,format1,*ptr8++);
				more = *ptr8 || !(how & DT_STOP_ZERO);
			}

			sprintf(&dump_buf[strlen(dump_buf)],format2,line_buf);
			offset++;
		}
	}

	if (dump_buf[0])
	{
		if (how & DT_GROUP_BY7)
			sprintf(&dump_buf[strlen(dump_buf)],"},");
		rig_file.println(dump_buf);
	}

	print_level--;
	displayIndent();
	if (how & DT_SEMI_COLON)
	{
		rig_file.println("};");
		rig_file.println();
		rig_file.println();
	}
	else
		rig_file.println("},");

	dbgPrint("}");
}


//------------------------------------------------
// main
//-------------------------------------------------

// extern
void dumpRigCode(const rig_t *rig)
{
	const char *prefix = rig->rig_type & RIG_TYPE_MODAL ?
		"default_modal" : "default";

	char filename[80];
	sprintf(filename,"%s.rig.h",prefix);
	display(0,"dumpRigCode(%s) = %s",prefix,filename);

    SD.remove(filename);
	rig_file = SD.open(filename, FILE_WRITE);
	if (!rig_file)
	{
		rig_error("dumpRigCode() could not open %s for writing ..",filename);
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

	dumpArray( DT_CHAR | DT_CONST_CHAR | DT_SEMI_COLON,
		prefix,"_define_pool",rig->define_pool,rig->define_pool_len);

	dumpArray( DT_CHAR | DT_CONST_CHAR | DT_SEMI_COLON,
		prefix,"_string_pool",rig->string_pool,rig->string_pool_len);

	dumpArray( DT_HEX | DT_CONST_UINT8 | DT_SEMI_COLON,
		prefix,"_statement_pool",rig->statement_pool,rig->statement_pool_len);

	dumpArray( DT_HEX | DT_CONST_UINT8 | DT_SEMI_COLON,
		prefix,"_expression_pool",rig->expression_pool,rig->expression_pool_len);

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

	dumpArray( DT_16,
		prefix,".define_ids",rig->define_ids,RIG_NUM_DEFINES);
	dumpArray( 0,
		prefix,".define_values",rig->define_values,RIG_NUM_DEFINES);
	dumpArray( DT_16 | DT_STOP_ZERO,
		prefix,".statements",rig->statements,MAX_STATEMENTS+1);
	dumpArray( DT_16 | DT_HEX | DT_GROUP_BY7,
		prefix,".button_refs",(const uint16_t *)rig->button_refs,NUM_BUTTONS*NUM_SUBSECTIONS);
	dumpArray( DT_16,
		prefix,".strings",rig->strings,RIG_NUM_STRINGS);

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

	dbgPrintln();
	display(0,"dumpRigCode(%s) finished",filename);

}
