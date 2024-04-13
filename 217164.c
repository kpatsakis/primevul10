ins_compl_free(void)
{
    compl_T *match;
    int	    i;

    VIM_CLEAR(compl_pattern);
    VIM_CLEAR(compl_leader);

    if (compl_first_match == NULL)
	return;

    ins_compl_del_pum();
    pum_clear();

    compl_curr_match = compl_first_match;
    do
    {
	match = compl_curr_match;
	compl_curr_match = compl_curr_match->cp_next;
	vim_free(match->cp_str);
	// several entries may use the same fname, free it just once.
	if (match->cp_flags & CP_FREE_FNAME)
	    vim_free(match->cp_fname);
	for (i = 0; i < CPT_COUNT; ++i)
	    vim_free(match->cp_text[i]);
#ifdef FEAT_EVAL
	clear_tv(&match->cp_user_data);
#endif
	vim_free(match);
    } while (compl_curr_match != NULL && !is_first_match(compl_curr_match));
    compl_first_match = compl_curr_match = NULL;
    compl_shown_match = NULL;
    compl_old_match = NULL;
}