void h2_stream_set_monitor(h2_stream *stream, h2_stream_monitor *monitor)
{
    stream->monitor = monitor;
}