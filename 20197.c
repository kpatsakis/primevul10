f_argc(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    if (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	return;

    if (argvars[0].v_type == VAR_UNKNOWN)
	// use the current window
	rettv->vval.v_number = ARGCOUNT;
    else if (argvars[0].v_type == VAR_NUMBER
					   && tv_get_number(&argvars[0]) == -1)
	// use the global argument list
	rettv->vval.v_number = GARGCOUNT;
    else
    {
	// use the argument list of the specified window
	wp = find_win_by_nr_or_id(&argvars[0]);
	if (wp != NULL)
	    rettv->vval.v_number = WARGCOUNT(wp);
	else
	    rettv->vval.v_number = -1;
    }
}