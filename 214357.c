static APR_INLINE int do_double_reverse (int double_reverse,
                                         const char *remote_host,
                                         apr_sockaddr_t *client_addr,
                                         apr_pool_t *pool)
{
    apr_sockaddr_t *sa;
    apr_status_t rv;

    if (double_reverse) {
        /* already done */
        return double_reverse;
    }

    if (remote_host == NULL || remote_host[0] == '\0') {
        /* single reverse failed, so don't bother */
        return -1;
    }

    rv = apr_sockaddr_info_get(&sa, remote_host, APR_UNSPEC, 0, 0, pool);
    if (rv == APR_SUCCESS) {
        while (sa) {
            if (apr_sockaddr_equal(sa, client_addr)) {
                return 1;
            }

            sa = sa->next;
        }
    }

    return -1;
}