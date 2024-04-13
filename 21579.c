MOBI_RET mobi_load_filename(MOBIData *m, const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        debug_print("%s", "File not found\n");
        return MOBI_FILE_NOT_FOUND;
    }
    const MOBI_RET ret = mobi_load_file(m, file);
    fclose(file);
    return ret;
}