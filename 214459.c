AP_DECLARE(const char *) ap_get_remote_host(conn_rec *conn, void *dir_config,
                                            int type, int *str_is_ip)
{
    int hostname_lookups;
    int ignored_str_is_ip;

    if (!str_is_ip) { /* caller doesn't want to know */
        str_is_ip = &ignored_str_is_ip;
    }
    *str_is_ip = 0;

    /* If we haven't checked the host name, and we want to */
    if (dir_config) {
        hostname_lookups = ((core_dir_config *)ap_get_core_module_config(dir_config))
                           ->hostname_lookups;

        if (hostname_lookups == HOSTNAME_LOOKUP_UNSET) {
            hostname_lookups = HOSTNAME_LOOKUP_OFF;
        }
    }
    else {
        /* the default */
        hostname_lookups = HOSTNAME_LOOKUP_OFF;
    }

    if (type != REMOTE_NOLOOKUP
        && conn->remote_host == NULL
        && (type == REMOTE_DOUBLE_REV
        || hostname_lookups != HOSTNAME_LOOKUP_OFF)) {

        if (apr_getnameinfo(&conn->remote_host, conn->client_addr, 0)
            == APR_SUCCESS) {
            ap_str_tolower(conn->remote_host);

            if (hostname_lookups == HOSTNAME_LOOKUP_DOUBLE) {
                conn->double_reverse = do_double_reverse(conn->double_reverse,
                                                         conn->remote_host,
                                                         conn->client_addr,
                                                         conn->pool);
                if (conn->double_reverse != 1) {
                    conn->remote_host = NULL;
                }
            }
        }

        /* if failed, set it to the NULL string to indicate error */
        if (conn->remote_host == NULL) {
            conn->remote_host = "";
        }
    }

    if (type == REMOTE_DOUBLE_REV) {
        conn->double_reverse = do_double_reverse(conn->double_reverse,
                                                 conn->remote_host,
                                                 conn->client_addr, conn->pool);
        if (conn->double_reverse == -1) {
            return NULL;
        }
    }

    /*
     * Return the desired information; either the remote DNS name, if found,
     * or either NULL (if the hostname was requested) or the IP address
     * (if any identifier was requested).
     */
    if (conn->remote_host != NULL && conn->remote_host[0] != '\0') {
        return conn->remote_host;
    }
    else {
        if (type == REMOTE_HOST || type == REMOTE_DOUBLE_REV) {
            return NULL;
        }
        else {
            *str_is_ip = 1;
            return conn->client_ip;
        }
    }
}