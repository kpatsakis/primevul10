get_next_filename_completion(void)
{
    char_u	**matches;
    int		num_matches;

    if (expand_wildcards(1, &compl_pattern, &num_matches, &matches,
		EW_FILE|EW_DIR|EW_ADDSLASH|EW_SILENT) != OK)
	return;

    // May change home directory back to "~".
    tilde_replace(compl_pattern, num_matches, matches);
#ifdef BACKSLASH_IN_FILENAME
    if (curbuf->b_p_csl[0] != NUL)
    {
	int	    i;

	for (i = 0; i < num_matches; ++i)
	{
	    char_u	*ptr = matches[i];

	    while (*ptr != NUL)
	    {
		if (curbuf->b_p_csl[0] == 's' && *ptr == '\\')
		    *ptr = '/';
		else if (curbuf->b_p_csl[0] == 'b' && *ptr == '/')
		    *ptr = '\\';
		ptr += (*mb_ptr2len)(ptr);
	    }
	}
    }
#endif
    ins_compl_add_matches(num_matches, matches, p_fic || p_wic);
}