bool pb_decode_nullterminated(pb_istream_t *stream, const pb_field_t fields[], void *dest_struct)
{
    /* This behaviour will be separated in nanopb-0.4.0, see issue #278. */
    return pb_decode(stream, fields, dest_struct);
}