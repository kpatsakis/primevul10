ex_argdelete(exarg_T *eap)
{
    int		i;
    int		n;

    if (check_arglist_locked() == FAIL)
	return;

    if (eap->addr_count > 0 || *eap->arg == NUL)
    {
	// ":argdel" works like ":.argdel"
	if (eap->addr_count == 0)
	{
	    if (curwin->w_arg_idx >= ARGCOUNT)
	    {
		emsg(_("E610: No argument to delete"));
		return;
	    }
	    eap->line1 = eap->line2 = curwin->w_arg_idx + 1;
	}
	else if (eap->line2 > ARGCOUNT)
	    // ":1,4argdel": Delete all arguments in the range.
	    eap->line2 = ARGCOUNT;
	n = eap->line2 - eap->line1 + 1;
	if (*eap->arg != NUL)
	    // Can't have both a range and an argument.
	    emsg(_(e_invarg));
	else if (n <= 0)
	{
	    // Don't give an error for ":%argdel" if the list is empty.
	    if (eap->line1 != 1 || eap->line2 != 0)
		emsg(_(e_invalid_range));
	}
	else
	{
	    for (i = eap->line1; i <= eap->line2; ++i)
		vim_free(ARGLIST[i - 1].ae_fname);
	    mch_memmove(ARGLIST + eap->line1 - 1, ARGLIST + eap->line2,
			(size_t)((ARGCOUNT - eap->line2) * sizeof(aentry_T)));
	    ALIST(curwin)->al_ga.ga_len -= n;
	    if (curwin->w_arg_idx >= eap->line2)
		curwin->w_arg_idx -= n;
	    else if (curwin->w_arg_idx > eap->line1)
		curwin->w_arg_idx = eap->line1;
	    if (ARGCOUNT == 0)
		curwin->w_arg_idx = 0;
	    else if (curwin->w_arg_idx >= ARGCOUNT)
		curwin->w_arg_idx = ARGCOUNT - 1;
	}
    }
    else
	do_arglist(eap->arg, AL_DEL, 0, FALSE);
    maketitle();
}