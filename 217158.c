ins_compl_update_shown_match(void)
{
    while (!ins_compl_equal(compl_shown_match,
		compl_leader, (int)STRLEN(compl_leader))
	    && compl_shown_match->cp_next != NULL
	    && !is_first_match(compl_shown_match->cp_next))
	compl_shown_match = compl_shown_match->cp_next;

    // If we didn't find it searching forward, and compl_shows_dir is
    // backward, find the last match.
    if (compl_shows_dir_backward()
	    && !ins_compl_equal(compl_shown_match,
		compl_leader, (int)STRLEN(compl_leader))
	    && (compl_shown_match->cp_next == NULL
		|| is_first_match(compl_shown_match->cp_next)))
    {
	while (!ins_compl_equal(compl_shown_match,
		    compl_leader, (int)STRLEN(compl_leader))
		&& compl_shown_match->cp_prev != NULL
		&& !is_first_match(compl_shown_match->cp_prev))
	    compl_shown_match = compl_shown_match->cp_prev;
    }
}