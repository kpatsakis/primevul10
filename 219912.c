read_string_decrypt(bufinfo_T *bi, int len)
{
    char_u  *ptr = alloc((unsigned)len + 1);

    if (ptr != NULL)
    {
	if (len > 0 && undo_read(bi, ptr, len) == FAIL)
	{
	    vim_free(ptr);
	    return NULL;
	}
	ptr[len] = NUL;
#ifdef FEAT_CRYPT
	if (bi->bi_state != NULL && bi->bi_buffer == NULL)
	    crypt_decode_inplace(bi->bi_state, ptr, len);
#endif
    }
    return ptr;
}