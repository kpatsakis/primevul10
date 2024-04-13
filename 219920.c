u_doit(int startcount)
{
    int count = startcount;

    if (!undo_allowed())
	return;

    u_newcount = 0;
    u_oldcount = 0;
    if (curbuf->b_ml.ml_flags & ML_EMPTY)
	u_oldcount = -1;
    while (count--)
    {
	/* Do the change warning now, so that it triggers FileChangedRO when
	 * needed.  This may cause the file to be reloaded, that must happen
	 * before we do anything, because it may change curbuf->b_u_curhead
	 * and more. */
	change_warning(0);

	if (undo_undoes)
	{
	    if (curbuf->b_u_curhead == NULL)		/* first undo */
		curbuf->b_u_curhead = curbuf->b_u_newhead;
	    else if (get_undolevel() > 0)		/* multi level undo */
		/* get next undo */
		curbuf->b_u_curhead = curbuf->b_u_curhead->uh_next.ptr;
	    /* nothing to undo */
	    if (curbuf->b_u_numhead == 0 || curbuf->b_u_curhead == NULL)
	    {
		/* stick curbuf->b_u_curhead at end */
		curbuf->b_u_curhead = curbuf->b_u_oldhead;
		beep_flush();
		if (count == startcount - 1)
		{
		    MSG(_("Already at oldest change"));
		    return;
		}
		break;
	    }

	    u_undoredo(TRUE);
	}
	else
	{
	    if (curbuf->b_u_curhead == NULL || get_undolevel() <= 0)
	    {
		beep_flush();	/* nothing to redo */
		if (count == startcount - 1)
		{
		    MSG(_("Already at newest change"));
		    return;
		}
		break;
	    }

	    u_undoredo(FALSE);

	    /* Advance for next redo.  Set "newhead" when at the end of the
	     * redoable changes. */
	    if (curbuf->b_u_curhead->uh_prev.ptr == NULL)
		curbuf->b_u_newhead = curbuf->b_u_curhead;
	    curbuf->b_u_curhead = curbuf->b_u_curhead->uh_prev.ptr;
	}
    }
    u_undo_end(undo_undoes, FALSE);
}