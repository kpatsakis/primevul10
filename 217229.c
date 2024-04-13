ins_compl_update_sequence_numbers()
{
    int		number = 0;
    compl_T	*match;

    if (compl_dir_forward())
    {
	// search backwards for the first valid (!= -1) number.
	// This should normally succeed already at the first loop
	// cycle, so it's fast!
	for (match = compl_curr_match->cp_prev; match != NULL
		&& !is_first_match(match); match = match->cp_prev)
	    if (match->cp_number != -1)
	    {
		number = match->cp_number;
		break;
	    }
	if (match != NULL)
	    // go up and assign all numbers which are not assigned
	    // yet
	    for (match = match->cp_next;
		    match != NULL && match->cp_number == -1;
					   match = match->cp_next)
		match->cp_number = ++number;
    }
    else // BACKWARD
    {
	// search forwards (upwards) for the first valid (!= -1)
	// number.  This should normally succeed already at the
	// first loop cycle, so it's fast!
	for (match = compl_curr_match->cp_next; match != NULL
		&& !is_first_match(match); match = match->cp_next)
	    if (match->cp_number != -1)
	    {
		number = match->cp_number;
		break;
	    }
	if (match != NULL)
	    // go down and assign all numbers which are not
	    // assigned yet
	    for (match = match->cp_prev; match
		    && match->cp_number == -1;
					   match = match->cp_prev)
		match->cp_number = ++number;
    }
}