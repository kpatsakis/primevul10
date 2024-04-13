get_next_cmdline_completion()
{
    char_u	**matches;
    int		num_matches;

    if (expand_cmdline(&compl_xp, compl_pattern,
		(int)STRLEN(compl_pattern),
		&num_matches, &matches) == EXPAND_OK)
	ins_compl_add_matches(num_matches, matches, FALSE);
}