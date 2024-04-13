make_stream_file(ref * pfile, stream * s, const char *access)
{
    uint attrs =
        (access[1] == '+' ? a_write + a_read + a_execute : 0) |
        imemory_space((gs_ref_memory_t *) s->memory);

    if (access[0] == 'r') {
        make_file(pfile, attrs | (a_read | a_execute), s->read_id, s);
        s->write_id = 0;
    } else {
        make_file(pfile, attrs | a_write, s->write_id, s);
        s->read_id = 0;
    }
}