u_unch_branch(u_header_T *uhp)
{
    u_header_T	*uh;

    for (uh = uhp; uh != NULL; uh = uh->uh_prev.ptr)
    {
	uh->uh_flags |= UH_CHANGED;
	if (uh->uh_alt_next.ptr != NULL)
	    u_unch_branch(uh->uh_alt_next.ptr);	    /* recursive */
    }
}