alist_add(
    alist_T	*al,
    char_u	*fname,
    int		set_fnum)	// 1: set buffer number; 2: re-use curbuf
{
    if (fname == NULL)		// don't add NULL file names
	return;
    if (check_arglist_locked() == FAIL)
	return;
    arglist_locked = TRUE;

#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(fname);
#endif
    AARGLIST(al)[al->al_ga.ga_len].ae_fname = fname;
    if (set_fnum > 0)
	AARGLIST(al)[al->al_ga.ga_len].ae_fnum =
	    buflist_add(fname, BLN_LISTED | (set_fnum == 2 ? BLN_CURBUF : 0));
    ++al->al_ga.ga_len;

    arglist_locked = FALSE;
}