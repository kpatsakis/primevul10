f_complete_info(typval_T *argvars, typval_T *rettv)
{
    list_T	*what_list = NULL;

    if (rettv_dict_alloc(rettv) == FAIL)
	return;

    if (in_vim9script() && check_for_opt_list_arg(argvars, 0) == FAIL)
	return;

    if (argvars[0].v_type != VAR_UNKNOWN)
    {
	if (argvars[0].v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    return;
	}
	what_list = argvars[0].vval.v_list;
    }
    get_complete_info(what_list, rettv->vval.v_dict);
}