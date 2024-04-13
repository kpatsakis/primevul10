thesarurs_add_words_in_line(
	char_u	*fname,
	char_u	**buf_arg,
	int	dir,
	char_u	*skip_word)
{
    int		status = OK;
    char_u	*ptr;
    char_u	*wstart;

    // Add the other matches on the line
    ptr = *buf_arg;
    while (!got_int)
    {
	// Find start of the next word.  Skip white
	// space and punctuation.
	ptr = find_word_start(ptr);
	if (*ptr == NUL || *ptr == NL)
	    break;
	wstart = ptr;

	// Find end of the word.
	if (has_mbyte)
	    // Japanese words may have characters in
	    // different classes, only separate words
	    // with single-byte non-word characters.
	    while (*ptr != NUL)
	    {
		int l = (*mb_ptr2len)(ptr);

		if (l < 2 && !vim_iswordc(*ptr))
		    break;
		ptr += l;
	    }
	else
	    ptr = find_word_end(ptr);

	// Add the word. Skip the regexp match.
	if (wstart != skip_word)
	{
	    status = ins_compl_add_infercase(wstart, (int)(ptr - wstart), p_ic,
							fname, dir, FALSE);
	    if (status == FAIL)
		break;
	}
    }

    *buf_arg = ptr;
    return status;
}