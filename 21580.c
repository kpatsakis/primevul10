size_t mobi_get_record_mb_extrasize(const MOBIPdbRecord *record, const uint16_t flags) {
    size_t extra_size = 0;
    if (flags & 1) {
        MOBIBuffer *buf = mobi_buffer_init_null(record->data, record->size);
        if (buf == NULL) {
            debug_print("%s", "Buffer init in extrasize failed\n");
            return MOBI_NOTSET;
        }
        /* set pointer at the end of the record data */
        mobi_buffer_setpos(buf, buf->maxlen - 1);
        for (int bit = 15; bit > 0; bit--) {
            if (flags & (1 << bit)) {
                /* bit is set */
                size_t len = 0;
                /* size contains varlen itself and optional data */
                const uint32_t size = mobi_buffer_get_varlen_dec(buf, &len);
                /* skip data */
                /* TODO: read and store in record struct */
                mobi_buffer_seek(buf, - (int)(size - len));
            }
        };
        /* read multibyte section */
        const uint8_t b = mobi_buffer_get8(buf);
        /* two first bits hold size */
        extra_size += (b & 0x3) + 1;
        mobi_buffer_free_null(buf);
    }
    return extra_size;
}