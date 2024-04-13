arg_all(void)
{
    int		len;
    int		idx;
    char_u	*retval = NULL;
    char_u	*p;

    // Do this loop two times:
    // first time: compute the total length
    // second time: concatenate the names
    for (;;)
    {
	len = 0;
	for (idx = 0; idx < ARGCOUNT; ++idx)
	{
	    p = alist_name(&ARGLIST[idx]);
	    if (p != NULL)
	    {
		if (len > 0)
		{
		    // insert a space in between names
		    if (retval != NULL)
			retval[len] = ' ';
		    ++len;
		}
		for ( ; *p != NUL; ++p)
		{
		    if (*p == ' '
#ifndef BACKSLASH_IN_FILENAME
			    || *p == '\\'
#endif
			    || *p == '`')
		    {
			// insert a backslash
			if (retval != NULL)
			    retval[len] = '\\';
			++len;
		    }
		    if (retval != NULL)
			retval[len] = *p;
		    ++len;
		}
	    }
	}

	// second time: break here
	if (retval != NULL)
	{
	    retval[len] = NUL;
	    break;
	}

	// allocate memory
	retval = alloc(len + 1);
	if (retval == NULL)
	    break;
    }

    return retval;
}