apr_status_t h2_stream_prep_processing(h2_stream *stream)
{
    if (stream->request) {
        const h2_request *r = stream->request;
        ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, stream->session->c,
                      H2_STRM_MSG(stream, "schedule %s %s://%s%s chunked=%d"),
                      r->method, r->scheme, r->authority, r->path, r->chunked);
        setup_input(stream);
        stream->scheduled = 1;
        return APR_SUCCESS;
    }
    return APR_EINVAL;
}