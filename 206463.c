void nghttp2_stream_write::MemoryInfo(MemoryTracker* tracker) const {
  if (req_wrap != nullptr)
    tracker->TrackField("req_wrap", req_wrap->GetAsyncWrap());
  tracker->TrackField("buf", buf);
}