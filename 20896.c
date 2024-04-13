ex_find(exarg_T *eap)
{
    char_u	*fname;
    int		count;

    fname = find_file_in_path(eap->arg, (int)STRLEN(eap->arg), FNAME_MESS,
						      TRUE, curbuf->b_ffname);
    if (eap->addr_count > 0)
    {
	// Repeat finding the file "count" times.  This matters when it
	// appears several times in the path.
	count = eap->line2;
	while (fname != NULL && --count > 0)
	{
	    vim_free(fname);
	    fname = find_file_in_path(NULL, 0, FNAME_MESS,
						     FALSE, curbuf->b_ffname);
	}
    }

    if (fname != NULL)
    {
	eap->arg = fname;
	do_exedit(eap, NULL);
	vim_free(fname);
    }
}