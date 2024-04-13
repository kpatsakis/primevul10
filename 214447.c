static conn_rec *core_create_conn(apr_pool_t *ptrans, server_rec *s,
                                  apr_socket_t *csd, long id, void *sbh,
                                  apr_bucket_alloc_t *alloc)
{
    apr_status_t rv;
    apr_pool_t *pool;
    conn_rec *c = (conn_rec *) apr_pcalloc(ptrans, sizeof(conn_rec));
    core_server_config *sconf = ap_get_core_module_config(s->module_config);

    c->sbh = sbh;
    ap_update_child_status(c->sbh, SERVER_BUSY_READ, NULL);

    /* Got a connection structure, so initialize what fields we can
     * (the rest are zeroed out by pcalloc).
     */
    apr_pool_create(&pool, ptrans);
    apr_pool_tag(pool, "master_conn");
    c->pool = pool;

    c->conn_config = ap_create_conn_config(c->pool);
    c->notes = apr_table_make(c->pool, 5);
    c->slaves = apr_array_make(c->pool, 20, sizeof(conn_slave_rec *));
    c->requests = apr_array_make(c->pool, 20, sizeof(request_rec *));


    if ((rv = apr_socket_addr_get(&c->local_addr, APR_LOCAL, csd))
        != APR_SUCCESS) {
        ap_log_error(APLOG_MARK, APLOG_INFO, rv, s, APLOGNO(00137)
                     "apr_socket_addr_get(APR_LOCAL)");
        apr_socket_close(csd);
        return NULL;
    }

    apr_sockaddr_ip_get(&c->local_ip, c->local_addr);
    if ((rv = apr_socket_addr_get(&c->client_addr, APR_REMOTE, csd))
        != APR_SUCCESS) {
        ap_log_error(APLOG_MARK, APLOG_INFO, rv, s, APLOGNO(00138)
                     "apr_socket_addr_get(APR_REMOTE)");
        apr_socket_close(csd);
        return NULL;
    }

    apr_sockaddr_ip_get(&c->client_ip, c->client_addr);
    c->base_server = s;

    c->id = id;
    c->bucket_alloc = alloc;
    c->empty = apr_brigade_create(c->pool, c->bucket_alloc);
    c->filters = apr_hash_make(c->pool);
    c->async_filter = sconf->async_filter;

    c->clogging_input_filters = 0;

    if (sconf->conn_log_level) {
        int i;
        conn_log_config *conf;
        const struct ap_logconf *log = NULL;
        struct ap_logconf *merged;

        for (i = 0; i < sconf->conn_log_level->nelts; i++) {
            conf = APR_ARRAY_IDX(sconf->conn_log_level, i, conn_log_config *);
            if (apr_ipsubnet_test(conf->subnet, c->client_addr))
                log = &conf->log;
        }
        if (log) {
            merged = ap_new_log_config(c->pool, log);
            ap_merge_log_config(&s->log, merged);
            c->log = merged;
        }
    }

    return c;
}