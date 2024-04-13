static int core_pre_connection(conn_rec *c, void *csd)
{
    core_net_rec *net;
    apr_status_t rv;

    if (c->master) {
        return DONE;
    }
    
    net = apr_palloc(c->pool, sizeof(*net));
    /* The Nagle algorithm says that we should delay sending partial
     * packets in hopes of getting more data.  We don't want to do
     * this; we are not telnet.  There are bad interactions between
     * persistent connections and Nagle's algorithm that have very severe
     * performance penalties.  (Failing to disable Nagle is not much of a
     * problem with simple HTTP.)
     */
    rv = apr_socket_opt_set(csd, APR_TCP_NODELAY, 1);
    if (rv != APR_SUCCESS && rv != APR_ENOTIMPL) {
        /* expected cause is that the client disconnected already,
         * hence the debug level
         */
        ap_log_cerror(APLOG_MARK, APLOG_DEBUG, rv, c, APLOGNO(00139)
                      "apr_socket_opt_set(APR_TCP_NODELAY)");
    }

    /* The core filter requires the timeout mode to be set, which
     * incidentally sets the socket to be nonblocking.  If this
     * is not initialized correctly, Linux - for example - will
     * be initially blocking, while Solaris will be non blocking
     * and any initial read will fail.
     */
    rv = apr_socket_timeout_set(csd, c->base_server->timeout);
    if (rv != APR_SUCCESS) {
        /* expected cause is that the client disconnected already */
        ap_log_cerror(APLOG_MARK, APLOG_DEBUG, rv, c, APLOGNO(00140)
                      "apr_socket_timeout_set");
    }

    net->c = c;
    net->in_ctx = NULL;
    net->out_ctx = NULL;
    net->client_socket = csd;

    ap_set_core_module_config(net->c->conn_config, csd);
    /* only the master connection talks to the network */
    if (c->master == NULL) {
        ap_add_input_filter_handle(ap_core_input_filter_handle, net, NULL,
                                   net->c);
        ap_add_output_filter_handle(ap_core_output_filter_handle, net, NULL,
                                    net->c);
    }
    return DONE;
}