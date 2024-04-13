static int chk_mem_check(void*       mem,
                         size_t*     allocated,
                         const char* func)
{
    char*  buffer;
    size_t offset, bytes;
    int    i;
    char*  buf;

    /* first check the bytes in the sentinel header */
    buf = (char*)mem - CHK_SENTINEL_HEAD_SIZE;
    for (i=0 ; i<CHK_SENTINEL_HEAD_SIZE ; i++) {
        if (buf[i] != CHK_SENTINEL_VALUE) {
            assert_log_message(
                "*** %s CHECK: buffer %p "
                "corrupted %d bytes before allocation",
                func, mem, CHK_SENTINEL_HEAD_SIZE-i);
            return -1;
        }
    }

    /* then the ones in the sentinel trailer */
    buffer = (char*)mem - CHK_SENTINEL_HEAD_SIZE;
    offset = dlmalloc_usable_size(buffer) - sizeof(size_t);
    bytes  = *(size_t *)(buffer + offset);

    buf = (char*)mem + bytes;
    for (i=CHK_SENTINEL_TAIL_SIZE-1 ; i>=0 ; i--) {
        if (buf[i] != CHK_SENTINEL_VALUE) {
            assert_log_message(
                "*** %s CHECK: buffer %p, size=%lu, "
                "corrupted %d bytes after allocation",
                func, buffer, bytes, i+1);
            return -1;
        }
    }

    *allocated = bytes;
    return 0;
}