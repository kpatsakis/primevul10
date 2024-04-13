file_read_string(const byte *str, uint len, ref *pfile, gs_ref_memory_t *imem)
{
    stream *s = file_alloc_stream((gs_memory_t *)imem, "file_read_string");

    if (s == 0)
        return_error(gs_error_VMerror);
    sread_string(s, str, len);
    s->foreign = 1;
    s->write_id = 0;
    make_file(pfile, a_readonly | imemory_space(imem), s->read_id, s);
    s->save_close = s->procs.close;
    s->procs.close = file_close_disable;
    return 0;
}