AP_DECLARE(const char *) ap_get_useragent_host(request_rec *r,
                                               int type, int *str_is_ip)
{
    conn_rec *conn = r->connection;
    int hostname_lookups;
    int ignored_str_is_ip;

    /* Guard here when examining the host before the read_request hook
     * has populated an r->useragent_addr
     */
    if (!r->useragent_addr || (r->useragent_addr == conn->client_addr)) {
        return ap_get_remote_host(conn, r->per_dir_config, type, str_is_ip);
    }

    if (!str_is_ip) { /* caller doesn't want to know */
        str_is_ip = &ignored_str_is_ip;
    }
    *str_is_ip = 0;

    hostname_lookups = ((core_dir_config *)
                        ap_get_core_module_config(r->per_dir_config))
                            ->hostname_lookups;
    if (hostname_lookups == HOSTNAME_LOOKUP_UNSET) {
        hostname_lookups = HOSTNAME_LOOKUP_OFF;
    }

    if (type != REMOTE_NOLOOKUP
        && r->useragent_host == NULL
        && (type == REMOTE_DOUBLE_REV
        || hostname_lookups != HOSTNAME_LOOKUP_OFF)) {

        if (apr_getnameinfo(&r->useragent_host, r->useragent_addr, 0)
            == APR_SUCCESS) {
            ap_str_tolower(r->useragent_host);

            if (hostname_lookups == HOSTNAME_LOOKUP_DOUBLE) {
                r->double_reverse = do_double_reverse(r->double_reverse,
                                                      r->useragent_host,
                                                      r->useragent_addr,
                                                      r->pool);
                if (r->double_reverse != 1) {
                    r->useragent_host = NULL;
                }
            }
        }

        /* if failed, set it to the NULL string to indicate error */
        if (r->useragent_host == NULL) {
            r->useragent_host = "";
        }
    }

    if (type == REMOTE_DOUBLE_REV) {
        r->double_reverse = do_double_reverse(r->double_reverse,
                                              r->useragent_host,
                                              r->useragent_addr, r->pool);
        if (r->double_reverse == -1) {
            return NULL;
        }
    }

    /*
     * Return the desired information; either the remote DNS name, if found,
     * or either NULL (if the hostname was requested) or the IP address
     * (if any identifier was requested).
     */
    if (r->useragent_host != NULL && r->useragent_host[0] != '\0') {
        return r->useragent_host;
    }
    else {
        if (type == REMOTE_HOST || type == REMOTE_DOUBLE_REV) {
            return NULL;
        }
        else {
            *str_is_ip = 1;
            return r->useragent_ip;
        }
    }
}