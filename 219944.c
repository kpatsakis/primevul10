undo_read_2c(bufinfo_T *bi)
{
#ifdef FEAT_CRYPT
    if (bi->bi_buffer != NULL)
    {
	char_u  buf[2];
	int	n;

	undo_read(bi, buf, (size_t)2);
	n = (buf[0] << 8) + buf[1];
	return n;
    }
#endif
    return get2c(bi->bi_fp);
}