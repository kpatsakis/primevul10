u_undoredo(int undo)
{
    char_u	**newarray = NULL;
    linenr_T	oldsize;
    linenr_T	newsize;
    linenr_T	top, bot;
    linenr_T	lnum;
    linenr_T	newlnum = MAXLNUM;
    long	i;
    u_entry_T	*uep, *nuep;
    u_entry_T	*newlist = NULL;
    int		old_flags;
    int		new_flags;
    pos_T	namedm[NMARKS];
    visualinfo_T visualinfo;
    int		empty_buffer;		    /* buffer became empty */
    u_header_T	*curhead = curbuf->b_u_curhead;

#ifdef FEAT_AUTOCMD
    /* Don't want autocommands using the undo structures here, they are
     * invalid till the end. */
    block_autocmds();
#endif

#ifdef U_DEBUG
    u_check(FALSE);
#endif
    old_flags = curhead->uh_flags;
    new_flags = (curbuf->b_changed ? UH_CHANGED : 0) +
	       ((curbuf->b_ml.ml_flags & ML_EMPTY) ? UH_EMPTYBUF : 0);
    setpcmark();

    /*
     * save marks before undo/redo
     */
    mch_memmove(namedm, curbuf->b_namedm, sizeof(pos_T) * NMARKS);
    visualinfo = curbuf->b_visual;
    curbuf->b_op_start.lnum = curbuf->b_ml.ml_line_count;
    curbuf->b_op_start.col = 0;
    curbuf->b_op_end.lnum = 0;
    curbuf->b_op_end.col = 0;

    for (uep = curhead->uh_entry; uep != NULL; uep = nuep)
    {
	top = uep->ue_top;
	bot = uep->ue_bot;
	if (bot == 0)
	    bot = curbuf->b_ml.ml_line_count + 1;
	if (top > curbuf->b_ml.ml_line_count || top >= bot
				      || bot > curbuf->b_ml.ml_line_count + 1)
	{
#ifdef FEAT_AUTOCMD
	    unblock_autocmds();
#endif
	    IEMSG(_("E438: u_undo: line numbers wrong"));
	    changed();		/* don't want UNCHANGED now */
	    return;
	}

	oldsize = bot - top - 1;    /* number of lines before undo */
	newsize = uep->ue_size;	    /* number of lines after undo */

	if (top < newlnum)
	{
	    /* If the saved cursor is somewhere in this undo block, move it to
	     * the remembered position.  Makes "gwap" put the cursor back
	     * where it was. */
	    lnum = curhead->uh_cursor.lnum;
	    if (lnum >= top && lnum <= top + newsize + 1)
	    {
		curwin->w_cursor = curhead->uh_cursor;
		newlnum = curwin->w_cursor.lnum - 1;
	    }
	    else
	    {
		/* Use the first line that actually changed.  Avoids that
		 * undoing auto-formatting puts the cursor in the previous
		 * line. */
		for (i = 0; i < newsize && i < oldsize; ++i)
		    if (STRCMP(uep->ue_array[i], ml_get(top + 1 + i)) != 0)
			break;
		if (i == newsize && newlnum == MAXLNUM && uep->ue_next == NULL)
		{
		    newlnum = top;
		    curwin->w_cursor.lnum = newlnum + 1;
		}
		else if (i < newsize)
		{
		    newlnum = top + i;
		    curwin->w_cursor.lnum = newlnum + 1;
		}
	    }
	}

	empty_buffer = FALSE;

	/* delete the lines between top and bot and save them in newarray */
	if (oldsize > 0)
	{
	    if ((newarray = (char_u **)U_ALLOC_LINE(
					 sizeof(char_u *) * oldsize)) == NULL)
	    {
		do_outofmem_msg((long_u)(sizeof(char_u *) * oldsize));
		/*
		 * We have messed up the entry list, repair is impossible.
		 * we have to free the rest of the list.
		 */
		while (uep != NULL)
		{
		    nuep = uep->ue_next;
		    u_freeentry(uep, uep->ue_size);
		    uep = nuep;
		}
		break;
	    }
	    /* delete backwards, it goes faster in most cases */
	    for (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
	    {
		/* what can we do when we run out of memory? */
		if ((newarray[i] = u_save_line(lnum)) == NULL)
		    do_outofmem_msg((long_u)0);
		/* remember we deleted the last line in the buffer, and a
		 * dummy empty line will be inserted */
		if (curbuf->b_ml.ml_line_count == 1)
		    empty_buffer = TRUE;
		ml_delete(lnum, FALSE);
	    }
	}
	else
	    newarray = NULL;

	/* insert the lines in u_array between top and bot */
	if (newsize)
	{
	    for (lnum = top, i = 0; i < newsize; ++i, ++lnum)
	    {
		/*
		 * If the file is empty, there is an empty line 1 that we
		 * should get rid of, by replacing it with the new line
		 */
		if (empty_buffer && lnum == 0)
		    ml_replace((linenr_T)1, uep->ue_array[i], TRUE);
		else
		    ml_append(lnum, uep->ue_array[i], (colnr_T)0, FALSE);
		vim_free(uep->ue_array[i]);
	    }
	    vim_free((char_u *)uep->ue_array);
	}

	/* adjust marks */
	if (oldsize != newsize)
	{
	    mark_adjust(top + 1, top + oldsize, (long)MAXLNUM,
					       (long)newsize - (long)oldsize);
	    if (curbuf->b_op_start.lnum > top + oldsize)
		curbuf->b_op_start.lnum += newsize - oldsize;
	    if (curbuf->b_op_end.lnum > top + oldsize)
		curbuf->b_op_end.lnum += newsize - oldsize;
	}

	changed_lines(top + 1, 0, bot, newsize - oldsize);

	/* set '[ and '] mark */
	if (top + 1 < curbuf->b_op_start.lnum)
	    curbuf->b_op_start.lnum = top + 1;
	if (newsize == 0 && top + 1 > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = top + 1;
	else if (top + newsize > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = top + newsize;

	u_newcount += newsize;
	u_oldcount += oldsize;
	uep->ue_size = oldsize;
	uep->ue_array = newarray;
	uep->ue_bot = top + newsize + 1;

	/*
	 * insert this entry in front of the new entry list
	 */
	nuep = uep->ue_next;
	uep->ue_next = newlist;
	newlist = uep;
    }

    curhead->uh_entry = newlist;
    curhead->uh_flags = new_flags;
    if ((old_flags & UH_EMPTYBUF) && bufempty())
	curbuf->b_ml.ml_flags |= ML_EMPTY;
    if (old_flags & UH_CHANGED)
	changed();
    else
#ifdef FEAT_NETBEANS_INTG
	/* per netbeans undo rules, keep it as modified */
	if (!isNetbeansModified(curbuf))
#endif
	unchanged(curbuf, FALSE);

    /*
     * restore marks from before undo/redo
     */
    for (i = 0; i < NMARKS; ++i)
    {
	if (curhead->uh_namedm[i].lnum != 0)
	    curbuf->b_namedm[i] = curhead->uh_namedm[i];
	if (namedm[i].lnum != 0)
	    curhead->uh_namedm[i] = namedm[i];
	else
	    curhead->uh_namedm[i].lnum = 0;
    }
    if (curhead->uh_visual.vi_start.lnum != 0)
    {
	curbuf->b_visual = curhead->uh_visual;
	curhead->uh_visual = visualinfo;
    }

    /*
     * If the cursor is only off by one line, put it at the same position as
     * before starting the change (for the "o" command).
     * Otherwise the cursor should go to the first undone line.
     */
    if (curhead->uh_cursor.lnum + 1 == curwin->w_cursor.lnum
						 && curwin->w_cursor.lnum > 1)
	--curwin->w_cursor.lnum;
    if (curwin->w_cursor.lnum <= curbuf->b_ml.ml_line_count)
    {
	if (curhead->uh_cursor.lnum == curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = curhead->uh_cursor.col;
#ifdef FEAT_VIRTUALEDIT
	    if (virtual_active() && curhead->uh_cursor_vcol >= 0)
		coladvance((colnr_T)curhead->uh_cursor_vcol);
	    else
		curwin->w_cursor.coladd = 0;
#endif
	}
	else
	    beginline(BL_SOL | BL_FIX);
    }
    else
    {
	/* We get here with the current cursor line being past the end (eg
	 * after adding lines at the end of the file, and then undoing it).
	 * check_cursor() will move the cursor to the last line.  Move it to
	 * the first column here. */
	curwin->w_cursor.col = 0;
#ifdef FEAT_VIRTUALEDIT
	curwin->w_cursor.coladd = 0;
#endif
    }

    /* Make sure the cursor is on an existing line and column. */
    check_cursor();

    /* Remember where we are for "g-" and ":earlier 10s". */
    curbuf->b_u_seq_cur = curhead->uh_seq;
    if (undo)
	/* We are below the previous undo.  However, to make ":earlier 1s"
	 * work we compute this as being just above the just undone change. */
	--curbuf->b_u_seq_cur;

    /* Remember where we are for ":earlier 1f" and ":later 1f". */
    if (curhead->uh_save_nr != 0)
    {
	if (undo)
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr - 1;
	else
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr;
    }

    /* The timestamp can be the same for multiple changes, just use the one of
     * the undone/redone change. */
    curbuf->b_u_time_cur = curhead->uh_time;

#ifdef FEAT_AUTOCMD
    unblock_autocmds();
#endif
#ifdef U_DEBUG
    u_check(FALSE);
#endif
}