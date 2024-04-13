get_arglist_name(expand_T *xp UNUSED, int idx)
{
    if (idx >= ARGCOUNT)
	return NULL;

    return alist_name(&ARGLIST[idx]);
}