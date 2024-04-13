connection_check_expect_100 (request_st * const r, connection * const con)
{
    if (con->is_writable <= 0)
        return 1;

    const buffer * const vb =
      http_header_request_get(r, HTTP_HEADER_EXPECT,
                              CONST_STR_LEN("Expect"));
    if (NULL == vb)
        return 1;

    /* (always unset Expect header so that check is not repeated for request */
    int rc = buffer_eq_icase_slen(vb, CONST_STR_LEN("100-continue"));
    http_header_request_unset(r, HTTP_HEADER_EXPECT,
                              CONST_STR_LEN("Expect"));
    if (!rc
        || 0 != r->reqbody_queue.bytes_in
        || !chunkqueue_is_empty(&r->read_queue)
        || !chunkqueue_is_empty(&r->write_queue))
        return 1;

    /* send 100 Continue only if no request body data received yet
     * and response has not yet started (checked above) */
    if (r->http_version > HTTP_VERSION_1_1)
        h2_send_100_continue(r, con);
    else if (r->http_version == HTTP_VERSION_1_1)
        return connection_write_100_continue(r, con);

    return 1;
}