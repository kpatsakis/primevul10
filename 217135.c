compl_get_info(char_u *line, int startcol, colnr_T curs_col, int *line_invalid)
{
    if (ctrl_x_mode_normal()
	    || (ctrl_x_mode & CTRL_X_WANT_IDENT
		&& !thesaurus_func_complete(ctrl_x_mode)))
    {
	return get_normal_compl_info(line, startcol, curs_col);
    }
    else if (ctrl_x_mode_line_or_eval())
    {
	return get_wholeline_compl_info(line, curs_col);
    }
    else if (ctrl_x_mode_files())
    {
	return get_filename_compl_info(line, startcol, curs_col);
    }
    else if (ctrl_x_mode == CTRL_X_CMDLINE)
    {
	return get_cmdline_compl_info(line, curs_col);
    }
    else if (ctrl_x_mode_function() || ctrl_x_mode_omni()
	    || thesaurus_func_complete(ctrl_x_mode))
    {
	if (get_userdefined_compl_info(curs_col) == FAIL)
	    return FAIL;
	*line_invalid = TRUE;	// "line" may have become invalid
    }
    else if (ctrl_x_mode_spell())
    {
	if (get_spell_compl_info(startcol, curs_col) == FAIL)
	    return FAIL;
	*line_invalid = TRUE;	// "line" may have become invalid
    }
    else
    {
	internal_error("ins_complete()");
	return FAIL;
    }

    return OK;
}