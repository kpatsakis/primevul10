get_spell_compl_info(int startcol UNUSED, colnr_T curs_col UNUSED)
{
    int		ret = FAIL;
#ifdef FEAT_SPELL
    char_u	*line;

    if (spell_bad_len > 0)
	compl_col = curs_col - spell_bad_len;
    else
	compl_col = spell_word_start(startcol);
    if (compl_col >= (colnr_T)startcol)
    {
	compl_length = 0;
	compl_col = curs_col;
    }
    else
    {
	spell_expand_check_cap(compl_col);
	compl_length = (int)curs_col - compl_col;
    }
    // Need to obtain "line" again, it may have become invalid.
    line = ml_get(curwin->w_cursor.lnum);
    compl_pattern = vim_strnsave(line + compl_col, compl_length);
    if (compl_pattern == NULL)
	return FAIL;

    ret = OK;
#endif

    return ret;
}