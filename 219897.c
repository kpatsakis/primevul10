serialize_header(bufinfo_T *bi, char_u *hash)
{
    int		len;
    buf_T	*buf = bi->bi_buf;
    FILE	*fp = bi->bi_fp;
    char_u	time_buf[8];

    /* Start writing, first the magic marker and undo info version. */
    if (fwrite(UF_START_MAGIC, (size_t)UF_START_MAGIC_LEN, (size_t)1, fp) != 1)
	return FAIL;

    /* If the buffer is encrypted then all text bytes following will be
     * encrypted.  Numbers and other info is not crypted. */
#ifdef FEAT_CRYPT
    if (*buf->b_p_key != NUL)
    {
	char_u *header;
	int    header_len;

	undo_write_bytes(bi, (long_u)UF_VERSION_CRYPT, 2);
	bi->bi_state = crypt_create_for_writing(crypt_get_method_nr(buf),
					  buf->b_p_key, &header, &header_len);
	if (bi->bi_state == NULL)
	    return FAIL;
	len = (int)fwrite(header, (size_t)header_len, (size_t)1, fp);
	vim_free(header);
	if (len != 1)
	{
	    crypt_free_state(bi->bi_state);
	    bi->bi_state = NULL;
	    return FAIL;
	}

	if (crypt_whole_undofile(crypt_get_method_nr(buf)))
	{
	    bi->bi_buffer = alloc(CRYPT_BUF_SIZE);
	    if (bi->bi_buffer == NULL)
	    {
		crypt_free_state(bi->bi_state);
		bi->bi_state = NULL;
		return FAIL;
	    }
	    bi->bi_used = 0;
	}
    }
    else
#endif
	undo_write_bytes(bi, (long_u)UF_VERSION, 2);


    /* Write a hash of the buffer text, so that we can verify it is still the
     * same when reading the buffer text. */
    if (undo_write(bi, hash, (size_t)UNDO_HASH_SIZE) == FAIL)
	return FAIL;

    /* buffer-specific data */
    undo_write_bytes(bi, (long_u)buf->b_ml.ml_line_count, 4);
    len = buf->b_u_line_ptr != NULL ? (int)STRLEN(buf->b_u_line_ptr) : 0;
    undo_write_bytes(bi, (long_u)len, 4);
    if (len > 0 && fwrite_crypt(bi, buf->b_u_line_ptr, (size_t)len) == FAIL)
	return FAIL;
    undo_write_bytes(bi, (long_u)buf->b_u_line_lnum, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_line_colnr, 4);

    /* Undo structures header data */
    put_header_ptr(bi, buf->b_u_oldhead);
    put_header_ptr(bi, buf->b_u_newhead);
    put_header_ptr(bi, buf->b_u_curhead);

    undo_write_bytes(bi, (long_u)buf->b_u_numhead, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_seq_last, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_seq_cur, 4);
    time_to_bytes(buf->b_u_time_cur, time_buf);
    undo_write(bi, time_buf, 8);

    /* Optional fields. */
    undo_write_bytes(bi, 4, 1);
    undo_write_bytes(bi, UF_LAST_SAVE_NR, 1);
    undo_write_bytes(bi, (long_u)buf->b_u_save_nr_last, 4);

    undo_write_bytes(bi, 0, 1);  /* end marker */

    return OK;
}