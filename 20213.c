f_arglistid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    if (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	return;

    rettv->vval.v_number = -1;
    wp = find_tabwin(&argvars[0], &argvars[1], NULL);
    if (wp != NULL)
	rettv->vval.v_number = wp->w_alist->id;
}