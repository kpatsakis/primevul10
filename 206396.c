void Http2Session::StopTrackingRcbuf(nghttp2_rcbuf* buf) {
  MemoryAllocatorInfo::StopTracking(this, buf);
}