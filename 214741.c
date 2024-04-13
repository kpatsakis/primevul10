connection_write_100_continue (request_st * const r, connection * const con)
{
    /* Make best effort to send "HTTP/1.1 100 Continue" */
    static const char http_100_continue[] = "HTTP/1.1 100 Continue\r\n\r\n";

    if (con->traffic_limit_reached)
        return 1; /* success; skip sending if throttled */

    chunkqueue * const cq = con->write_queue; /*(bypass r->write_queue)*/
    chunkqueue_append_mem(cq, http_100_continue, sizeof(http_100_continue)-1);
    return connection_write_1xx_info(r, con);
}