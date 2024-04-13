void vnc_zlib_zfree(void *x, void *addr)
{
    g_free(addr);
}