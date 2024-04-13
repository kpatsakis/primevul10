get_compl_len(void)
{
    int off = (int)curwin->w_cursor.col - (int)compl_col;

    if (off < 0)
	return 0;
    return off;
}