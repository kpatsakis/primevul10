set_completefunc_option(void)
{
    int	retval;

    retval = option_set_callback_func(curbuf->b_p_cfu, &cfu_cb);
    if (retval == OK)
	set_buflocal_cfu_callback(curbuf);

    return retval;
}