//-------------------------------------------------------
// rigDump.h
//-------------------------------------------------------
// Included inline in rigParser.cpp to dump intermediate code for sanity

// a buffer to build statements and expressions

#define dbg_dump 1


#define MAX_DUMP_BUF 255
int dump_buf_ptr = 0;
char dump_buf[MAX_DUMP_BUF + 1];


static void dumpExpression(int offset)
{
	sprintf(&dump_buf[strlen(dump_buf)],"%d",expression_pool[offset]);
}


static void dumpParam(bool button_section, int arg_type, bool last, uint8_t **code)
{
	switch (arg_type)
	{
		case PARAM_VALUE_NUM 	:
		case PARAM_AREA_NUM     :
		case PARAM_MIDI_CHANNEL :
		case PARAM_MIDI_CC      :
			sprintf(&dump_buf[strlen(dump_buf)],"%d",*(*code)++);
			break;
		case PARAM_MIDI_PORT    :
			sprintf(&dump_buf[strlen(dump_buf)],"%s",
				rigTokenToText(*(*code)++ + RIG_TOKEN_MIDI));
		case PARAM_NUM_EXPRESSION				:
		case PARAM_STRING_EXPRESSION			:
		case PARAM_LED_COLOR_EXPRESSION			:
		case PARAM_DISPLAY_COLOR_EXPRESSION		:
			uint16_t *ptr16 = (uint16_t *) *code;
			dumpExpression(*ptr16);
			break;
	}
	sprintf(&dump_buf[strlen(dump_buf)],"%s",last?");":", ");
}


static void dumpStatement(bool button_section, uint16_t statement_num)
{
	dump_buf[0] = 0;

	uint16_t offset = statements[statement_num];
	uint8_t *code = &statement_pool[offset];

	proc_level = button_section ? 2 : 0;
	display(dbg_dump,"# dumping statement %d at offset %d",statement_num,offset);

	int tt = *code++;

	sprintf(dump_buf,"%s(",rigTokenToText(tt));
	const statement_param_t *params = findParams(tt);
	if (params) // opposite should never happen
	{
		const int *arg = params->args;
		while (*arg)
		{
			const int *ptr = arg++;
			dumpParam(button_section,*ptr,!*arg,&code);
		}
	}

	display(0,dump_buf,0);
	proc_level = 0;
}


static void dumpStatementList(bool button_section, int statement_list_num)
{
	uint16_t first_statement = statement_lists[statement_list_num];
	uint16_t num_statements  = statement_lists[statement_list_num+1] - first_statement;

	proc_level = button_section ? 2 : 0;
	display(dbg_dump,"# dumping statment_list[%d] with %d statements",statement_list_num,num_statements);
	for (int i=0; i<num_statements; i++)
	{
		dumpStatement(button_section,first_statement+i);
	}

}

static void dumpRig()
{
	int save_proc_level = proc_level;
	proc_level = 0;

	// display interesting numbers]

	display(dbg_dump,"Dumping Rig",0);
	display(0,"expression_pool_len=%d",statement_pool_len);
	display(dbg_dump,"statement_pool_len=%d",statement_pool_len);
	display(dbg_dump,"num_statements=%d",num_statements);
	display(dbg_dump,"num_statement_lists=%d",num_statement_lists);
	for (int i=0; i<=num_statement_lists; i++)
		display(dbg_dump,"statement_list[%d] = %d",i,statement_lists[i]);

	// dump the program

	display(0,"------------------------------------------------",0);
	display(0,"",0);
	display(0,"%s",rig_header.overlay_type?"Overlay":"BaseRig");
	display(0,"",0);

	for (int i=0; i<NUM_PEDALS; i++)
	{
		if (rig_header.pedals[i].name[0])	// definition indicator - they must name em'
		{
			display(0,"PEDAL(%d, \"%s\", %s, %d, %d);",
				i,
				rig_header.pedals[i].name,
				rigTokenToString(rig_header.pedals[i].port + RIG_TOKEN_MIDI),
				rig_header.pedals[i].channel,
				rig_header.pedals[i].cc);
		}
	}
	for (int i=0; i<RIG_NUM_AREAS; i++)
	{
		if (rig_header.areas[i].font_size)	// definition indicator
		{
			display(0,"AREA(%d, %d %s, %s, %d, %d, %d, %d);",
				i,
				rig_header.areas[i].font_size,
				rigTokenToString(rig_header.areas[i].font_type + RIG_TOKEN_NORMAL),
				rigTokenToString(rig_header.areas[i].font_just + RIG_TOKEN_LEFT),
				rig_header.areas[i].xs,
				rig_header.areas[i].ys,
				rig_header.areas[i].xe,
				rig_header.areas[i].ye);
		}
	}
	for (int i=0; i<RIG_NUM_LISTENS; i++)
	{
		if (rig_header.listens[i].active)	// definition indicator
		{
			display(0,"LISTEN(%d, %s, %d, %d);",
				i,
				rigTokenToString(rig_header.listens[i].port ? RIG_TOKEN_SERIAL : RIG_TOKEN_MIDI),
				rig_header.listens[i].channel,
				rig_header.listens[i].cc);
		}
	}

	for (int i=0; i<RIG_NUM_STRINGS; i++)
	{
		// the offset is incremented so that we can identify
		// accesses to string 0 explicitly.
		uint16_t string_offset = rig_header.strings[i];

		if (string_offset--)	// test and adjust offset
		{
			display(0,"STRING(%d, \"%s\");",
				i,
				&string_pool[string_offset]);
		}
	}

	// dump init_section statement list

	dumpStatementList(0,0);
	display(0,"",0);

	// to dump buttons we check if any off it's sections exist
	// there is no single indicator for whether a button is defined

	for (int i=0; i<NUM_BUTTONS; i++)
	{
		bool started = 0;
		for (int j=0; j<NUM_SUBSECTIONS; j++)
		{
			if (rig_header.button_refs[i][j])
			{
				if (!started)
				{
					started = 1;
					display(0,"BUTTON(%d):",i);
				}

				display(0,"    %s:", rigTokenToText(j + RIG_TOKEN_COLOR));

				if (j >= SUBSECTION_FIRST_CODE)	// statement lists
				{
					// statement list num is 1 based
					// so that we can identify a used statement list
					dumpStatementList(1,rig_header.button_refs[i][j] - 1);
				}
				else
				{
					dump_buf[0] = 0;
					dumpExpression(rig_header.button_refs[i][j] - 1);
					proc_level = 2;
					display(0,"%s;",dump_buf);
					proc_level = 0;
				}
			}
		}
	}

	proc_level = save_proc_level;

}
