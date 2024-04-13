ins_compl_start(void)
{
    char_u	*line;
    int		startcol = 0;	    // column where searched text starts
    colnr_T	curs_col;	    // cursor column
    int		line_invalid = FALSE;
    int		save_did_ai = did_ai;
    int		flags = CP_ORIGINAL_TEXT;

    // First time we hit ^N or ^P (in a row, I mean)

    did_ai = FALSE;
    did_si = FALSE;
    can_si = FALSE;
    can_si_back = FALSE;
    if (stop_arrow() == FAIL)
	return FAIL;

    line = ml_get(curwin->w_cursor.lnum);
    curs_col = curwin->w_cursor.col;
    compl_pending = 0;

    if ((compl_cont_status & CONT_INTRPT) == CONT_INTRPT
	    && compl_cont_mode == ctrl_x_mode)
	// this same ctrl-x_mode was interrupted previously. Continue the
	// completion.
	ins_compl_continue_search(line);
    else
	compl_cont_status &= CONT_LOCAL;

    if (!compl_status_adding())	// normal expansion
    {
	compl_cont_mode = ctrl_x_mode;
	if (ctrl_x_mode_not_default())
	    // Remove LOCAL if ctrl_x_mode != CTRL_X_NORMAL
	    compl_cont_status = 0;
	compl_cont_status |= CONT_N_ADDS;
	compl_startpos = curwin->w_cursor;
	startcol = (int)curs_col;
	compl_col = 0;
    }

    // Work out completion pattern and original text -- webb
    if (compl_get_info(line, startcol, curs_col, &line_invalid) == FAIL)
    {
	if (ctrl_x_mode_function() || ctrl_x_mode_omni()
				|| thesaurus_func_complete(ctrl_x_mode))
	    // restore did_ai, so that adding comment leader works
	    did_ai = save_did_ai;
	return FAIL;
    }
    // If "line" was changed while getting completion info get it again.
    if (line_invalid)
	line = ml_get(curwin->w_cursor.lnum);

    if (compl_status_adding())
    {
	edit_submode_pre = (char_u *)_(" Adding");
	if (ctrl_x_mode_line_or_eval())
	{
	    // Insert a new line, keep indentation but ignore 'comments'.
	    char_u *old = curbuf->b_p_com;

	    curbuf->b_p_com = (char_u *)"";
	    compl_startpos.lnum = curwin->w_cursor.lnum;
	    compl_startpos.col = compl_col;
	    ins_eol('\r');
	    curbuf->b_p_com = old;
	    compl_length = 0;
	    compl_col = curwin->w_cursor.col;
	}
    }
    else
    {
	edit_submode_pre = NULL;
	compl_startpos.col = compl_col;
    }

    if (compl_cont_status & CONT_LOCAL)
	edit_submode = (char_u *)_(ctrl_x_msgs[CTRL_X_LOCAL_MSG]);
    else
	edit_submode = (char_u *)_(CTRL_X_MSG(ctrl_x_mode));

    // If any of the original typed text has been changed we need to fix
    // the redo buffer.
    ins_compl_fixRedoBufForLeader(NULL);

    // Always add completion for the original text.
    vim_free(compl_orig_text);
    compl_orig_text = vim_strnsave(line + compl_col, compl_length);
    if (p_ic)
	flags |= CP_ICASE;
    if (compl_orig_text == NULL || ins_compl_add(compl_orig_text,
		-1, NULL, NULL, NULL, 0, flags, FALSE) != OK)
    {
	VIM_CLEAR(compl_pattern);
	VIM_CLEAR(compl_orig_text);
	return FAIL;
    }

    // showmode might reset the internal line pointers, so it must
    // be called before line = ml_get(), or when this address is no
    // longer needed.  -- Acevedo.
    edit_submode_extra = (char_u *)_("-- Searching...");
    edit_submode_highl = HLF_COUNT;
    showmode();
    edit_submode_extra = NULL;
    out_flush();

    return OK;
}