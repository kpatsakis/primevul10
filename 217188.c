get_wholeline_compl_info(char_u *line, colnr_T curs_col)
{
    compl_col = (colnr_T)getwhitecols(line);
    compl_length = (int)curs_col - (int)compl_col;
    if (compl_length < 0)	// cursor in indent: empty pattern
	compl_length = 0;
    if (p_ic)
	compl_pattern = str_foldcase(line + compl_col, compl_length,
		NULL, 0);
    else
	compl_pattern = vim_strnsave(line + compl_col, compl_length);
    if (compl_pattern == NULL)
	return FAIL;

    return OK;
}