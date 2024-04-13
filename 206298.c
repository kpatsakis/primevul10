void Http2Session::PushOutgoingBuffer(nghttp2_stream_write&& write) {
  outgoing_length_ += write.buf.len;
  outgoing_buffers_.emplace_back(std::move(write));
}