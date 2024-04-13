void h2_stream_dispatch(h2_stream *stream, h2_stream_event_t ev)
{
    int new_state;
    
    ap_log_cerror(APLOG_MARK, APLOG_TRACE2, 0, stream->session->c,
                  H2_STRM_MSG(stream, "dispatch event %d"), ev);
    new_state = on_event(stream, ev);
    if (new_state < 0) {
        ap_log_cerror(APLOG_MARK, APLOG_WARNING, 0, stream->session->c, 
                      H2_STRM_LOG(APLOGNO(10002), stream, "invalid event %d"), ev);
        on_state_invalid(stream);
        AP_DEBUG_ASSERT(new_state > S_XXX);
        return;
    }
    else if (new_state == stream->state) {
        /* nop */
        ap_log_cerror(APLOG_MARK, APLOG_TRACE2, 0, stream->session->c,
                      H2_STRM_MSG(stream, "non-state event %d"), ev);
        return;
    }
    else {
        on_state_event(stream, ev);
        transit(stream, new_state);
    }
}