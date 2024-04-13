MOBI_RET mobi_parse_extheader(MOBIData *m, MOBIBuffer *buf) {
    if (m == NULL) {
        debug_print("%s", "Mobi structure not initialized\n");
        return MOBI_INIT_FAILED;
    }
    char exth_magic[5];
    const size_t header_length = 12;
    mobi_buffer_getstring(exth_magic, buf, 4);
    const size_t exth_length = mobi_buffer_get32(buf) - header_length;
    const size_t rec_count = mobi_buffer_get32(buf);
    if (strncmp(exth_magic, EXTH_MAGIC, 4) != 0 ||
        exth_length + buf->offset > buf->maxlen ||
        rec_count == 0 || rec_count > MOBI_EXTH_MAXCNT) {
        debug_print("%s", "Sanity checks for EXTH header failed\n");
        return MOBI_DATA_CORRUPT;
    }
    const size_t saved_maxlen = buf->maxlen;
    buf->maxlen = exth_length + buf->offset;
    m->eh = calloc(1, sizeof(MOBIExthHeader));
    if (m->eh == NULL) {
        debug_print("%s", "Memory allocation for EXTH header failed\n");
        return MOBI_MALLOC_FAILED;
    }
    MOBIExthHeader *curr = m->eh;
    for (size_t i = 0; i < rec_count; i++) {
        if (curr->data) {
            curr->next = calloc(1, sizeof(MOBIExthHeader));
            if (curr->next == NULL) {
                debug_print("%s", "Memory allocation for EXTH header failed\n");
                mobi_free_eh(m);
                return MOBI_MALLOC_FAILED;
            }
            curr = curr->next;
        }
        curr->tag = mobi_buffer_get32(buf);
        /* data size = record size minus 8 bytes for uid and size */
        curr->size = mobi_buffer_get32(buf) - 8;
        if (curr->size == 0) {
            debug_print("Skip record %i, data too short\n", curr->tag);
            continue;
        }
        if (buf->offset + curr->size > buf->maxlen) {
            debug_print("Record %i too long\n", curr->tag);
            mobi_free_eh(m);
            return MOBI_DATA_CORRUPT;
        }
        curr->data = malloc(curr->size);
        if (curr->data == NULL) {
            debug_print("Memory allocation for EXTH record %i failed\n", curr->tag);
            mobi_free_eh(m);
            return MOBI_MALLOC_FAILED;
        }
        mobi_buffer_getraw(curr->data, buf, curr->size);
        curr->next = NULL;
    }    
    buf->maxlen = saved_maxlen;
    return MOBI_SUCCESS;
}