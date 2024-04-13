bool Http2Session::HasWritesOnSocketForStream(Http2Stream* stream) {
  for (const nghttp2_stream_write& wr : outgoing_buffers_) {
    if (wr.req_wrap != nullptr && wr.req_wrap->stream() == stream)
      return true;
  }
  return false;
}