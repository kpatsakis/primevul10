void Http2Session::ClearOutgoing(int status) {
  CHECK_NE(flags_ & SESSION_STATE_SENDING, 0);

  flags_ &= ~SESSION_STATE_SENDING;

  if (outgoing_buffers_.size() > 0) {
    outgoing_storage_.clear();
    outgoing_length_ = 0;

    std::vector<nghttp2_stream_write> current_outgoing_buffers_;
    current_outgoing_buffers_.swap(outgoing_buffers_);
    for (const nghttp2_stream_write& wr : current_outgoing_buffers_) {
      WriteWrap* wrap = wr.req_wrap;
      if (wrap != nullptr)
        wrap->Done(status);
    }
  }

  // Now that we've finished sending queued data, if there are any pending
  // RstStreams we should try sending again and then flush them one by one.
  if (pending_rst_streams_.size() > 0) {
    std::vector<int32_t> current_pending_rst_streams;
    pending_rst_streams_.swap(current_pending_rst_streams);

    SendPendingData();

    for (int32_t stream_id : current_pending_rst_streams) {
      Http2Stream* stream = FindStream(stream_id);
      if (LIKELY(stream != nullptr))
        stream->FlushRstStream();
    }
  }
}