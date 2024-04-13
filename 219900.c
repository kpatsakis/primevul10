fwrite_crypt(bufinfo_T *bi, char_u *ptr, size_t len)
{
#ifdef FEAT_CRYPT
    char_u  *copy;
    char_u  small_buf[100];
    size_t  i;

    if (bi->bi_state != NULL && bi->bi_buffer == NULL)
    {
	/* crypting every piece of text separately */
	if (len < 100)
	    copy = small_buf;  /* no malloc()/free() for short strings */
	else
	{
	    copy = lalloc(len, FALSE);
	    if (copy == NULL)
		return 0;
	}
	crypt_encode(bi->bi_state, ptr, len, copy);
	i = fwrite(copy, len, (size_t)1, bi->bi_fp);
	if (copy != small_buf)
	    vim_free(copy);
	return i == 1 ? OK : FAIL;
    }
#endif
    return undo_write(bi, ptr, len);
}