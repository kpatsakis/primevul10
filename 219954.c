u_add_time(char_u *buf, size_t buflen, time_t tt)
{
#ifdef HAVE_STRFTIME
    struct tm	*curtime;

    if (vim_time() - tt >= 100)
    {
	curtime = localtime(&tt);
	if (vim_time() - tt < (60L * 60L * 12L))
	    /* within 12 hours */
	    (void)strftime((char *)buf, buflen, "%H:%M:%S", curtime);
	else
	    /* longer ago */
	    (void)strftime((char *)buf, buflen, "%Y/%m/%d %H:%M:%S", curtime);
    }
    else
#endif
	vim_snprintf((char *)buf, buflen, _("%ld seconds ago"),
						      (long)(vim_time() - tt));
}