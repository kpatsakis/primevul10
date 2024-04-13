void* chk_realloc(void* mem, size_t bytes)
{
    char*   buffer;
    int     ret;
    size_t  old_bytes = 0;

    assert_valid_malloc_pointer(mem);

    if (mem != NULL && chk_mem_check(mem, &old_bytes, "REALLOC") < 0)
        return NULL;

    char* new_buffer = chk_malloc(bytes);
    if (mem == NULL) {
        return new_buffer;
    }

    if (new_buffer) {
        if (bytes > old_bytes)
            bytes = old_bytes;
        memcpy(new_buffer, mem, bytes);
        chk_free(mem);
    }

    return new_buffer;
}