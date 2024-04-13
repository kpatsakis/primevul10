ins_compl_addfrommatch(void)
{
    char_u	*p;
    int		len = (int)curwin->w_cursor.col - (int)compl_col;
    int		c;
    compl_T	*cp;

    p = compl_shown_match->cp_str;
    if ((int)STRLEN(p) <= len)   // the match is too short
    {
	// When still at the original match use the first entry that matches
	// the leader.
	if (!match_at_original_text(compl_shown_match))
	    return;

	p = NULL;
	for (cp = compl_shown_match->cp_next; cp != NULL
		&& !is_first_match(cp); cp = cp->cp_next)
	{
	    if (compl_leader == NULL
		    || ins_compl_equal(cp, compl_leader,
			(int)STRLEN(compl_leader)))
	    {
		p = cp->cp_str;
		break;
	    }
	}
	if (p == NULL || (int)STRLEN(p) <= len)
	    return;
    }
    p += len;
    c = PTR2CHAR(p);
    ins_compl_addleader(c);
}