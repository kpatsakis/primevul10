ins_compl_make_cyclic(void)
{
    compl_T *match;
    int	    count = 0;

    if (compl_first_match == NULL)
	return 0;

    // Find the end of the list.
    match = compl_first_match;
    // there's always an entry for the compl_orig_text, it doesn't count.
    while (match->cp_next != NULL && !is_first_match(match->cp_next))
    {
	match = match->cp_next;
	++count;
    }
    match->cp_next = compl_first_match;
    compl_first_match->cp_prev = match;

    return count;
}