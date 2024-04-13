filter_open(const char *file_access, uint buffer_size, ref * pfile,
            const stream_procs * procs, const stream_template * templat,
            const stream_state * st, gs_memory_t *mem)
{
    stream *s;
    uint ssize = gs_struct_type_size(templat->stype);
    stream_state *sst = 0;
    int code;

    if (templat->stype != &st_stream_state) {
        sst = s_alloc_state(mem, templat->stype, "filter_open(stream_state)");
        if (sst == 0)
            return_error(gs_error_VMerror);
    }
    code = file_open_stream((char *)0, 0, file_access, buffer_size, &s,
                                (gx_io_device *)0, (iodev_proc_fopen_t)0, mem);
    if (code < 0) {
        gs_free_object(mem, sst, "filter_open(stream_state)");
        return code;
    }
    s_std_init(s, s->cbuf, s->bsize, procs,
               (*file_access == 'r' ? s_mode_read : s_mode_write));
    s->procs.process = templat->process;
    s->save_close = s->procs.close;
    s->procs.close = file_close_file;
    if (sst == 0) {
        /* This stream doesn't have any state of its own. */
        /* Hack: use the stream itself as the state. */
        sst = (stream_state *) s;
    } else if (st != 0)         /* might not have client parameters */
        memcpy(sst, st, ssize);
    s->state = sst;
    s_init_state(sst, templat, mem);
    sst->report_error = filter_report_error;

    if (templat->init != 0) {
        code = (*templat->init)(sst);
        if (code < 0) {
            gs_free_object(mem, sst, "filter_open(stream_state)");
            gs_free_object(mem, s->cbuf, "filter_open(buffer)");
            return code;
        }
    }
    make_stream_file(pfile, s, file_access);
    return 0;
}