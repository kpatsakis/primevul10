u_find_first_changed(void)
{
    u_header_T	*uhp = curbuf->b_u_newhead;
    u_entry_T   *uep;
    linenr_T	lnum;

    if (curbuf->b_u_curhead != NULL || uhp == NULL)
	return;  /* undid something in an autocmd? */

    /* Check that the last undo block was for the whole file. */
    uep = uhp->uh_entry;
    if (uep->ue_top != 0 || uep->ue_bot != 0)
	return;

    for (lnum = 1; lnum < curbuf->b_ml.ml_line_count
					      && lnum <= uep->ue_size; ++lnum)
	if (STRCMP(ml_get_buf(curbuf, lnum, FALSE),
						uep->ue_array[lnum - 1]) != 0)
	{
	    clearpos(&(uhp->uh_cursor));
	    uhp->uh_cursor.lnum = lnum;
	    return;
	}
    if (curbuf->b_ml.ml_line_count != uep->ue_size)
    {
	/* lines added or deleted at the end, put the cursor there */
	clearpos(&(uhp->uh_cursor));
	uhp->uh_cursor.lnum = lnum;
    }
}