static apr_status_t transit(h2_stream *stream, int new_state)
{
    if (new_state == stream->state) {
        return APR_SUCCESS;
    }
    else if (new_state < 0) {
        ap_log_cerror(APLOG_MARK, APLOG_WARNING, 0, stream->session->c, 
                      H2_STRM_LOG(APLOGNO(03081), stream, "invalid transition"));
        on_state_invalid(stream);
        return APR_EINVAL;
    }
    
    ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, stream->session->c, 
                  H2_STRM_MSG(stream, "transit to [%s]"), h2_ss_str(new_state));
    stream->state = new_state;
    switch (new_state) {
        case H2_SS_IDLE:
            break;
        case H2_SS_RSVD_L:
            close_input(stream);
            break;
        case H2_SS_RSVD_R:
            break;
        case H2_SS_OPEN:
            break;
        case H2_SS_CLOSED_L:
            close_output(stream);
            break;
        case H2_SS_CLOSED_R:
            close_input(stream);
            break;
        case H2_SS_CLOSED:
            close_input(stream);
            close_output(stream);
            if (stream->out_buffer) {
                apr_brigade_cleanup(stream->out_buffer);
            }
            break;
        case H2_SS_CLEANUP:
            break;
    }
    on_state_enter(stream);
    return APR_SUCCESS;
}