const char *h2_stream_state_str(h2_stream *stream) 
{
    return h2_ss_str(stream->state);
}