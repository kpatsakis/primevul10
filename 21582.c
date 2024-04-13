MOBI_RET mobi_load_pdbheader(MOBIData *m, FILE *file) {
    if (m == NULL) {
        debug_print("%s", "Mobi structure not initialized\n");
        return MOBI_INIT_FAILED;
    }
    if (!file) {
        return MOBI_FILE_NOT_FOUND;
    }
    MOBIBuffer *buf = mobi_buffer_init(PALMDB_HEADER_LEN);
    if (buf == NULL) {
        debug_print("%s\n", "Memory allocation failed");
        return MOBI_MALLOC_FAILED;
    }
    const size_t len = fread(buf->data, 1, PALMDB_HEADER_LEN, file);
    if (len != PALMDB_HEADER_LEN) {
        mobi_buffer_free(buf);
        return MOBI_DATA_CORRUPT;
    }
    m->ph = calloc(1, sizeof(MOBIPdbHeader));
    if (m->ph == NULL) {
        debug_print("%s", "Memory allocation for pdb header failed\n");
        mobi_buffer_free(buf);
        return MOBI_MALLOC_FAILED;
    }
    /* parse header */
    mobi_buffer_getstring(m->ph->name, buf, PALMDB_NAME_SIZE_MAX);
    m->ph->attributes = mobi_buffer_get16(buf);
    m->ph->version = mobi_buffer_get16(buf);
    m->ph->ctime = mobi_buffer_get32(buf);
    m->ph->mtime = mobi_buffer_get32(buf);
    m->ph->btime = mobi_buffer_get32(buf);
    m->ph->mod_num = mobi_buffer_get32(buf);
    m->ph->appinfo_offset = mobi_buffer_get32(buf);
    m->ph->sortinfo_offset = mobi_buffer_get32(buf);
    mobi_buffer_getstring(m->ph->type, buf, 4);
    mobi_buffer_getstring(m->ph->creator, buf, 4);
    m->ph->uid = mobi_buffer_get32(buf);
    m->ph->next_rec = mobi_buffer_get32(buf);
    m->ph->rec_count = mobi_buffer_get16(buf);
    mobi_buffer_free(buf);
    return MOBI_SUCCESS;
}