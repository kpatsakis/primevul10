find_start_rawstring(int ind_maxcomment)	// XXX
{
    pos_T	*pos;
    char_u	*line;
    char_u	*p;
    int		cur_maxcomment = ind_maxcomment;

    for (;;)
    {
	pos = findmatchlimit(NULL, 'R', FM_BACKWARD, cur_maxcomment);
	if (pos == NULL)
	    break;

	// Check if the raw string start we found is inside a string.
	// If it is then restrict the search to below this line and try again.
	line = ml_get(pos->lnum);
	for (p = line; *p && (colnr_T)(p - line) < pos->col; ++p)
	    p = skip_string(p);
	if ((colnr_T)(p - line) <= pos->col)
	    break;
	cur_maxcomment = curwin->w_cursor.lnum - pos->lnum - 1;
	if (cur_maxcomment <= 0)
	{
	    pos = NULL;
	    break;
	}
    }
    return pos;
}