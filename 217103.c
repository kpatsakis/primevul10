check_file_permissions(i_ctx_t *i_ctx_p, const char *fname, int len,
                        const char *permitgroup)
{
    char fname_reduced[gp_file_name_sizeof];
    uint rlen = sizeof(fname_reduced);

    if (gp_file_name_reduce(fname, len, fname_reduced, &rlen) != gp_combine_success)
        return gs_error_invalidaccess;         /* fail if we couldn't reduce */
    return check_file_permissions_reduced(i_ctx_p, fname_reduced, rlen, permitgroup);
}