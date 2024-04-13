check_arglist_locked(void)
{
    if (arglist_locked)
    {
	emsg(_(e_cannot_change_arglist_recursively));
	return FAIL;
    }
    return OK;
}