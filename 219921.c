undo_read(bufinfo_T *bi, char_u *buffer, size_t size)
{
#ifdef FEAT_CRYPT
    if (bi->bi_buffer != NULL)
    {
	int	size_todo = (int)size;
	char_u	*p = buffer;

	while (size_todo > 0)
	{
	    size_t n;

	    if (bi->bi_used >= bi->bi_avail)
	    {
		n = fread(bi->bi_buffer, 1, (size_t)CRYPT_BUF_SIZE, bi->bi_fp);
		if (n == 0)
		{
		    /* Error may be checked for only later.  Fill with zeros,
		     * so that the reader won't use garbage. */
		    vim_memset(p, 0, size_todo);
		    return FAIL;
		}
		bi->bi_avail = n;
		bi->bi_used = 0;
		crypt_decode_inplace(bi->bi_state, bi->bi_buffer, bi->bi_avail);
	    }
	    n = size_todo;
	    if (n > bi->bi_avail - bi->bi_used)
		n = bi->bi_avail - bi->bi_used;
	    mch_memmove(p, bi->bi_buffer + bi->bi_used, n);
	    bi->bi_used += n;
	    size_todo -= (int)n;
	    p += n;
	}
	return OK;
    }
#endif
    if (fread(buffer, (size_t)size, 1, bi->bi_fp) != 1)
	return FAIL;
    return OK;
}