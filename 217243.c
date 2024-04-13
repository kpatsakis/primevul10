ins_compl_longest_match(compl_T *match)
{
    char_u	*p, *s;
    int		c1, c2;
    int		had_match;

    if (compl_leader == NULL)
    {
	// First match, use it as a whole.
	compl_leader = vim_strsave(match->cp_str);
	if (compl_leader == NULL)
	    return;

	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_delete();
	ins_bytes(compl_leader + get_compl_len());
	ins_redraw(FALSE);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	if (!had_match)
	    ins_compl_delete();
	compl_used_match = FALSE;

	return;
    }

    // Reduce the text if this match differs from compl_leader.
    p = compl_leader;
    s = match->cp_str;
    while (*p != NUL)
    {
	if (has_mbyte)
	{
	    c1 = mb_ptr2char(p);
	    c2 = mb_ptr2char(s);
	}
	else
	{
	    c1 = *p;
	    c2 = *s;
	}
	if ((match->cp_flags & CP_ICASE)
		? (MB_TOLOWER(c1) != MB_TOLOWER(c2)) : (c1 != c2))
	    break;
	if (has_mbyte)
	{
	    MB_PTR_ADV(p);
	    MB_PTR_ADV(s);
	}
	else
	{
	    ++p;
	    ++s;
	}
    }

    if (*p != NUL)
    {
	// Leader was shortened, need to change the inserted text.
	*p = NUL;
	had_match = (curwin->w_cursor.col > compl_col);
	ins_compl_delete();
	ins_bytes(compl_leader + get_compl_len());
	ins_redraw(FALSE);

	// When the match isn't there (to avoid matching itself) remove it
	// again after redrawing.
	if (!had_match)
	    ins_compl_delete();
    }

    compl_used_match = FALSE;
}