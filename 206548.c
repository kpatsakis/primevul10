void Http2Stream::EmitStatistics() {
  if (!HasHttp2Observer(env()))
    return;
  Http2StreamPerformanceEntry* entry =
    new Http2StreamPerformanceEntry(env(), id_, statistics_);
  env()->SetImmediate([](Environment* env, void* data) {
    // This takes ownership, the entry is destroyed at the end of this scope.
    std::unique_ptr<Http2StreamPerformanceEntry> entry {
        static_cast<Http2StreamPerformanceEntry*>(data) };
    if (!HasHttp2Observer(env))
      return;
    HandleScope handle_scope(env->isolate());
    AliasedBuffer<double, Float64Array>& buffer =
        env->http2_state()->stream_stats_buffer;
    buffer[IDX_STREAM_STATS_ID] = entry->id();
    if (entry->first_byte() != 0) {
      buffer[IDX_STREAM_STATS_TIMETOFIRSTBYTE] =
          (entry->first_byte() - entry->startTimeNano()) / 1e6;
    } else {
      buffer[IDX_STREAM_STATS_TIMETOFIRSTBYTE] = 0;
    }
    if (entry->first_header() != 0) {
      buffer[IDX_STREAM_STATS_TIMETOFIRSTHEADER] =
          (entry->first_header() - entry->startTimeNano()) / 1e6;
    } else {
      buffer[IDX_STREAM_STATS_TIMETOFIRSTHEADER] = 0;
    }
    if (entry->first_byte_sent() != 0) {
      buffer[IDX_STREAM_STATS_TIMETOFIRSTBYTESENT] =
          (entry->first_byte_sent() - entry->startTimeNano()) / 1e6;
    } else {
      buffer[IDX_STREAM_STATS_TIMETOFIRSTBYTESENT] = 0;
    }
    buffer[IDX_STREAM_STATS_SENTBYTES] = entry->sent_bytes();
    buffer[IDX_STREAM_STATS_RECEIVEDBYTES] = entry->received_bytes();
    entry->Notify(entry->ToObject());
  }, static_cast<void*>(entry));
}