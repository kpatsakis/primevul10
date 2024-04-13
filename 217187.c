find_next_completion_match(
	int	allow_get_expansion,
	int	todo,		// repeat completion this many times
	int	advance,
	int	*num_matches)
{
    int	    found_end = FALSE;
    compl_T *found_compl = NULL;

    while (--todo >= 0)
    {
	if (compl_shows_dir_forward() && compl_shown_match->cp_next != NULL)
	{
	    compl_shown_match = compl_shown_match->cp_next;
	    found_end = (compl_first_match != NULL
		    && (is_first_match(compl_shown_match->cp_next)
			|| is_first_match(compl_shown_match)));
	}
	else if (compl_shows_dir_backward()
		&& compl_shown_match->cp_prev != NULL)
	{
	    found_end = is_first_match(compl_shown_match);
	    compl_shown_match = compl_shown_match->cp_prev;
	    found_end |= is_first_match(compl_shown_match);
	}
	else
	{
	    if (!allow_get_expansion)
	    {
		if (advance)
		{
		    if (compl_shows_dir_backward())
			compl_pending -= todo + 1;
		    else
			compl_pending += todo + 1;
		}
		return -1;
	    }

	    if (!compl_no_select && advance)
	    {
		if (compl_shows_dir_backward())
		    --compl_pending;
		else
		    ++compl_pending;
	    }

	    // Find matches.
	    *num_matches = ins_compl_get_exp(&compl_startpos);

	    // handle any pending completions
	    while (compl_pending != 0 && compl_direction == compl_shows_dir
		    && advance)
	    {
		if (compl_pending > 0 && compl_shown_match->cp_next != NULL)
		{
		    compl_shown_match = compl_shown_match->cp_next;
		    --compl_pending;
		}
		if (compl_pending < 0 && compl_shown_match->cp_prev != NULL)
		{
		    compl_shown_match = compl_shown_match->cp_prev;
		    ++compl_pending;
		}
		else
		    break;
	    }
	    found_end = FALSE;
	}
	if (!match_at_original_text(compl_shown_match)
		&& compl_leader != NULL
		&& !ins_compl_equal(compl_shown_match,
		    compl_leader, (int)STRLEN(compl_leader)))
	    ++todo;
	else
	    // Remember a matching item.
	    found_compl = compl_shown_match;

	// Stop at the end of the list when we found a usable match.
	if (found_end)
	{
	    if (found_compl != NULL)
	    {
		compl_shown_match = found_compl;
		break;
	    }
	    todo = 1;	    // use first usable match after wrapping around
	}
    }

    return OK;
}