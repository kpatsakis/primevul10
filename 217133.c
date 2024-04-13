get_filename_compl_info(char_u *line, int startcol, colnr_T curs_col)
{
    // Go back to just before the first filename character.
    if (startcol > 0)
    {
	char_u	*p = line + startcol;

	MB_PTR_BACK(line, p);
	while (p > line && vim_isfilec(PTR2CHAR(p)))
	    MB_PTR_BACK(line, p);
	if (p == line && vim_isfilec(PTR2CHAR(p)))
	    startcol = 0;
	else
	    startcol = (int)(p - line) + 1;
    }

    compl_col += startcol;
    compl_length = (int)curs_col - startcol;
    compl_pattern = addstar(line + compl_col, compl_length, EXPAND_FILES);
    if (compl_pattern == NULL)
	return FAIL;

    return OK;
}