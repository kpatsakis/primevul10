u_undo(int count)
{
    /*
     * If we get an undo command while executing a macro, we behave like the
     * original vi. If this happens twice in one macro the result will not
     * be compatible.
     */
    if (curbuf->b_u_synced == FALSE)
    {
	u_sync(TRUE);
	count = 1;
    }

    if (vim_strchr(p_cpo, CPO_UNDO) == NULL)
	undo_undoes = TRUE;
    else
	undo_undoes = !undo_undoes;
    u_doit(count);
}