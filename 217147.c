ins_compl_clear(void)
{
    compl_cont_status = 0;
    compl_started = FALSE;
    compl_matches = 0;
    VIM_CLEAR(compl_pattern);
    VIM_CLEAR(compl_leader);
    edit_submode_extra = NULL;
    VIM_CLEAR(compl_orig_text);
    compl_enter_selects = FALSE;
#ifdef FEAT_EVAL
    // clear v:completed_item
    set_vim_var_dict(VV_COMPLETED_ITEM, dict_alloc_lock(VAR_FIXED));
#endif
}