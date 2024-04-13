u_check_tree(u_header_T *uhp,
	u_header_T *exp_uh_next,
	u_header_T *exp_uh_alt_prev)
{
    u_entry_T *uep;

    if (uhp == NULL)
	return;
    ++header_count;
    if (uhp == curbuf->b_u_curhead && ++seen_b_u_curhead > 1)
    {
	EMSG("b_u_curhead found twice (looping?)");
	return;
    }
    if (uhp == curbuf->b_u_newhead && ++seen_b_u_newhead > 1)
    {
	EMSG("b_u_newhead found twice (looping?)");
	return;
    }

    if (uhp->uh_magic != UH_MAGIC)
	EMSG("uh_magic wrong (may be using freed memory)");
    else
    {
	/* Check pointers back are correct. */
	if (uhp->uh_next.ptr != exp_uh_next)
	{
	    EMSG("uh_next wrong");
	    smsg((char_u *)"expected: 0x%x, actual: 0x%x",
					       exp_uh_next, uhp->uh_next.ptr);
	}
	if (uhp->uh_alt_prev.ptr != exp_uh_alt_prev)
	{
	    EMSG("uh_alt_prev wrong");
	    smsg((char_u *)"expected: 0x%x, actual: 0x%x",
				       exp_uh_alt_prev, uhp->uh_alt_prev.ptr);
	}

	/* Check the undo tree at this header. */
	for (uep = uhp->uh_entry; uep != NULL; uep = uep->ue_next)
	{
	    if (uep->ue_magic != UE_MAGIC)
	    {
		EMSG("ue_magic wrong (may be using freed memory)");
		break;
	    }
	}

	/* Check the next alt tree. */
	u_check_tree(uhp->uh_alt_next.ptr, uhp->uh_next.ptr, uhp);

	/* Check the next header in this branch. */
	u_check_tree(uhp->uh_prev.ptr, uhp, NULL);
    }
}