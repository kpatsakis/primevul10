f_complete_check(typval_T *argvars UNUSED, typval_T *rettv)
{
    int		saved = RedrawingDisabled;

    RedrawingDisabled = 0;
    ins_compl_check_keys(0, TRUE);
    rettv->vval.v_number = ins_compl_interrupted();
    RedrawingDisabled = saved;
}