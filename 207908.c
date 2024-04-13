void  chk_free(void* mem)
{
    assert_valid_malloc_pointer(mem);
    if (mem) {
        size_t  size;
        char*   buffer;

        if (chk_mem_check(mem, &size, "FREE") == 0) {
            buffer = (char*)mem - CHK_SENTINEL_HEAD_SIZE;
            memset(buffer, CHK_FILL_FREE, size + CHK_OVERHEAD_SIZE);
            dlfree(buffer);
        }
    }
}