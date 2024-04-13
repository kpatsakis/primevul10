undo_allowed(void)
{
    /* Don't allow changes when 'modifiable' is off.  */
    if (!curbuf->b_p_ma)
    {
	EMSG(_(e_modifiable));
	return FALSE;
    }

#ifdef HAVE_SANDBOX
    /* In the sandbox it's not allowed to change the text. */
    if (sandbox != 0)
    {
	EMSG(_(e_sandbox));
	return FALSE;
    }
#endif

    /* Don't allow changes in the buffer while editing the cmdline.  The
     * caller of getcmdline() may get confused. */
    if (textlock != 0)
    {
	EMSG(_(e_secure));
	return FALSE;
    }

    return TRUE;
}