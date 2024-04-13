ex_next(exarg_T *eap)
{
    int		i;

    // check for changed buffer now, if this fails the argument list is not
    // redefined.
    if (       buf_hide(curbuf)
	    || eap->cmdidx == CMD_snext
	    || !check_changed(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	if (*eap->arg != NUL)		    // redefine file list
	{
	    if (do_arglist(eap->arg, AL_SET, 0, TRUE) == FAIL)
		return;
	    i = 0;
	}
	else
	    i = curwin->w_arg_idx + (int)eap->line2;
	do_argfile(eap, i);
    }
}