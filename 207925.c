void* fill_memalign(size_t alignment, size_t bytes)
{
    void* buffer = dlmemalign(alignment, bytes);
    if (buffer) {
        memset(buffer, CHK_SENTINEL_VALUE, bytes);
    }
    return buffer;
}