void h2_stream_rst(h2_stream *stream, int error_code)
{
    stream->rst_error = error_code;
    if (stream->input) {
        h2_beam_abort(stream->input);
    }
    if (stream->output) {
        h2_beam_leave(stream->output);
    }
    ap_log_cerror(APLOG_MARK, APLOG_TRACE1, 0, stream->session->c,
                  H2_STRM_MSG(stream, "reset, error=%d"), error_code);
    h2_stream_dispatch(stream, H2_SEV_CANCELLED);
}