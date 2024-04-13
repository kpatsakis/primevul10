apr_status_t h2_stream_recv_DATA(h2_stream *stream, uint8_t flags,
                                    const uint8_t *data, size_t len)
{
    h2_session *session = stream->session;
    apr_status_t status = APR_SUCCESS;
    
    stream->in_data_frames++;
    if (len > 0) {
        if (APLOGctrace3(session->c)) {
            const char *load = apr_pstrndup(stream->pool, (const char *)data, len);
            ap_log_cerror(APLOG_MARK, APLOG_TRACE3, 0, session->c,
                          H2_STRM_MSG(stream, "recv DATA, len=%d: -->%s<--"), 
                          (int)len, load);
        }
        else {
            ap_log_cerror(APLOG_MARK, APLOG_TRACE2, status, session->c,
                          H2_STRM_MSG(stream, "recv DATA, len=%d"), (int)len);
        }
        stream->in_data_octets += len;
        if (!stream->in_buffer) {
            stream->in_buffer = apr_brigade_create(stream->pool, 
                                                   session->c->bucket_alloc);
        }
        apr_brigade_write(stream->in_buffer, NULL, NULL, (const char *)data, len);
        h2_stream_dispatch(stream, H2_SEV_IN_DATA_PENDING);
    }
    return status;
}