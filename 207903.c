void* leak_calloc(size_t n_elements, size_t elem_size)
{
    size_t  size;
    void*   ptr;

    /* Fail on overflow - just to be safe even though this code runs only
     * within the debugging C library, not the production one */
    if (n_elements && MAX_SIZE_T / n_elements < elem_size) {
        return NULL;
    }
    size = n_elements * elem_size;
    ptr  = leak_malloc(size);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    }
    return ptr;
}