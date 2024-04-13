static apr_status_t close_output(h2_stream *stream)
{
    if (!stream->output || h2_beam_is_closed(stream->output)) {
        return APR_SUCCESS;
    }
    ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, stream->session->c,
                  H2_STRM_MSG(stream, "closing output"));
    return h2_beam_leave(stream->output);
}