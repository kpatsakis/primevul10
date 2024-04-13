static apr_status_t core_insert_network_bucket(conn_rec *c,
                                               apr_bucket_brigade *bb,
                                               apr_socket_t *socket)
{
    apr_bucket *e = apr_bucket_socket_create(socket, c->bucket_alloc);
    APR_BRIGADE_INSERT_TAIL(bb, e);
    return APR_SUCCESS;
}