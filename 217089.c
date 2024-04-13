lib_fopen(const gs_file_path_ptr pfpath, const gs_memory_t *mem, const char *fname)
{
    /* We need a buffer to hold the expanded file name. */
    char filename_found[DEFAULT_BUFFER_SIZE];
    FILE *file = NULL;
    uint fnamelen;
    ref obj;
    int code;

    /* open the usual 'stream', then if successful, return the file */
    code = lib_file_open(pfpath, mem, NULL, fname, strlen(fname),
                            filename_found, sizeof(filename_found), &fnamelen, &obj);

    if (code < 0)
        return NULL;
    file = ((stream *)(obj.value.pfile))->file;
    return file;
}