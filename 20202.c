f_argv(typval_T *argvars, typval_T *rettv)
{
    int		idx;
    aentry_T	*arglist = NULL;
    int		argcount = -1;

    if (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	return;

    if (argvars[0].v_type != VAR_UNKNOWN)
    {
	if (argvars[1].v_type == VAR_UNKNOWN)
	{
	    arglist = ARGLIST;
	    argcount = ARGCOUNT;
	}
	else if (argvars[1].v_type == VAR_NUMBER
					   && tv_get_number(&argvars[1]) == -1)
	{
	    arglist = GARGLIST;
	    argcount = GARGCOUNT;
	}
	else
	{
	    win_T	*wp = find_win_by_nr_or_id(&argvars[1]);

	    if (wp != NULL)
	    {
		// Use the argument list of the specified window
		arglist = WARGLIST(wp);
		argcount = WARGCOUNT(wp);
	    }
	}

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = NULL;
	idx = tv_get_number_chk(&argvars[0], NULL);
	if (arglist != NULL && idx >= 0 && idx < argcount)
	    rettv->vval.v_string = vim_strsave(alist_name(&arglist[idx]));
	else if (idx == -1)
	    get_arglist_as_rettv(arglist, argcount, rettv);
    }
    else
	get_arglist_as_rettv(ARGLIST, ARGCOUNT, rettv);
}