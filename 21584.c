MOBI_RET mobi_parse_cdic(MOBIHuffCdic *huffcdic, const MOBIPdbRecord *record, const size_t num) {
    MOBIBuffer *buf = mobi_buffer_init_null(record->data, record->size);
    if (buf == NULL) {
        debug_print("%s\n", "Memory allocation failed");
        return MOBI_MALLOC_FAILED;
    }
    char cdic_magic[5];
    mobi_buffer_getstring(cdic_magic, buf, 4);
    const size_t header_length = mobi_buffer_get32(buf);
    if (strncmp(cdic_magic, CDIC_MAGIC, 4) != 0 || header_length < CDIC_HEADER_LEN) {
        debug_print("CDIC wrong magic: %s or declared header length: %zu\n", cdic_magic, header_length);
        mobi_buffer_free_null(buf);
        return MOBI_DATA_CORRUPT;
    }
    /* variables in huffcdic initialized to zero with calloc */
    /* save initial count and length */
    size_t index_count = mobi_buffer_get32(buf);
    const size_t code_length = mobi_buffer_get32(buf);
    if (huffcdic->code_length && huffcdic->code_length != code_length) {
        debug_print("CDIC different code length %zu in record %i, previous was %zu\n", huffcdic->code_length, record->uid, code_length);
        mobi_buffer_free_null(buf);
        return MOBI_DATA_CORRUPT;
    }
    if (huffcdic->index_count && huffcdic->index_count != index_count) {
        debug_print("CDIC different index count %zu in record %i, previous was %zu\n", huffcdic->index_count, record->uid, index_count);
        mobi_buffer_free_null(buf);
        return MOBI_DATA_CORRUPT;
    }
    if (code_length == 0 || code_length > HUFF_CODELEN_MAX) {
        debug_print("Code length exceeds sanity checks (%zu)\n", code_length);
        mobi_buffer_free_null(buf);
        return MOBI_DATA_CORRUPT;
    }
    huffcdic->code_length = code_length;
    huffcdic->index_count = index_count;
    if (index_count == 0) {
        debug_print("%s", "CDIC index count is null");
        mobi_buffer_free_null(buf);
        return MOBI_DATA_CORRUPT;
    }
    /* allocate memory for symbol offsets if not already allocated */
    if (num == 0) {
        if (index_count > (1 << HUFF_CODELEN_MAX) * CDIC_RECORD_MAXCNT) {
            debug_print("CDIC index count too large %zu\n", index_count);
            mobi_buffer_free_null(buf);
            return MOBI_DATA_CORRUPT;
        }
        huffcdic->symbol_offsets = malloc(index_count * sizeof(*huffcdic->symbol_offsets));
        if (huffcdic->symbol_offsets == NULL) {
            debug_print("%s", "CDIC cannot allocate memory");
            mobi_buffer_free_null(buf);
            return MOBI_MALLOC_FAILED;
        }
    }
    index_count -= huffcdic->index_read;
    /* limit number of records read to code_length bits */
    if (index_count >> code_length) {
        index_count = (1 << code_length);
    }
    if (buf->offset + (index_count * 2) > buf->maxlen) {
        debug_print("%s", "CDIC indices data too short\n");
        mobi_buffer_free_null(buf);
        return MOBI_DATA_CORRUPT;
    }
    /* read i * 2 byte big-endian indices */
    while (index_count--) {
        const uint16_t offset = mobi_buffer_get16(buf);
        const size_t saved_pos = buf->offset;
        mobi_buffer_setpos(buf, offset + CDIC_HEADER_LEN);
        const size_t len = mobi_buffer_get16(buf) & 0x7fff;
        if (buf->error != MOBI_SUCCESS || buf->offset + len > buf->maxlen) {
            debug_print("%s", "CDIC offset beyond buffer\n");
            mobi_buffer_free_null(buf);
            return MOBI_DATA_CORRUPT;
        }
        mobi_buffer_setpos(buf, saved_pos);
        huffcdic->symbol_offsets[huffcdic->index_read++] = offset;
    }
    if (buf->offset + code_length > buf->maxlen) {
        debug_print("%s", "CDIC dictionary data too short\n");
        mobi_buffer_free_null(buf);
        return MOBI_DATA_CORRUPT;
    }
    /* copy pointer to data */
    huffcdic->symbols[num] = record->data + CDIC_HEADER_LEN;
    /* free buffer */
    mobi_buffer_free_null(buf);
    return MOBI_SUCCESS;
}