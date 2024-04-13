unserialize_uhp(bufinfo_T *bi, char_u *file_name)
{
    u_header_T	*uhp;
    int		i;
    u_entry_T	*uep, *last_uep;
    int		c;
    int		error;

    uhp = (u_header_T *)U_ALLOC_LINE(sizeof(u_header_T));
    if (uhp == NULL)
	return NULL;
    vim_memset(uhp, 0, sizeof(u_header_T));
#ifdef U_DEBUG
    uhp->uh_magic = UH_MAGIC;
#endif
    uhp->uh_next.seq = undo_read_4c(bi);
    uhp->uh_prev.seq = undo_read_4c(bi);
    uhp->uh_alt_next.seq = undo_read_4c(bi);
    uhp->uh_alt_prev.seq = undo_read_4c(bi);
    uhp->uh_seq = undo_read_4c(bi);
    if (uhp->uh_seq <= 0)
    {
	corruption_error("uh_seq", file_name);
	vim_free(uhp);
	return NULL;
    }
    unserialize_pos(bi, &uhp->uh_cursor);
#ifdef FEAT_VIRTUALEDIT
    uhp->uh_cursor_vcol = undo_read_4c(bi);
#else
    (void)undo_read_4c(bi);
#endif
    uhp->uh_flags = undo_read_2c(bi);
    for (i = 0; i < NMARKS; ++i)
	unserialize_pos(bi, &uhp->uh_namedm[i]);
    unserialize_visualinfo(bi, &uhp->uh_visual);
    uhp->uh_time = undo_read_time(bi);

    /* Optional fields. */
    for (;;)
    {
	int len = undo_read_byte(bi);
	int what;

	if (len == 0)
	    break;
	what = undo_read_byte(bi);
	switch (what)
	{
	    case UHP_SAVE_NR:
		uhp->uh_save_nr = undo_read_4c(bi);
		break;
	    default:
		/* field not supported, skip */
		while (--len >= 0)
		    (void)undo_read_byte(bi);
	}
    }

    /* Unserialize the uep list. */
    last_uep = NULL;
    while ((c = undo_read_2c(bi)) == UF_ENTRY_MAGIC)
    {
	error = FALSE;
	uep = unserialize_uep(bi, &error, file_name);
	if (last_uep == NULL)
	    uhp->uh_entry = uep;
	else
	    last_uep->ue_next = uep;
	last_uep = uep;
	if (uep == NULL || error)
	{
	    u_free_uhp(uhp);
	    return NULL;
	}
    }
    if (c != UF_ENTRY_END_MAGIC)
    {
	corruption_error("entry end", file_name);
	u_free_uhp(uhp);
	return NULL;
    }

    return uhp;
}