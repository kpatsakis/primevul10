u_savecommon(
    linenr_T	top,
    linenr_T	bot,
    linenr_T	newbot,
    int		reload)
{
    linenr_T	lnum;
    long	i;
    u_header_T	*uhp;
    u_header_T	*old_curhead;
    u_entry_T	*uep;
    u_entry_T	*prev_uep;
    long	size;

    if (!reload)
    {
	/* When making changes is not allowed return FAIL.  It's a crude way
	 * to make all change commands fail. */
	if (!undo_allowed())
	    return FAIL;

#ifdef FEAT_NETBEANS_INTG
	/*
	 * Netbeans defines areas that cannot be modified.  Bail out here when
	 * trying to change text in a guarded area.
	 */
	if (netbeans_active())
	{
	    if (netbeans_is_guarded(top, bot))
	    {
		EMSG(_(e_guarded));
		return FAIL;
	    }
	    if (curbuf->b_p_ro)
	    {
		EMSG(_(e_nbreadonly));
		return FAIL;
	    }
	}
#endif

#ifdef FEAT_AUTOCMD
	/*
	 * Saving text for undo means we are going to make a change.  Give a
	 * warning for a read-only file before making the change, so that the
	 * FileChangedRO event can replace the buffer with a read-write version
	 * (e.g., obtained from a source control system).
	 */
	change_warning(0);
	if (bot > curbuf->b_ml.ml_line_count + 1)
	{
	    /* This happens when the FileChangedRO autocommand changes the
	     * file in a way it becomes shorter. */
	    EMSG(_("E881: Line count changed unexpectedly"));
	    return FAIL;
	}
#endif
    }

#ifdef U_DEBUG
    u_check(FALSE);
#endif

    size = bot - top - 1;

    /*
     * If curbuf->b_u_synced == TRUE make a new header.
     */
    if (curbuf->b_u_synced)
    {
#ifdef FEAT_JUMPLIST
	/* Need to create new entry in b_changelist. */
	curbuf->b_new_change = TRUE;
#endif

	if (get_undolevel() >= 0)
	{
	    /*
	     * Make a new header entry.  Do this first so that we don't mess
	     * up the undo info when out of memory.
	     */
	    uhp = (u_header_T *)U_ALLOC_LINE(sizeof(u_header_T));
	    if (uhp == NULL)
		goto nomem;
#ifdef U_DEBUG
	    uhp->uh_magic = UH_MAGIC;
#endif
	}
	else
	    uhp = NULL;

	/*
	 * If we undid more than we redid, move the entry lists before and
	 * including curbuf->b_u_curhead to an alternate branch.
	 */
	old_curhead = curbuf->b_u_curhead;
	if (old_curhead != NULL)
	{
	    curbuf->b_u_newhead = old_curhead->uh_next.ptr;
	    curbuf->b_u_curhead = NULL;
	}

	/*
	 * free headers to keep the size right
	 */
	while (curbuf->b_u_numhead > get_undolevel()
					       && curbuf->b_u_oldhead != NULL)
	{
	    u_header_T	    *uhfree = curbuf->b_u_oldhead;

	    if (uhfree == old_curhead)
		/* Can't reconnect the branch, delete all of it. */
		u_freebranch(curbuf, uhfree, &old_curhead);
	    else if (uhfree->uh_alt_next.ptr == NULL)
		/* There is no branch, only free one header. */
		u_freeheader(curbuf, uhfree, &old_curhead);
	    else
	    {
		/* Free the oldest alternate branch as a whole. */
		while (uhfree->uh_alt_next.ptr != NULL)
		    uhfree = uhfree->uh_alt_next.ptr;
		u_freebranch(curbuf, uhfree, &old_curhead);
	    }
#ifdef U_DEBUG
	    u_check(TRUE);
#endif
	}

	if (uhp == NULL)		/* no undo at all */
	{
	    if (old_curhead != NULL)
		u_freebranch(curbuf, old_curhead, NULL);
	    curbuf->b_u_synced = FALSE;
	    return OK;
	}

	uhp->uh_prev.ptr = NULL;
	uhp->uh_next.ptr = curbuf->b_u_newhead;
	uhp->uh_alt_next.ptr = old_curhead;
	if (old_curhead != NULL)
	{
	    uhp->uh_alt_prev.ptr = old_curhead->uh_alt_prev.ptr;
	    if (uhp->uh_alt_prev.ptr != NULL)
		uhp->uh_alt_prev.ptr->uh_alt_next.ptr = uhp;
	    old_curhead->uh_alt_prev.ptr = uhp;
	    if (curbuf->b_u_oldhead == old_curhead)
		curbuf->b_u_oldhead = uhp;
	}
	else
	    uhp->uh_alt_prev.ptr = NULL;
	if (curbuf->b_u_newhead != NULL)
	    curbuf->b_u_newhead->uh_prev.ptr = uhp;

	uhp->uh_seq = ++curbuf->b_u_seq_last;
	curbuf->b_u_seq_cur = uhp->uh_seq;
	uhp->uh_time = vim_time();
	uhp->uh_save_nr = 0;
	curbuf->b_u_time_cur = uhp->uh_time + 1;

	uhp->uh_walk = 0;
	uhp->uh_entry = NULL;
	uhp->uh_getbot_entry = NULL;
	uhp->uh_cursor = curwin->w_cursor;	/* save cursor pos. for undo */
#ifdef FEAT_VIRTUALEDIT
	if (virtual_active() && curwin->w_cursor.coladd > 0)
	    uhp->uh_cursor_vcol = getviscol();
	else
	    uhp->uh_cursor_vcol = -1;
#endif

	/* save changed and buffer empty flag for undo */
	uhp->uh_flags = (curbuf->b_changed ? UH_CHANGED : 0) +
		       ((curbuf->b_ml.ml_flags & ML_EMPTY) ? UH_EMPTYBUF : 0);

	/* save named marks and Visual marks for undo */
	mch_memmove(uhp->uh_namedm, curbuf->b_namedm, sizeof(pos_T) * NMARKS);
	uhp->uh_visual = curbuf->b_visual;

	curbuf->b_u_newhead = uhp;
	if (curbuf->b_u_oldhead == NULL)
	    curbuf->b_u_oldhead = uhp;
	++curbuf->b_u_numhead;
    }
    else
    {
	if (get_undolevel() < 0)	/* no undo at all */
	    return OK;

	/*
	 * When saving a single line, and it has been saved just before, it
	 * doesn't make sense saving it again.  Saves a lot of memory when
	 * making lots of changes inside the same line.
	 * This is only possible if the previous change didn't increase or
	 * decrease the number of lines.
	 * Check the ten last changes.  More doesn't make sense and takes too
	 * long.
	 */
	if (size == 1)
	{
	    uep = u_get_headentry();
	    prev_uep = NULL;
	    for (i = 0; i < 10; ++i)
	    {
		if (uep == NULL)
		    break;

		/* If lines have been inserted/deleted we give up.
		 * Also when the line was included in a multi-line save. */
		if ((curbuf->b_u_newhead->uh_getbot_entry != uep
			    ? (uep->ue_top + uep->ue_size + 1
				!= (uep->ue_bot == 0
				    ? curbuf->b_ml.ml_line_count + 1
				    : uep->ue_bot))
			    : uep->ue_lcount != curbuf->b_ml.ml_line_count)
			|| (uep->ue_size > 1
			    && top >= uep->ue_top
			    && top + 2 <= uep->ue_top + uep->ue_size + 1))
		    break;

		/* If it's the same line we can skip saving it again. */
		if (uep->ue_size == 1 && uep->ue_top == top)
		{
		    if (i > 0)
		    {
			/* It's not the last entry: get ue_bot for the last
			 * entry now.  Following deleted/inserted lines go to
			 * the re-used entry. */
			u_getbot();
			curbuf->b_u_synced = FALSE;

			/* Move the found entry to become the last entry.  The
			 * order of undo/redo doesn't matter for the entries
			 * we move it over, since they don't change the line
			 * count and don't include this line.  It does matter
			 * for the found entry if the line count is changed by
			 * the executed command. */
			prev_uep->ue_next = uep->ue_next;
			uep->ue_next = curbuf->b_u_newhead->uh_entry;
			curbuf->b_u_newhead->uh_entry = uep;
		    }

		    /* The executed command may change the line count. */
		    if (newbot != 0)
			uep->ue_bot = newbot;
		    else if (bot > curbuf->b_ml.ml_line_count)
			uep->ue_bot = 0;
		    else
		    {
			uep->ue_lcount = curbuf->b_ml.ml_line_count;
			curbuf->b_u_newhead->uh_getbot_entry = uep;
		    }
		    return OK;
		}
		prev_uep = uep;
		uep = uep->ue_next;
	    }
	}

	/* find line number for ue_bot for previous u_save() */
	u_getbot();
    }

#if !defined(UNIX) && !defined(WIN32)
	/*
	 * With Amiga we can't handle big undo's, because
	 * then u_alloc_line would have to allocate a block larger than 32K
	 */
    if (size >= 8000)
	goto nomem;
#endif

    /*
     * add lines in front of entry list
     */
    uep = (u_entry_T *)U_ALLOC_LINE(sizeof(u_entry_T));
    if (uep == NULL)
	goto nomem;
    vim_memset(uep, 0, sizeof(u_entry_T));
#ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
#endif

    uep->ue_size = size;
    uep->ue_top = top;
    if (newbot != 0)
	uep->ue_bot = newbot;
    /*
     * Use 0 for ue_bot if bot is below last line.
     * Otherwise we have to compute ue_bot later.
     */
    else if (bot > curbuf->b_ml.ml_line_count)
	uep->ue_bot = 0;
    else
    {
	uep->ue_lcount = curbuf->b_ml.ml_line_count;
	curbuf->b_u_newhead->uh_getbot_entry = uep;
    }

    if (size > 0)
    {
	if ((uep->ue_array = (char_u **)U_ALLOC_LINE(
					    sizeof(char_u *) * size)) == NULL)
	{
	    u_freeentry(uep, 0L);
	    goto nomem;
	}
	for (i = 0, lnum = top + 1; i < size; ++i)
	{
	    fast_breakcheck();
	    if (got_int)
	    {
		u_freeentry(uep, i);
		return FAIL;
	    }
	    if ((uep->ue_array[i] = u_save_line(lnum++)) == NULL)
	    {
		u_freeentry(uep, i);
		goto nomem;
	    }
	}
    }
    else
	uep->ue_array = NULL;
    uep->ue_next = curbuf->b_u_newhead->uh_entry;
    curbuf->b_u_newhead->uh_entry = uep;
    curbuf->b_u_synced = FALSE;
    undo_undoes = FALSE;

#ifdef U_DEBUG
    u_check(FALSE);
#endif
    return OK;

nomem:
    msg_silent = 0;	/* must display the prompt */
    if (ask_yesno((char_u *)_("No undo possible; continue anyway"), TRUE)
								       == 'y')
    {
	undo_off = TRUE;	    /* will be reset when character typed */
	return OK;
    }
    do_outofmem_msg((long_u)0);
    return FAIL;
}