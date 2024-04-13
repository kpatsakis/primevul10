get_next_spell_completion(linenr_T lnum UNUSED)
{
#ifdef FEAT_SPELL
    char_u	**matches;
    int		num_matches;

    num_matches = expand_spelling(lnum, compl_pattern, &matches);
    if (num_matches > 0)
	ins_compl_add_matches(num_matches, matches, p_ic);
    else
	vim_free(matches);
#endif
}