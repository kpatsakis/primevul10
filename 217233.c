get_normal_compl_info(char_u *line, int startcol, colnr_T curs_col)
{
    if ((compl_cont_status & CONT_SOL) || ctrl_x_mode_path_defines())
    {
	if (!compl_status_adding())
	{
	    while (--startcol >= 0 && vim_isIDc(line[startcol]))
		;
	    compl_col += ++startcol;
	    compl_length = curs_col - startcol;
	}
	if (p_ic)
	    compl_pattern = str_foldcase(line + compl_col,
		    compl_length, NULL, 0);
	else
	    compl_pattern = vim_strnsave(line + compl_col, compl_length);
	if (compl_pattern == NULL)
	    return FAIL;
    }
    else if (compl_status_adding())
    {
	char_u	    *prefix = (char_u *)"\\<";

	// we need up to 2 extra chars for the prefix
	compl_pattern = alloc(quote_meta(NULL, line + compl_col,
		    compl_length) + 2);
	if (compl_pattern == NULL)
	    return FAIL;
	if (!vim_iswordp(line + compl_col)
		|| (compl_col > 0
		    && (vim_iswordp(mb_prevptr(line, line + compl_col)))))
	    prefix = (char_u *)"";
	STRCPY((char *)compl_pattern, prefix);
	(void)quote_meta(compl_pattern + STRLEN(prefix),
		line + compl_col, compl_length);
    }
    else if (--startcol < 0
	    || !vim_iswordp(mb_prevptr(line, line + startcol + 1)))
    {
	// Match any word of at least two chars
	compl_pattern = vim_strsave((char_u *)"\\<\\k\\k");
	if (compl_pattern == NULL)
	    return FAIL;
	compl_col += curs_col;
	compl_length = 0;
    }
    else
    {
	// Search the point of change class of multibyte character
	// or not a word single byte character backward.
	if (has_mbyte)
	{
	    int base_class;
	    int head_off;

	    startcol -= (*mb_head_off)(line, line + startcol);
	    base_class = mb_get_class(line + startcol);
	    while (--startcol >= 0)
	    {
		head_off = (*mb_head_off)(line, line + startcol);
		if (base_class != mb_get_class(line + startcol
			    - head_off))
		    break;
		startcol -= head_off;
	    }
	}
	else
	    while (--startcol >= 0 && vim_iswordc(line[startcol]))
		;
	compl_col += ++startcol;
	compl_length = (int)curs_col - startcol;
	if (compl_length == 1)
	{
	    // Only match word with at least two chars -- webb
	    // there's no need to call quote_meta,
	    // alloc(7) is enough  -- Acevedo
	    compl_pattern = alloc(7);
	    if (compl_pattern == NULL)
		return FAIL;
	    STRCPY((char *)compl_pattern, "\\<");
	    (void)quote_meta(compl_pattern + 2, line + compl_col, 1);
	    STRCAT((char *)compl_pattern, "\\k");
	}
	else
	{
	    compl_pattern = alloc(quote_meta(NULL, line + compl_col,
			compl_length) + 2);
	    if (compl_pattern == NULL)
		return FAIL;
	    STRCPY((char *)compl_pattern, "\\<");
	    (void)quote_meta(compl_pattern + 2, line + compl_col,
		    compl_length);
	}
    }

    return OK;
}