AP_DECLARE(apr_socket_t *) ap_get_conn_socket(conn_rec *c)
{
    return ap_get_core_module_config(c->conn_config);
}