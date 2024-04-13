MOBI_RET mobi_parse_record0(MOBIData *m, const size_t seqnumber) {
    MOBI_RET ret;
    if (m == NULL) {
        debug_print("%s", "Mobi structure not initialized\n");
        return MOBI_INIT_FAILED;
    }
    const MOBIPdbRecord *record0 = mobi_get_record_by_seqnumber(m, seqnumber);
    if (record0 == NULL) {
        debug_print("%s", "Record 0 not loaded\n");
        return MOBI_DATA_CORRUPT;
    }
    if (record0->size < RECORD0_HEADER_LEN) {
        debug_print("%s", "Record 0 too short\n");
        return MOBI_DATA_CORRUPT;
    }
    MOBIBuffer *buf = mobi_buffer_init_null(record0->data, record0->size);
    if (buf == NULL) {
        debug_print("%s\n", "Memory allocation failed");
        return MOBI_MALLOC_FAILED;
    }
    m->rh = calloc(1, sizeof(MOBIRecord0Header));
    if (m->rh == NULL) {
        debug_print("%s", "Memory allocation for record 0 header failed\n");
        mobi_buffer_free_null(buf);
        return MOBI_MALLOC_FAILED;
    }
    /* parse palmdoc header */
    const uint16_t compression = mobi_buffer_get16(buf);
    mobi_buffer_seek(buf, 2); // unused 2 bytes, zeroes
    if ((compression != RECORD0_NO_COMPRESSION &&
         compression != RECORD0_PALMDOC_COMPRESSION &&
         compression != RECORD0_HUFF_COMPRESSION)) {
        debug_print("Wrong record0 header: %c%c%c%c\n", record0->data[0], record0->data[1], record0->data[2], record0->data[3]);
        mobi_buffer_free_null(buf);
        free(m->rh);
        m->rh = NULL;
        return MOBI_DATA_CORRUPT;
    }
    m->rh->compression_type = compression;
    m->rh->text_length = mobi_buffer_get32(buf);
    m->rh->text_record_count = mobi_buffer_get16(buf);
    m->rh->text_record_size = mobi_buffer_get16(buf);
    m->rh->encryption_type = mobi_buffer_get16(buf);
    m->rh->unknown1 = mobi_buffer_get16(buf);
    if (mobi_is_mobipocket(m)) {
        /* parse mobi header if present  */
        ret = mobi_parse_mobiheader(m, buf);
        if (ret == MOBI_SUCCESS) {
            /* parse exth header if present */
            mobi_parse_extheader(m, buf);
        }
    } 
    mobi_buffer_free_null(buf);
    return MOBI_SUCCESS;
}