static bool checkreturn pb_dec_fixed_length_bytes(pb_istream_t *stream, const pb_field_t *field, void *dest)
{
    uint32_t size;

    if (!pb_decode_varint32(stream, &size))
        return false;

    if (size > PB_SIZE_MAX)
        PB_RETURN_ERROR(stream, "bytes overflow");

    if (size == 0)
    {
        /* As a special case, treat empty bytes string as all zeros for fixed_length_bytes. */
        memset(dest, 0, field->data_size);
        return true;
    }

    if (size != field->data_size)
        PB_RETURN_ERROR(stream, "incorrect fixed length bytes size");

    return pb_read(stream, (pb_byte_t*)dest, field->data_size);
}