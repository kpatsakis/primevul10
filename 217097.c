check_file_permissions_aux(i_ctx_t *i_ctx_p, char *fname, uint flen)
{   /* i_ctx_p is NULL running init files. */
    /* fname must be reduced. */
    if (i_ctx_p == NULL)
        return 0;
    if (check_file_permissions_reduced(i_ctx_p, fname, flen, "PermitFileReading") < 0)
        return_error(gs_error_invalidfileaccess);
    return 0;
}