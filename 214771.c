bool pb_decode_delimited_noinit(pb_istream_t *stream, const pb_field_t fields[], void *dest_struct)
{
    pb_istream_t substream;
    bool status;

    if (!pb_make_string_substream(stream, &substream))
        return false;

    status = pb_decode_noinit(&substream, fields, dest_struct);

    if (!pb_close_string_substream(stream, &substream))
        return false;
    return status;
}