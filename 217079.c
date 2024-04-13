file_close(ref * pfile)
{
    stream *s;

    if (file_is_valid(s, pfile)) {      /* closing a closed file is a no-op */
        if (sclose(s))
            return_error(gs_error_ioerror);
    }
    return 0;
}