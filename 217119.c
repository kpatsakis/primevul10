set_omnifunc_option(void)
{
    int	retval;

    retval = option_set_callback_func(curbuf->b_p_ofu, &ofu_cb);
    if (retval == OK)
	set_buflocal_ofu_callback(curbuf);

    return retval;
}