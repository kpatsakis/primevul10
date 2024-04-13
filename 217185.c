ins_compl_addleader(int c)
{
    int		cc;

    if (stop_arrow() == FAIL)
	return;
    if (has_mbyte && (cc = (*mb_char2len)(c)) > 1)
    {
	char_u	buf[MB_MAXBYTES + 1];

	(*mb_char2bytes)(c, buf);
	buf[cc] = NUL;
	ins_char_bytes(buf, cc);
	if (compl_opt_refresh_always)
	    AppendToRedobuff(buf);
    }
    else
    {
	ins_char(c);
	if (compl_opt_refresh_always)
	    AppendCharToRedobuff(c);
    }

    // If we didn't complete finding matches we must search again.
    if (ins_compl_need_restart())
	ins_compl_restart();

    // When 'always' is set, don't reset compl_leader. While completing,
    // cursor doesn't point original position, changing compl_leader would
    // break redo.
    if (!compl_opt_refresh_always)
    {
	vim_free(compl_leader);
	compl_leader = vim_strnsave(ml_get_curline() + compl_col,
					     curwin->w_cursor.col - compl_col);
	if (compl_leader != NULL)
	    ins_compl_new_leader();
    }
}