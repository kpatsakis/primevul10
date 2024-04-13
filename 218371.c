static void on_state_enter(h2_stream *stream) 
{
    if (stream->monitor && stream->monitor->on_state_enter) {
        stream->monitor->on_state_enter(stream->monitor->ctx, stream);
    }
}