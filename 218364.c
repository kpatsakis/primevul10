int h2_stream_was_closed(const h2_stream *stream)
{
    switch (stream->state) {
        case H2_SS_CLOSED:
        case H2_SS_CLEANUP:
            return 1;
        default:
            return 0;
    }
}