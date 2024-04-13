static void on_state_event(h2_stream *stream, h2_stream_event_t ev) 
{
    if (stream->monitor && stream->monitor->on_state_event) {
        stream->monitor->on_state_event(stream->monitor->ctx, stream, ev);
    }
}