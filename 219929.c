u_freebranch(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	/* if not NULL reset when freeing this header */
{
    u_header_T	    *tofree, *next;

    /* If this is the top branch we may need to use u_freeheader() to update
     * all the pointers. */
    if (uhp == buf->b_u_oldhead)
    {
	while (buf->b_u_oldhead != NULL)
	    u_freeheader(buf, buf->b_u_oldhead, uhpp);
	return;
    }

    if (uhp->uh_alt_prev.ptr != NULL)
	uhp->uh_alt_prev.ptr->uh_alt_next.ptr = NULL;

    next = uhp;
    while (next != NULL)
    {
	tofree = next;
	if (tofree->uh_alt_next.ptr != NULL)
	    u_freebranch(buf, tofree->uh_alt_next.ptr, uhpp);   /* recursive */
	next = tofree->uh_prev.ptr;
	u_freeentries(buf, tofree, uhpp);
    }
}