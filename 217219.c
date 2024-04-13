ins_compl_infercase_gettext(
	char_u	*str,
	int	char_len,
	int	compl_char_len,
	int	min_len,
	char_u  **tofree)
{
    int		*wca;			// Wide character array.
    char_u	*p;
    int		i, c;
    int		has_lower = FALSE;
    int		was_letter = FALSE;
    garray_T	gap;

    IObuff[0] = NUL;

    // Allocate wide character array for the completion and fill it.
    wca = ALLOC_MULT(int, char_len);
    if (wca == NULL)
	return IObuff;

    p = str;
    for (i = 0; i < char_len; ++i)
	if (has_mbyte)
	    wca[i] = mb_ptr2char_adv(&p);
	else
	    wca[i] = *(p++);

    // Rule 1: Were any chars converted to lower?
    p = compl_orig_text;
    for (i = 0; i < min_len; ++i)
    {
	if (has_mbyte)
	    c = mb_ptr2char_adv(&p);
	else
	    c = *(p++);
	if (MB_ISLOWER(c))
	{
	    has_lower = TRUE;
	    if (MB_ISUPPER(wca[i]))
	    {
		// Rule 1 is satisfied.
		for (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOLOWER(wca[i]);
		break;
	    }
	}
    }

    // Rule 2: No lower case, 2nd consecutive letter converted to
    // upper case.
    if (!has_lower)
    {
	p = compl_orig_text;
	for (i = 0; i < min_len; ++i)
	{
	    if (has_mbyte)
		c = mb_ptr2char_adv(&p);
	    else
		c = *(p++);
	    if (was_letter && MB_ISUPPER(c) && MB_ISLOWER(wca[i]))
	    {
		// Rule 2 is satisfied.
		for (i = compl_char_len; i < char_len; ++i)
		    wca[i] = MB_TOUPPER(wca[i]);
		break;
	    }
	    was_letter = MB_ISLOWER(c) || MB_ISUPPER(c);
	}
    }

    // Copy the original case of the part we typed.
    p = compl_orig_text;
    for (i = 0; i < min_len; ++i)
    {
	if (has_mbyte)
	    c = mb_ptr2char_adv(&p);
	else
	    c = *(p++);
	if (MB_ISLOWER(c))
	    wca[i] = MB_TOLOWER(wca[i]);
	else if (MB_ISUPPER(c))
	    wca[i] = MB_TOUPPER(wca[i]);
    }

    // Generate encoding specific output from wide character array.
    p = IObuff;
    i = 0;
    ga_init2(&gap, 1, 500);
    while (i < char_len)
    {
	if (gap.ga_data != NULL)
	{
	    if (ga_grow(&gap, 10) == FAIL)
	    {
		ga_clear(&gap);
		return (char_u *)"[failed]";
	    }
	    p = (char_u *)gap.ga_data + gap.ga_len;
	    if (has_mbyte)
		gap.ga_len += (*mb_char2bytes)(wca[i++], p);
	    else
	    {
		*p = wca[i++];
		++gap.ga_len;
	    }
	}
	else if ((p - IObuff) + 6 >= IOSIZE)
	{
	    // Multi-byte characters can occupy up to five bytes more than
	    // ASCII characters, and we also need one byte for NUL, so when
	    // getting to six bytes from the edge of IObuff switch to using a
	    // growarray.  Add the character in the next round.
	    if (ga_grow(&gap, IOSIZE) == FAIL)
		return (char_u *)"[failed]";
	    *p = NUL;
	    STRCPY(gap.ga_data, IObuff);
	    gap.ga_len = (int)STRLEN(IObuff);
	}
	else if (has_mbyte)
	    p += (*mb_char2bytes)(wca[i++], p);
	else
	    *(p++) = wca[i++];
    }
    vim_free(wca);

    if (gap.ga_data != NULL)
    {
	*tofree = gap.ga_data;
	return gap.ga_data;
    }

    *p = NUL;
    return IObuff;
}