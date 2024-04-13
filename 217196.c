ins_compl_bs(void)
{
    char_u	*line;
    char_u	*p;

    line = ml_get_curline();
    p = line + curwin->w_cursor.col;
    MB_PTR_BACK(line, p);

    // Stop completion when the whole word was deleted.  For Omni completion
    // allow the word to be deleted, we won't match everything.
    // Respect the 'backspace' option.
    if ((int)(p - line) - (int)compl_col < 0
	    || ((int)(p - line) - (int)compl_col == 0 && !ctrl_x_mode_omni())
	    || ctrl_x_mode_eval()
	    || (!can_bs(BS_START) && (int)(p - line) - (int)compl_col
							- compl_length < 0))
	return K_BS;

    // Deleted more than what was used to find matches or didn't finish
    // finding all matches: need to look for matches all over again.
    if (curwin->w_cursor.col <= compl_col + compl_length
						  || ins_compl_need_restart())
	ins_compl_restart();

    vim_free(compl_leader);
    compl_leader = vim_strnsave(line + compl_col, (p - line) - compl_col);
    if (compl_leader == NULL)
	return K_BS;

    ins_compl_new_leader();
    if (compl_shown_match != NULL)
	// Make sure current match is not a hidden item.
	compl_curr_match = compl_shown_match;
    return NUL;
}