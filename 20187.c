alist_add_list(
    int		count,
    char_u	**files,
    int		after,	    // where to add: 0 = before first one
    int		will_edit)  // will edit adding argument
{
    int		i;
    int		old_argcount = ARGCOUNT;

    if (check_arglist_locked() != FAIL
	    && GA_GROW_OK(&ALIST(curwin)->al_ga, count))
    {
	if (after < 0)
	    after = 0;
	if (after > ARGCOUNT)
	    after = ARGCOUNT;
	if (after < ARGCOUNT)
	    mch_memmove(&(ARGLIST[after + count]), &(ARGLIST[after]),
				       (ARGCOUNT - after) * sizeof(aentry_T));
	arglist_locked = TRUE;
	for (i = 0; i < count; ++i)
	{
	    int flags = BLN_LISTED | (will_edit ? BLN_CURBUF : 0);

	    ARGLIST[after + i].ae_fname = files[i];
	    ARGLIST[after + i].ae_fnum = buflist_add(files[i], flags);
	}
	arglist_locked = FALSE;
	ALIST(curwin)->al_ga.ga_len += count;
	if (old_argcount > 0 && curwin->w_arg_idx >= after)
	    curwin->w_arg_idx += count;
	return;
    }

    for (i = 0; i < count; ++i)
	vim_free(files[i]);
}