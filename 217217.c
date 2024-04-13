set_completion(colnr_T startcol, list_T *list)
{
    int save_w_wrow = curwin->w_wrow;
    int save_w_leftcol = curwin->w_leftcol;
    int flags = CP_ORIGINAL_TEXT;

    // If already doing completions stop it.
    if (ctrl_x_mode_not_default())
	ins_compl_prep(' ');
    ins_compl_clear();
    ins_compl_free();

    compl_direction = FORWARD;
    if (startcol > curwin->w_cursor.col)
	startcol = curwin->w_cursor.col;
    compl_col = startcol;
    compl_length = (int)curwin->w_cursor.col - (int)startcol;
    // compl_pattern doesn't need to be set
    compl_orig_text = vim_strnsave(ml_get_curline() + compl_col, compl_length);
    if (p_ic)
	flags |= CP_ICASE;
    if (compl_orig_text == NULL || ins_compl_add(compl_orig_text,
					      -1, NULL, NULL, NULL, 0,
					      flags | CP_FAST, FALSE) != OK)
	return;

    ctrl_x_mode = CTRL_X_EVAL;

    ins_compl_add_list(list);
    compl_matches = ins_compl_make_cyclic();
    compl_started = TRUE;
    compl_used_match = TRUE;
    compl_cont_status = 0;

    compl_curr_match = compl_first_match;
    if (compl_no_insert || compl_no_select)
    {
	ins_complete(K_DOWN, FALSE);
	if (compl_no_select)
	    // Down/Up has no real effect.
	    ins_complete(K_UP, FALSE);
    }
    else
	ins_complete(Ctrl_N, FALSE);
    compl_enter_selects = compl_no_insert;

    // Lazily show the popup menu, unless we got interrupted.
    if (!compl_interrupted)
	show_pum(save_w_wrow, save_w_leftcol);
    may_trigger_modechanged();
    out_flush();
}