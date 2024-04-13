ex_argedit(exarg_T *eap)
{
    int i = eap->addr_count ? (int)eap->line2 : curwin->w_arg_idx + 1;
    // Whether curbuf will be reused, curbuf->b_ffname will be set.
    int curbuf_is_reusable = curbuf_reusable();

    if (do_arglist(eap->arg, AL_ADD, i, TRUE) == FAIL)
	return;
    maketitle();

    if (curwin->w_arg_idx == 0
	    && (curbuf->b_ml.ml_flags & ML_EMPTY)
	    && (curbuf->b_ffname == NULL || curbuf_is_reusable))
	i = 0;
    // Edit the argument.
    if (i < ARGCOUNT)
	do_argfile(eap, i);
}