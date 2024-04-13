get_arglist_exp(
    char_u	*str,
    int		*fcountp,
    char_u	***fnamesp,
    int		wig)
{
    garray_T	ga;
    int		i;

    if (get_arglist(&ga, str, TRUE) == FAIL)
	return FAIL;
    if (wig == TRUE)
	i = expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);
    else
	i = gen_expand_wildcards(ga.ga_len, (char_u **)ga.ga_data,
			     fcountp, fnamesp, EW_FILE|EW_NOTFOUND|EW_NOTWILD);

    ga_clear(&ga);
    return i;
}