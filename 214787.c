static bool checkreturn pb_dec_fixed64(pb_istream_t *stream, const pb_field_t *field, void *dest)
{
    PB_UNUSED(field);
#ifndef PB_WITHOUT_64BIT
    return pb_decode_fixed64(stream, dest);
#else
    PB_UNUSED(dest);
    PB_RETURN_ERROR(stream, "no 64bit support");
#endif
}