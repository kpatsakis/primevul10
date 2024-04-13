skip_regexp_err(
    char_u	*startp,
    int		delim,
    int		magic)
{
    char_u *p = skip_regexp(startp, delim, magic);

    if (*p != delim)
    {
	semsg(_("E654: missing delimiter after search pattern: %s"), startp);
	return NULL;
    }
    return p;
}