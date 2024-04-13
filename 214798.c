static bool checkreturn pb_dec_svarint(pb_istream_t *stream, const pb_field_t *field, void *dest)
{
    pb_int64_t value, clamped;
    if (!pb_decode_svarint(stream, &value))
        return false;
    
    /* Cast to the proper field size, while checking for overflows */
    if (field->data_size == sizeof(pb_int64_t))
        clamped = *(pb_int64_t*)dest = value;
    else if (field->data_size == sizeof(int32_t))
        clamped = *(int32_t*)dest = (int32_t)value;
    else if (field->data_size == sizeof(int_least16_t))
        clamped = *(int_least16_t*)dest = (int_least16_t)value;
    else if (field->data_size == sizeof(int_least8_t))
        clamped = *(int_least8_t*)dest = (int_least8_t)value;
    else
        PB_RETURN_ERROR(stream, "invalid data_size");

    if (clamped != value)
        PB_RETURN_ERROR(stream, "integer too large");
    
    return true;
}