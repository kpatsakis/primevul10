static apr_status_t close_input(h2_stream *stream)
{
    conn_rec *c = stream->session->c;
    apr_status_t status = APR_SUCCESS;

    stream->input_eof = 1;
    if (stream->input && h2_beam_is_closed(stream->input)) {
        return APR_SUCCESS;
    }
    
    ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, stream->session->c,
                  H2_STRM_MSG(stream, "closing input"));
    if (stream->rst_error) {
        return APR_ECONNRESET;
    }
    
    if (stream->trailers && !apr_is_empty_table(stream->trailers)) {
        apr_bucket *b;
        h2_headers *r;
        
        if (!stream->in_buffer) {
            stream->in_buffer = apr_brigade_create(stream->pool, c->bucket_alloc);
        }
        
        r = h2_headers_create(HTTP_OK, stream->trailers, NULL, 
            stream->in_trailer_octets, stream->pool);
        stream->trailers = NULL;        
        b = h2_bucket_headers_create(c->bucket_alloc, r);
        APR_BRIGADE_INSERT_TAIL(stream->in_buffer, b);
        
        b = apr_bucket_eos_create(c->bucket_alloc);
        APR_BRIGADE_INSERT_TAIL(stream->in_buffer, b);
        
        ap_log_cerror(APLOG_MARK, APLOG_TRACE2, 0, stream->session->c,
                      H2_STRM_MSG(stream, "added trailers"));
        h2_stream_dispatch(stream, H2_SEV_IN_DATA_PENDING);
    }
    if (stream->input) {
        h2_stream_flush_input(stream);
        return h2_beam_close(stream->input);
    }
    return status;
}