u_undofile_reset_and_delete(buf_T *buf)
{
    char_u *file_name;

    if (!buf->b_p_udf)
	return;

    file_name = u_get_undo_file_name(buf->b_ffname, TRUE);
    if (file_name != NULL)
    {
	mch_remove(file_name);
	vim_free(file_name);
    }

    set_option_value((char_u *)"undofile", 0L, NULL, OPT_LOCAL);
}