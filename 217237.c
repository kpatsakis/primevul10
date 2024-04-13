get_cmdline_compl_info(char_u *line, colnr_T curs_col)
{
    compl_pattern = vim_strnsave(line, curs_col);
    if (compl_pattern == NULL)
	return FAIL;
    set_cmd_context(&compl_xp, compl_pattern,
	    (int)STRLEN(compl_pattern), curs_col, FALSE);
    if (compl_xp.xp_context == EXPAND_UNSUCCESSFUL
	    || compl_xp.xp_context == EXPAND_NOTHING)
	// No completion possible, use an empty pattern to get a
	// "pattern not found" message.
	compl_col = curs_col;
    else
	compl_col = (int)(compl_xp.xp_pattern - compl_pattern);
    compl_length = curs_col - compl_col;

    return OK;
}