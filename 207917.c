void* fill_malloc(size_t bytes)
{
    void* buffer = dlmalloc(bytes);
    if (buffer) {
        memset(buffer, CHK_SENTINEL_VALUE, bytes);
    }
    return buffer;
}