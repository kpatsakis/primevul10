static u32 PrintHelpForArgs(char *arg_name, GF_GPACArg *args, u32 search_type)
{
	u32 res=0;
	u32 i=0;
	u32 alen = (u32) strlen(arg_name);
	while (args[i].name) {
		u32 flags=0;
		GF_GPACArg *arg = &args[i];
		GF_GPACArg an_arg;
		Bool do_match = GF_FALSE;

		if (args==ImportFileOpts) {
			flags = GF_PRINTARG_COLON;
			if (!strncmp(arg_name, arg->name, alen) && ((arg->name[alen]==0) || (arg->name[alen]=='=')))
				do_match = GF_TRUE;
		}
		else if (!strcmp(arg_name, arg->name))
			do_match = GF_TRUE;
		else if ((alen < (u32) strlen(arg->name)) && (arg->name[alen]==' ') && !strncmp(arg_name, arg->name, alen))
			do_match = GF_TRUE;

		if (arg_name[0] == '@')
			do_match = GF_TRUE;

		if ((search_type==SEARCH_ARG_EXACT) && !do_match) {
			i++;
			continue;
		}
		if ((search_type==SEARCH_ARG_CLOSE) && !gf_sys_word_match(arg_name, arg->name)) {
			i++;
			continue;
		}
		if ((search_type==SEARCH_DESC) && !strstr_nocase(arg->description, arg_name, alen)) {
			i++;
			continue;
		}

		an_arg = *arg;
		if (search_type!=SEARCH_ARG_EXACT) {
			an_arg.description = NULL;
			an_arg.type = GF_ARG_BOOL;
		}
		gf_sys_print_arg(helpout, flags, &an_arg, "");
		res++;
		i++;
	}
	return res;
}