execfile_cleanup(i_ctx_t *i_ctx_p)
{
    check_ostack(1);
    *++osp = esp[2];
    return zclosefile(i_ctx_p);
}