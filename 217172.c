ins_compl_set_original_text(char_u *str)
{
    char_u	*p;

    // Replace the original text entry.
    // The CP_ORIGINAL_TEXT flag is either at the first item or might possibly
    // be at the last item for backward completion
    if (match_at_original_text(compl_first_match))	// safety check
    {
	p = vim_strsave(str);
	if (p != NULL)
	{
	    vim_free(compl_first_match->cp_str);
	    compl_first_match->cp_str = p;
	}
    }
    else if (compl_first_match->cp_prev != NULL
	    && match_at_original_text(compl_first_match->cp_prev))
    {
       p = vim_strsave(str);
       if (p != NULL)
       {
	   vim_free(compl_first_match->cp_prev->cp_str);
	   compl_first_match->cp_prev->cp_str = p;
       }
    }
}