void* chk_memalign(size_t alignment, size_t bytes)
{
    // XXX: it's better to use malloc, than being wrong
    return chk_malloc(bytes);
}