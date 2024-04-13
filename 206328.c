void nghttp2_header::MemoryInfo(MemoryTracker* tracker) const {
  tracker->TrackFieldWithSize("name", nghttp2_rcbuf_get_buf(name).len);
  tracker->TrackFieldWithSize("value", nghttp2_rcbuf_get_buf(value).len);
}