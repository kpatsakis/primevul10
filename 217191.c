f_complete(typval_T *argvars, typval_T *rettv UNUSED)
{
    int	    startcol;

    if (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_list_arg(argvars, 1) == FAIL))
	return;

    if ((State & MODE_INSERT) == 0)
    {
	emsg(_(e_complete_can_only_be_used_in_insert_mode));
	return;
    }

    // Check for undo allowed here, because if something was already inserted
    // the line was already saved for undo and this check isn't done.
    if (!undo_allowed())
	return;

    if (argvars[1].v_type != VAR_LIST || argvars[1].vval.v_list == NULL)
	emsg(_(e_invalid_argument));
    else
    {
	startcol = (int)tv_get_number_chk(&argvars[0], NULL);
	if (startcol > 0)
	    set_completion(startcol - 1, argvars[1].vval.v_list);
    }
}