ins_complete(int c, int enable_pum)
{
    int		n;
    int		save_w_wrow;
    int		save_w_leftcol;
    int		insert_match;

    compl_direction = ins_compl_key2dir(c);
    insert_match = ins_compl_use_match(c);

    if (!compl_started)
    {
	if (ins_compl_start() == FAIL)
	    return FAIL;
    }
    else if (insert_match && stop_arrow() == FAIL)
	return FAIL;

    compl_shown_match = compl_curr_match;
    compl_shows_dir = compl_direction;

    // Find next match (and following matches).
    save_w_wrow = curwin->w_wrow;
    save_w_leftcol = curwin->w_leftcol;
    n = ins_compl_next(TRUE, ins_compl_key2count(c), insert_match, FALSE);

    // may undisplay the popup menu
    ins_compl_upd_pum();

    if (n > 1)		// all matches have been found
	compl_matches = n;
    compl_curr_match = compl_shown_match;
    compl_direction = compl_shows_dir;

    // Eat the ESC that vgetc() returns after a CTRL-C to avoid leaving Insert
    // mode.
    if (got_int && !global_busy)
    {
	(void)vgetc();
	got_int = FALSE;
    }

    // we found no match if the list has only the "compl_orig_text"-entry
    if (is_first_match(compl_first_match->cp_next))
    {
	// remove N_ADDS flag, so next ^X<> won't try to go to ADDING mode,
	// because we couldn't expand anything at first place, but if we used
	// ^P, ^N, ^X^I or ^X^D we might want to add-expand a single-char-word
	// (such as M in M'exico) if not tried already.  -- Acevedo
	if (compl_length > 1
		|| compl_status_adding()
		|| (ctrl_x_mode_not_default()
		    && !ctrl_x_mode_path_patterns()
		    && !ctrl_x_mode_path_defines()))
	    compl_cont_status &= ~CONT_N_ADDS;
    }

    if (compl_curr_match->cp_flags & CP_CONT_S_IPOS)
	compl_cont_status |= CONT_S_IPOS;
    else
	compl_cont_status &= ~CONT_S_IPOS;

    ins_compl_show_statusmsg();

    // Show the popup menu, unless we got interrupted.
    if (enable_pum && !compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);

    compl_was_interrupted = compl_interrupted;
    compl_interrupted = FALSE;

    return OK;
}