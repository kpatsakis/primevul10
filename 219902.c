ex_undojoin(exarg_T *eap UNUSED)
{
    if (curbuf->b_u_newhead == NULL)
	return;		    /* nothing changed before */
    if (curbuf->b_u_curhead != NULL)
    {
	EMSG(_("E790: undojoin is not allowed after undo"));
	return;
    }
    if (!curbuf->b_u_synced)
	return;		    /* already unsynced */
    if (get_undolevel() < 0)
	return;		    /* no entries, nothing to do */
    else
	/* Append next change to the last entry */
	curbuf->b_u_synced = FALSE;
}