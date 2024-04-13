alist_set(
    alist_T	*al,
    int		count,
    char_u	**files,
    int		use_curbuf,
    int		*fnum_list,
    int		fnum_len)
{
    int		i;

    if (check_arglist_locked() == FAIL)
	return;

    alist_clear(al);
    if (GA_GROW_OK(&al->al_ga, count))
    {
	for (i = 0; i < count; ++i)
	{
	    if (got_int)
	    {
		// When adding many buffers this can take a long time.  Allow
		// interrupting here.
		while (i < count)
		    vim_free(files[i++]);
		break;
	    }

	    // May set buffer name of a buffer previously used for the
	    // argument list, so that it's re-used by alist_add.
	    if (fnum_list != NULL && i < fnum_len)
	    {
		arglist_locked = TRUE;
		buf_set_name(fnum_list[i], files[i]);
		arglist_locked = FALSE;
	    }

	    alist_add(al, files[i], use_curbuf ? 2 : 1);
	    ui_breakcheck();
	}
	vim_free(files);
    }
    else
	FreeWild(count, files);
    if (al == &global_alist)
	arg_had_last = FALSE;
}