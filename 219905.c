serialize_uhp(bufinfo_T *bi, u_header_T *uhp)
{
    int		i;
    u_entry_T	*uep;
    char_u	time_buf[8];

    if (undo_write_bytes(bi, (long_u)UF_HEADER_MAGIC, 2) == FAIL)
	return FAIL;

    put_header_ptr(bi, uhp->uh_next.ptr);
    put_header_ptr(bi, uhp->uh_prev.ptr);
    put_header_ptr(bi, uhp->uh_alt_next.ptr);
    put_header_ptr(bi, uhp->uh_alt_prev.ptr);
    undo_write_bytes(bi, uhp->uh_seq, 4);
    serialize_pos(bi, uhp->uh_cursor);
#ifdef FEAT_VIRTUALEDIT
    undo_write_bytes(bi, (long_u)uhp->uh_cursor_vcol, 4);
#else
    undo_write_bytes(bi, (long_u)0, 4);
#endif
    undo_write_bytes(bi, (long_u)uhp->uh_flags, 2);
    /* Assume NMARKS will stay the same. */
    for (i = 0; i < NMARKS; ++i)
	serialize_pos(bi, uhp->uh_namedm[i]);
    serialize_visualinfo(bi, &uhp->uh_visual);
    time_to_bytes(uhp->uh_time, time_buf);
    undo_write(bi, time_buf, 8);

    /* Optional fields. */
    undo_write_bytes(bi, 4, 1);
    undo_write_bytes(bi, UHP_SAVE_NR, 1);
    undo_write_bytes(bi, (long_u)uhp->uh_save_nr, 4);

    undo_write_bytes(bi, 0, 1);  /* end marker */

    /* Write all the entries. */
    for (uep = uhp->uh_entry; uep != NULL; uep = uep->ue_next)
    {
	undo_write_bytes(bi, (long_u)UF_ENTRY_MAGIC, 2);
	if (serialize_uep(bi, uep) == FAIL)
	    return FAIL;
    }
    undo_write_bytes(bi, (long_u)UF_ENTRY_END_MAGIC, 2);
    return OK;
}