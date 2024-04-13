quote_meta(char_u *dest, char_u *src, int len)
{
    unsigned	m = (unsigned)len + 1;  // one extra for the NUL

    for ( ; --len >= 0; src++)
    {
	switch (*src)
	{
	    case '.':
	    case '*':
	    case '[':
		if (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    break;
		// FALLTHROUGH
	    case '~':
		if (!magic_isset())	// quote these only if magic is set
		    break;
		// FALLTHROUGH
	    case '\\':
		if (ctrl_x_mode_dictionary() || ctrl_x_mode_thesaurus())
		    break;
		// FALLTHROUGH
	    case '^':		// currently it's not needed.
	    case '$':
		m++;
		if (dest != NULL)
		    *dest++ = '\\';
		break;
	}
	if (dest != NULL)
	    *dest++ = *src;
	// Copy remaining bytes of a multibyte character.
	if (has_mbyte)
	{
	    int i, mb_len;

	    mb_len = (*mb_ptr2len)(src) - 1;
	    if (mb_len > 0 && len >= mb_len)
		for (i = 0; i < mb_len; ++i)
		{
		    --len;
		    ++src;
		    if (dest != NULL)
			*dest++ = *src;
		}
	}
    }
    if (dest != NULL)
	*dest = NUL;

    return m;
}