h2_stream *h2_stream_create(int id, apr_pool_t *pool, h2_session *session,
                            h2_stream_monitor *monitor, int initiated_on)
{
    h2_stream *stream = apr_pcalloc(pool, sizeof(h2_stream));
    
    stream->id           = id;
    stream->initiated_on = initiated_on;
    stream->created      = apr_time_now();
    stream->state        = H2_SS_IDLE;
    stream->pool         = pool;
    stream->session      = session;
    stream->monitor      = monitor;
    stream->max_mem      = session->max_stream_mem;
    
#ifdef H2_NG2_LOCAL_WIN_SIZE
    stream->in_window_size = 
        nghttp2_session_get_stream_local_window_size(
            stream->session->ngh2, stream->id);
#endif

    ap_log_cerror(APLOG_MARK, APLOG_DEBUG, 0, session->c, 
                  H2_STRM_LOG(APLOGNO(03082), stream, "created"));
    on_state_enter(stream);
    return stream;
}