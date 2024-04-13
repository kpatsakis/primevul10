u_sync(
    int	    force)	/* Also sync when no_u_sync is set. */
{
    /* Skip it when already synced or syncing is disabled. */
    if (curbuf->b_u_synced || (!force && no_u_sync > 0))
	return;
#if defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    if (im_is_preediting())
	return;		    /* XIM is busy, don't break an undo sequence */
#endif
    if (get_undolevel() < 0)
	curbuf->b_u_synced = TRUE;  /* no entries, nothing to do */
    else
    {
	u_getbot();		    /* compute ue_bot of previous u_save */
	curbuf->b_u_curhead = NULL;
    }
}