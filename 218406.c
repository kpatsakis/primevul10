void h2_stream_destroy(h2_stream *stream)
{
    ap_assert(stream);
    ap_log_cerror(APLOG_MARK, APLOG_TRACE3, 0, stream->session->c, 
                  H2_STRM_MSG(stream, "destroy"));
    apr_pool_destroy(stream->pool);
}