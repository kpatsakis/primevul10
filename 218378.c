static int on_event(h2_stream* stream, h2_stream_event_t ev)
{
    if (stream->monitor && stream->monitor->on_event) {
        stream->monitor->on_event(stream->monitor->ctx, stream, ev);
    }
    if (ev < H2_ALEN(trans_on_event)) {
        return on_map(stream->state, trans_on_event[ev]);
    }
    return stream->state;
}