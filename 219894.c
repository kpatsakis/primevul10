undo_read_4c(bufinfo_T *bi)
{
#ifdef FEAT_CRYPT
    if (bi->bi_buffer != NULL)
    {
	char_u  buf[4];
	int	n;

	undo_read(bi, buf, (size_t)4);
	n = ((unsigned)buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	return n;
    }
#endif
    return get4c(bi->bi_fp);
}