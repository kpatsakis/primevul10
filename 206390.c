void Http2Session::EmitStatistics() {
  if (!HasHttp2Observer(env()))
    return;
  Http2SessionPerformanceEntry* entry =
    new Http2SessionPerformanceEntry(env(), statistics_, session_type_);
  env()->SetImmediate([](Environment* env, void* data) {
    // This takes ownership, the entr is destroyed at the end of this scope.
    std::unique_ptr<Http2SessionPerformanceEntry> entry {
        static_cast<Http2SessionPerformanceEntry*>(data) };
    if (!HasHttp2Observer(env))
      return;
    HandleScope handle_scope(env->isolate());
    AliasedBuffer<double, Float64Array>& buffer =
        env->http2_state()->session_stats_buffer;
    buffer[IDX_SESSION_STATS_TYPE] = entry->type();
    buffer[IDX_SESSION_STATS_PINGRTT] = entry->ping_rtt() / 1e6;
    buffer[IDX_SESSION_STATS_FRAMESRECEIVED] = entry->frame_count();
    buffer[IDX_SESSION_STATS_FRAMESSENT] = entry->frame_sent();
    buffer[IDX_SESSION_STATS_STREAMCOUNT] = entry->stream_count();
    buffer[IDX_SESSION_STATS_STREAMAVERAGEDURATION] =
        entry->stream_average_duration();
    buffer[IDX_SESSION_STATS_DATA_SENT] = entry->data_sent();
    buffer[IDX_SESSION_STATS_DATA_RECEIVED] = entry->data_received();
    buffer[IDX_SESSION_STATS_MAX_CONCURRENT_STREAMS] =
        entry->max_concurrent_streams();
    entry->Notify(entry->ToObject());
  }, static_cast<void*>(entry));
}