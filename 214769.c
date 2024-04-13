static bool checkreturn pb_dec_uvarint(pb_istream_t *stream, const pb_field_t *field, void *dest)
{
    pb_uint64_t value, clamped;
    if (!pb_decode_varint(stream, &value))
        return false;
    
    /* Cast to the proper field size, while checking for overflows */
    if (field->data_size == sizeof(pb_uint64_t))
        clamped = *(pb_uint64_t*)dest = value;
    else if (field->data_size == sizeof(uint32_t))
        clamped = *(uint32_t*)dest = (uint32_t)value;
    else if (field->data_size == sizeof(uint_least16_t))
        clamped = *(uint_least16_t*)dest = (uint_least16_t)value;
    else if (field->data_size == sizeof(uint_least8_t))
        clamped = *(uint_least8_t*)dest = (uint_least8_t)value;
    else
        PB_RETURN_ERROR(stream, "invalid data_size");
    
    if (clamped != value)
        PB_RETURN_ERROR(stream, "integer too large");

    return true;
}