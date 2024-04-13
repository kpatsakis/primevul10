AP_CORE_DECLARE(conn_rec *) ap_create_slave_connection(conn_rec *c)
{
    apr_pool_t *pool;
    conn_slave_rec *new;
    conn_rec *sc = (conn_rec *) apr_palloc(c->pool, sizeof(conn_rec));

    apr_pool_create(&pool, c->pool);
    apr_pool_tag(pool, "slave_conn");
    memcpy(sc, c, sizeof(conn_rec));
    sc->slaves = NULL;
    sc->master = c;
    sc->input_filters = NULL;
    sc->output_filters = NULL;
    sc->pool = pool;
    new = apr_array_push(c->slaves);
    new->c = sc;
    return sc;
}