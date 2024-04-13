int h2_stream_is_ready(h2_stream *stream)
{
    if (stream->has_response) {
        return 1;
    }
    else if (stream->out_buffer && get_first_headers_bucket(stream->out_buffer)) {
        return 1;
    }
    return 0;
}