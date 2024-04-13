static void on_state_invalid(h2_stream *stream) 
{
    if (stream->monitor && stream->monitor->on_state_invalid) {
        stream->monitor->on_state_invalid(stream->monitor->ctx, stream);
    }
    /* stream got an event/frame invalid in its state */
    ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, stream->session->c,
                  H2_STRM_MSG(stream, "invalid state event")); 
    switch (stream->state) {
        case H2_SS_OPEN:
        case H2_SS_RSVD_L:
        case H2_SS_RSVD_R:
        case H2_SS_CLOSED_L:
        case H2_SS_CLOSED_R:
            h2_stream_rst(stream, H2_ERR_INTERNAL_ERROR);
            break;
        default:
            break;
    }
}