execfile_finish(i_ctx_t *i_ctx_p)
{
    check_ostack(1);
    esp -= 2;
    execfile_cleanup(i_ctx_p);
    return o_pop_estack;
}