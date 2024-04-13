alist_clear(alist_T *al)
{
    if (check_arglist_locked() == FAIL)
	return;
    while (--al->al_ga.ga_len >= 0)
	vim_free(AARGLIST(al)[al->al_ga.ga_len].ae_fname);
    ga_clear(&al->al_ga);
}