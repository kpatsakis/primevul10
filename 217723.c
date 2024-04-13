format_hex_arg(struct ds *s, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (i) {
            ds_put_char(s, '.');
        }
        ds_put_format(s, "%02"PRIx8, data[i]);
    }
}