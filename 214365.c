static int core_upgrade_handler(request_rec *r)
{
    conn_rec *c = r->connection;
    const char *upgrade;

    if (c->master) {
        /* Not possible to perform an HTTP/1.1 upgrade from a slave
         * connection. */
        return DECLINED;
    }
    
    upgrade = apr_table_get(r->headers_in, "Upgrade");
    if (upgrade && *upgrade) {
        const char *conn = apr_table_get(r->headers_in, "Connection");
        if (ap_find_token(r->pool, conn, "upgrade")) {
            apr_array_header_t *offers = NULL;
            const char *err;
            
            err = ap_parse_token_list_strict(r->pool, upgrade, &offers, 0);
            if (err) {
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(02910)
                              "parsing Upgrade header: %s", err);
                return DECLINED;
            }
            
            if (offers && offers->nelts > 0) {
                const char *protocol = ap_select_protocol(c, r, NULL, offers);
                if (protocol && strcmp(protocol, ap_get_protocol(c))) {
                    ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(02909)
                                  "Upgrade selects '%s'", protocol);
                    /* Let the client know what we are upgrading to. */
                    apr_table_clear(r->headers_out);
                    apr_table_setn(r->headers_out, "Upgrade", protocol);
                    apr_table_setn(r->headers_out, "Connection", "Upgrade");
                    
                    r->status = HTTP_SWITCHING_PROTOCOLS;
                    r->status_line = ap_get_status_line(r->status);
                    ap_send_interim_response(r, 1);

                    ap_switch_protocol(c, r, r->server, protocol);

                    /* make sure httpd closes the connection after this */
                    c->keepalive = AP_CONN_CLOSE;
                    return DONE;
                }
            }
        }
    }
    else if (!c->keepalives) {
        /* first request on a master connection, if we have protocols other
         * than the current one enabled here, announce them to the
         * client. If the client is already talking a protocol with requests
         * on slave connections, leave it be. */
        const apr_array_header_t *upgrades;
        ap_get_protocol_upgrades(c, r, NULL, 0, &upgrades);
        if (upgrades && upgrades->nelts > 0) {
            char *protocols = apr_array_pstrcat(r->pool, upgrades, ',');
            apr_table_setn(r->headers_out, "Upgrade", protocols);
            apr_table_setn(r->headers_out, "Connection", "Upgrade");
        }
    }
    
    return DECLINED;
}