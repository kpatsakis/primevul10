undo_read_time(bufinfo_T *bi)
{
#ifdef FEAT_CRYPT
    if (bi->bi_buffer != NULL)
    {
	char_u  buf[8];
	time_t	n = 0;
	int	i;

	undo_read(bi, buf, (size_t)8);
	for (i = 0; i < 8; ++i)
	    n = (n << 8) + buf[i];
	return n;
    }
#endif
    return get8ctime(bi->bi_fp);
}