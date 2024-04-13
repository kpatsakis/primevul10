MOBI_RET mobi_load_reclist(MOBIData *m, FILE *file) {
    if (m == NULL) {
        debug_print("%s", "Mobi structure not initialized\n");
        return MOBI_INIT_FAILED;
    }
    if (!file) {
        debug_print("%s", "File not ready\n");
        return MOBI_FILE_NOT_FOUND;
    }
    m->rec = calloc(1, sizeof(MOBIPdbRecord));
    if (m->rec == NULL) {
        debug_print("%s", "Memory allocation for pdb record failed\n");
        return MOBI_MALLOC_FAILED;
    }
    MOBIPdbRecord *curr = m->rec;
    for (int i = 0; i < m->ph->rec_count; i++) {
        MOBIBuffer *buf = mobi_buffer_init(PALMDB_RECORD_INFO_SIZE);
        if (buf == NULL) {
            debug_print("%s\n", "Memory allocation failed");
            return MOBI_MALLOC_FAILED;
        }
        const size_t len = fread(buf->data, 1, PALMDB_RECORD_INFO_SIZE, file);
        if (len != PALMDB_RECORD_INFO_SIZE) {
            mobi_buffer_free(buf);
            return MOBI_DATA_CORRUPT;
        }
        if (i > 0) {
            curr->next = calloc(1, sizeof(MOBIPdbRecord));
            if (curr->next == NULL) {
                debug_print("%s", "Memory allocation for pdb record failed\n");
                mobi_buffer_free(buf);
                return MOBI_MALLOC_FAILED;
            }
            curr = curr->next;
        }
        curr->offset = mobi_buffer_get32(buf);
        curr->attributes = mobi_buffer_get8(buf);
        const uint8_t h = mobi_buffer_get8(buf);
        const uint16_t l = mobi_buffer_get16(buf);
        curr->uid =  (uint32_t) h << 16 | l;
        curr->next = NULL;
        mobi_buffer_free(buf);
    }
    return MOBI_SUCCESS;
}