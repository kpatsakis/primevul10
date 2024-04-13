serialize_uep(
    bufinfo_T	*bi,
    u_entry_T	*uep)
{
    int		i;
    size_t	len;

    undo_write_bytes(bi, (long_u)uep->ue_top, 4);
    undo_write_bytes(bi, (long_u)uep->ue_bot, 4);
    undo_write_bytes(bi, (long_u)uep->ue_lcount, 4);
    undo_write_bytes(bi, (long_u)uep->ue_size, 4);
    for (i = 0; i < uep->ue_size; ++i)
    {
	len = STRLEN(uep->ue_array[i]);
	if (undo_write_bytes(bi, (long_u)len, 4) == FAIL)
	    return FAIL;
	if (len > 0 && fwrite_crypt(bi, uep->ue_array[i], len) == FAIL)
	    return FAIL;
    }
    return OK;
}