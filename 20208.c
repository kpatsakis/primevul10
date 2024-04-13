do_argfile(exarg_T *eap, int argn)
{
    int		other;
    char_u	*p;
    int		old_arg_idx = curwin->w_arg_idx;

    if (ERROR_IF_ANY_POPUP_WINDOW)
	return;
    if (argn < 0 || argn >= ARGCOUNT)
    {
	if (ARGCOUNT <= 1)
	    emsg(_("E163: There is only one file to edit"));
	else if (argn < 0)
	    emsg(_("E164: Cannot go before first file"));
	else
	    emsg(_("E165: Cannot go beyond last file"));
    }
    else
    {
	setpcmark();
#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif

	// split window or create new tab page first
	if (*eap->cmd == 's' || cmdmod.cmod_tab != 0)
	{
	    if (win_split(0, 0) == FAIL)
		return;
	    RESET_BINDING(curwin);
	}
	else
	{
	    // if 'hidden' set, only check for changed file when re-editing
	    // the same buffer
	    other = TRUE;
	    if (buf_hide(curbuf))
	    {
		p = fix_fname(alist_name(&ARGLIST[argn]));
		other = otherfile(p);
		vim_free(p);
	    }
	    if ((!buf_hide(curbuf) || !other)
		  && check_changed(curbuf, CCGD_AW
					 | (other ? 0 : CCGD_MULTWIN)
					 | (eap->forceit ? CCGD_FORCEIT : 0)
					 | CCGD_EXCMD))
		return;
	}

	curwin->w_arg_idx = argn;
	if (argn == ARGCOUNT - 1 && curwin->w_alist == &global_alist)
	    arg_had_last = TRUE;

	// Edit the file; always use the last known line number.
	// When it fails (e.g. Abort for already edited file) restore the
	// argument index.
	if (do_ecmd(0, alist_name(&ARGLIST[curwin->w_arg_idx]), NULL,
		      eap, ECMD_LAST,
		      (buf_hide(curwin->w_buffer) ? ECMD_HIDE : 0)
			 + (eap->forceit ? ECMD_FORCEIT : 0), curwin) == FAIL)
	    curwin->w_arg_idx = old_arg_idx;
	// like Vi: set the mark where the cursor is in the file.
	else if (eap->cmdidx != CMD_argdo)
	    setmark('\'');
    }
}