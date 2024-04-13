AP_DECLARE(apr_port_t) ap_get_server_port(const request_rec *r)
{
    apr_port_t port;
    core_dir_config *d =
      (core_dir_config *)ap_get_core_module_config(r->per_dir_config);

    switch (d->use_canonical_name) {
        case USE_CANONICAL_NAME_OFF:
        case USE_CANONICAL_NAME_DNS:
        case USE_CANONICAL_NAME_UNSET:
            if (d->use_canonical_phys_port == USE_CANONICAL_PHYS_PORT_ON)
                port = r->parsed_uri.port_str ? r->parsed_uri.port :
                       r->connection->local_addr->port ? r->connection->local_addr->port :
                       r->server->port ? r->server->port :
                       ap_default_port(r);
            else /* USE_CANONICAL_PHYS_PORT_OFF or USE_CANONICAL_PHYS_PORT_UNSET */
                port = r->parsed_uri.port_str ? r->parsed_uri.port :
                       r->server->port ? r->server->port :
                       ap_default_port(r);
            break;
        case USE_CANONICAL_NAME_ON:
            /* With UseCanonicalName on (and in all versions prior to 1.3)
             * Apache will use the hostname and port specified in the
             * ServerName directive to construct a canonical name for the
             * server. (If no port was specified in the ServerName
             * directive, Apache uses the port supplied by the client if
             * any is supplied, and finally the default port for the protocol
             * used.
             */
            if (d->use_canonical_phys_port == USE_CANONICAL_PHYS_PORT_ON)
                port = r->server->port ? r->server->port :
                       r->connection->local_addr->port ? r->connection->local_addr->port :
                       ap_default_port(r);
            else /* USE_CANONICAL_PHYS_PORT_OFF or USE_CANONICAL_PHYS_PORT_UNSET */
                port = r->server->port ? r->server->port :
                       ap_default_port(r);
            break;
        default:
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, APLOGNO(00110)
                         "ap_get_server_port: Invalid UCN Option somehow");
            port = ap_default_port(r);
            break;
    }

    return port;
}