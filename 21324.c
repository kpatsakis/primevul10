static void vnc_free_addresses(SocketAddress ***retsaddr,
                               size_t *retnsaddr)
{
    size_t i;

    for (i = 0; i < *retnsaddr; i++) {
        qapi_free_SocketAddress((*retsaddr)[i]);
    }
    g_free(*retsaddr);

    *retsaddr = NULL;
    *retnsaddr = 0;
}