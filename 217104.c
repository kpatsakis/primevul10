make_invalid_file(i_ctx_t *i_ctx_p, ref * fp)
{
    make_file(fp, avm_invalid_file_entry, ~0, i_ctx_p->invalid_file_stream);
}