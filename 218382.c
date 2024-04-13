apr_status_t h2_stream_end_headers(h2_stream *stream, int eos, size_t raw_bytes)
{
    apr_status_t status;
    val_len_check_ctx ctx;
    
    status = h2_request_end_headers(stream->rtmp, stream->pool, eos, raw_bytes);
    if (APR_SUCCESS == status) {
        set_policy_for(stream, stream->rtmp);
        stream->request = stream->rtmp;
        stream->rtmp = NULL;
        
        ctx.maxlen = stream->session->s->limit_req_fieldsize;
        ctx.failed_key = NULL;
        apr_table_do(table_check_val_len, &ctx, stream->request->headers, NULL);
        if (ctx.failed_key) {
            if (!h2_stream_is_ready(stream)) {
                ap_log_cerror(APLOG_MARK, APLOG_INFO, 0, stream->session->c,
                              H2_STRM_LOG(APLOGNO(10230), stream,"Request header exceeds "
                                          "LimitRequestFieldSize: %.*s"),
                              (int)H2MIN(strlen(ctx.failed_key), 80), ctx.failed_key);
            }
            set_error_response(stream, HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE);
            /* keep on returning APR_SUCCESS, so that we send a HTTP response and
             * do not RST the stream. */
        }
    }
    return status;
}