static bool checkreturn pb_dec_bool(pb_istream_t *stream, const pb_field_t *field, void *dest)
{
    uint32_t value;
    PB_UNUSED(field);
    if (!pb_decode_varint32(stream, &value))
        return false;

    *(bool*)dest = (value != 0);
    return true;
}