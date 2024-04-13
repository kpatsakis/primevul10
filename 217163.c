find_word_end(char_u *ptr)
{
    int		start_class;

    if (has_mbyte)
    {
	start_class = mb_get_class(ptr);
	if (start_class > 1)
	    while (*ptr != NUL)
	    {
		ptr += (*mb_ptr2len)(ptr);
		if (mb_get_class(ptr) != start_class)
		    break;
	    }
    }
    else
	while (vim_iswordc(*ptr))
	    ++ptr;
    return ptr;
}