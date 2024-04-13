ins_compl_insert(int in_compl_func)
{
    int compl_len = get_compl_len();

    // Make sure we don't go over the end of the string, this can happen with
    // illegal bytes.
    if (compl_len < (int)STRLEN(compl_shown_match->cp_str))
	ins_bytes(compl_shown_match->cp_str + compl_len);
    if (match_at_original_text(compl_shown_match))
	compl_used_match = FALSE;
    else
	compl_used_match = TRUE;
#ifdef FEAT_EVAL
    {
	dict_T *dict = ins_compl_dict_alloc(compl_shown_match);

	set_vim_var_dict(VV_COMPLETED_ITEM, dict);
    }
#endif
    if (!in_compl_func)
	compl_curr_match = compl_shown_match;
}