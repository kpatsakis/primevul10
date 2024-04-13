undo_flush(bufinfo_T *bi)
{
    if (bi->bi_buffer != NULL && bi->bi_used > 0)
    {
	crypt_encode_inplace(bi->bi_state, bi->bi_buffer, bi->bi_used);
	if (fwrite(bi->bi_buffer, bi->bi_used, (size_t)1, bi->bi_fp) != 1)
	    return FAIL;
	bi->bi_used = 0;
    }
    return OK;
}