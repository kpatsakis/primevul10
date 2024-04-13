MOBI_RET mobi_parse_fdst(const MOBIData *m, MOBIRawml *rawml) {
    if (m == NULL) {
        debug_print("%s", "Mobi structure not initialized\n");
        return MOBI_INIT_FAILED;
    }
    const size_t fdst_record_number = mobi_get_fdst_record_number(m);
    if (fdst_record_number == MOBI_NOTSET) {
        return MOBI_DATA_CORRUPT;
    }
    const MOBIPdbRecord *fdst_record = mobi_get_record_by_seqnumber(m, fdst_record_number);
    if (fdst_record == NULL) {
        return MOBI_DATA_CORRUPT;
    }
    MOBIBuffer *buf = mobi_buffer_init_null(fdst_record->data, fdst_record->size);
    if (buf == NULL) {
        debug_print("%s\n", "Memory allocation failed");
        return MOBI_MALLOC_FAILED;
    }
    char fdst_magic[5];
    mobi_buffer_getstring(fdst_magic, buf, 4);
    const size_t data_offset = mobi_buffer_get32(buf);
    const size_t section_count = mobi_buffer_get32(buf);
    if (strncmp(fdst_magic, FDST_MAGIC, 4) != 0 ||
        section_count <= 1 ||
        section_count != *m->mh->fdst_section_count ||
        data_offset != 12) {
        debug_print("FDST wrong magic: %s, sections count: %zu or data offset: %zu\n", fdst_magic, section_count, data_offset);
        mobi_buffer_free_null(buf);
        return MOBI_DATA_CORRUPT;
    }
    if ((buf->maxlen - buf->offset) < section_count * 8) {
        debug_print("%s", "Record FDST too short\n");
        mobi_buffer_free_null(buf);
        return MOBI_DATA_CORRUPT;
    }
    rawml->fdst = malloc(sizeof(MOBIFdst));
    if (rawml->fdst == NULL) {
        debug_print("%s\n", "Memory allocation failed");
        mobi_buffer_free_null(buf);
        return MOBI_MALLOC_FAILED;
    }
    rawml->fdst->fdst_section_count = section_count;
    rawml->fdst->fdst_section_starts = malloc(sizeof(*rawml->fdst->fdst_section_starts) * section_count);
    if (rawml->fdst->fdst_section_starts == NULL) {
        debug_print("%s\n", "Memory allocation failed");
        mobi_buffer_free_null(buf);
        free(rawml->fdst);
        rawml->fdst = NULL;
        return MOBI_MALLOC_FAILED;
    }
    rawml->fdst->fdst_section_ends = malloc(sizeof(*rawml->fdst->fdst_section_ends) * section_count);
    if (rawml->fdst->fdst_section_ends == NULL) {
        debug_print("%s\n", "Memory allocation failed");
        mobi_buffer_free_null(buf);
        free(rawml->fdst->fdst_section_starts);
        free(rawml->fdst);
        rawml->fdst = NULL;
        return MOBI_MALLOC_FAILED;
    }
    size_t i = 0;
    while (i < section_count) {
        rawml->fdst->fdst_section_starts[i] = mobi_buffer_get32(buf);
        rawml->fdst->fdst_section_ends[i] = mobi_buffer_get32(buf);
        debug_print("FDST[%zu]:\t%i\t%i\n", i, rawml->fdst->fdst_section_starts[i], rawml->fdst->fdst_section_ends[i]);
        i++;
    }
    mobi_buffer_free_null(buf);
    return MOBI_SUCCESS;
}