void  fill_free(void* mem)
{
    size_t bytes = dlmalloc_usable_size(mem);
    memset(mem, CHK_FILL_FREE, bytes);
    dlfree(mem);
}