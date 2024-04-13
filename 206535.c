int Http2Session::OnFrameSent(nghttp2_session* handle,
                              const nghttp2_frame* frame,
                              void* user_data) {
  Http2Session* session = static_cast<Http2Session*>(user_data);
  session->statistics_.frame_sent += 1;
  return 0;
}