MOBI_RET mobi_load_recdata(MOBIPdbRecord *rec, FILE *file) {
    const int ret = fseek(file, rec->offset, SEEK_SET);
    if (ret != 0) {
        debug_print("Record %i not found\n", rec->uid);
        return MOBI_DATA_CORRUPT;
    }
    rec->data = malloc(rec->size);
    if (rec->data == NULL) {
        debug_print("%s", "Memory allocation for pdb record data failed\n");
        return MOBI_MALLOC_FAILED;
    }
    const size_t len = fread(rec->data, 1, rec->size, file);
    if (len < rec->size) {
        debug_print("Truncated data in record %i\n", rec->uid);
        return MOBI_DATA_CORRUPT;
    }
    return MOBI_SUCCESS;
}