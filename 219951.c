undo_write_bytes(bufinfo_T *bi, long_u nr, int len)
{
    char_u  buf[8];
    int	    i;
    int	    bufi = 0;

    for (i = len - 1; i >= 0; --i)
	buf[bufi++] = (char_u)(nr >> (i * 8));
    return undo_write(bi, buf, (size_t)len);
}