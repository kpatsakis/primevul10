u_freeentries(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	/* if not NULL reset when freeing this header */
{
    u_entry_T	    *uep, *nuep;

    /* Check for pointers to the header that become invalid now. */
    if (buf->b_u_curhead == uhp)
	buf->b_u_curhead = NULL;
    if (buf->b_u_newhead == uhp)
	buf->b_u_newhead = NULL;  /* freeing the newest entry */
    if (uhpp != NULL && uhp == *uhpp)
	*uhpp = NULL;

    for (uep = uhp->uh_entry; uep != NULL; uep = nuep)
    {
	nuep = uep->ue_next;
	u_freeentry(uep, uep->ue_size);
    }

#ifdef U_DEBUG
    uhp->uh_magic = 0;
#endif
    vim_free((char_u *)uhp);
    --buf->b_u_numhead;
}