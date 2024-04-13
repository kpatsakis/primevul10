void* fill_realloc(void* mem, size_t bytes)
{
    void* buffer = fill_malloc(bytes);
    if (mem == NULL) {
        return buffer;
    }
    if (buffer) {
        size_t old_size = dlmalloc_usable_size(mem);
        size_t size = (bytes < old_size)?(bytes):(old_size);
        memcpy(buffer, mem, size);
        fill_free(mem);
    }
    return buffer;
}