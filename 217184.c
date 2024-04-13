set_thesaurusfunc_option(void)
{
    int	retval;

    if (*curbuf->b_p_tsrfu != NUL)
    {
	// buffer-local option set
	retval = option_set_callback_func(curbuf->b_p_tsrfu,
							&curbuf->b_tsrfu_cb);
    }
    else
    {
	// global option set
	retval = option_set_callback_func(p_tsrfu, &tsrfu_cb);
    }

    return retval;
}