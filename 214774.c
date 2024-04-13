bool pb_decode_bool(pb_istream_t *stream, bool *dest)
{
    return pb_dec_bool(stream, NULL, (void*)dest);
}