static int check_header_variable(EXRContext *s,
                                 const char *value_name,
                                 const char *value_type,
                                 unsigned int minimum_length)
{
    GetByteContext *gb = &s->gb;
    int var_size = -1;

    if (bytestream2_get_bytes_left(gb) >= minimum_length &&
        !strcmp(gb->buffer, value_name)) {
        // found value_name, jump to value_type (null terminated strings)
        gb->buffer += strlen(value_name) + 1;
        if (!strcmp(gb->buffer, value_type)) {
            gb->buffer += strlen(value_type) + 1;
            var_size = bytestream2_get_le32(gb);
            // don't go read past boundaries
            if (var_size > bytestream2_get_bytes_left(gb))
                var_size = 0;
        } else {
            // value_type not found, reset the buffer
            gb->buffer -= strlen(value_name) + 1;
            av_log(s->avctx, AV_LOG_WARNING,
                   "Unknown data type %s for header variable %s.\n",
                   value_type, value_name);
        }
    }

    return var_size;
}