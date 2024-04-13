undo_write(bufinfo_T *bi, char_u *ptr, size_t len)
{
#ifdef FEAT_CRYPT
    if (bi->bi_buffer != NULL)
    {
	size_t	len_todo = len;
	char_u  *p = ptr;

	while (bi->bi_used + len_todo >= CRYPT_BUF_SIZE)
	{
	    size_t	n = CRYPT_BUF_SIZE - bi->bi_used;

	    mch_memmove(bi->bi_buffer + bi->bi_used, p, n);
	    len_todo -= n;
	    p += n;
	    bi->bi_used = CRYPT_BUF_SIZE;
	    if (undo_flush(bi) == FAIL)
		return FAIL;
	}
	if (len_todo > 0)
	{
	    mch_memmove(bi->bi_buffer + bi->bi_used, p, len_todo);
	    bi->bi_used += len_todo;
	}
	return OK;
    }
#endif
    if (fwrite(ptr, len, (size_t)1, bi->bi_fp) != 1)
	return FAIL;
    return OK;
}