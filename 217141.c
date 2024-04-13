f_complete_add(typval_T *argvars, typval_T *rettv)
{
    if (in_vim9script() && check_for_string_or_dict_arg(argvars, 0) == FAIL)
	return;

    rettv->vval.v_number = ins_compl_add_tv(&argvars[0], 0, FALSE);
}