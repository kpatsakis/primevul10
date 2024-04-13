int Http2Session::OnBeginHeadersCallback(nghttp2_session* handle,
                                         const nghttp2_frame* frame,
                                         void* user_data) {
  Http2Session* session = static_cast<Http2Session*>(user_data);
  int32_t id = GetFrameID(frame);
  Debug(session, "beginning headers for stream %d", id);

  Http2Stream* stream = session->FindStream(id);
  // The common case is that we're creating a new stream. The less likely
  // case is that we're receiving a set of trailers
  if (LIKELY(stream == nullptr)) {
    if (UNLIKELY(!session->CanAddStream() ||
                 Http2Stream::New(session, id, frame->headers.cat) ==
                     nullptr)) {
      if (session->rejected_stream_count_++ > 100 &&
          !IsReverted(SECURITY_REVERT_CVE_2019_9514)) {
        return NGHTTP2_ERR_CALLBACK_FAILURE;
      }
      // Too many concurrent streams being opened
      nghttp2_submit_rst_stream(**session, NGHTTP2_FLAG_NONE, id,
                                NGHTTP2_ENHANCE_YOUR_CALM);
      return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
    }

    session->rejected_stream_count_ = 0;
  } else if (!stream->IsDestroyed()) {
    stream->StartHeaders(frame->headers.cat);
  }
  return 0;
}