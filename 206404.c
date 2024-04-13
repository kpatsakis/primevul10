Http2Session::Callbacks::Callbacks(bool kHasGetPaddingCallback) {
  CHECK_EQ(nghttp2_session_callbacks_new(&callbacks), 0);

  nghttp2_session_callbacks_set_on_begin_headers_callback(
    callbacks, OnBeginHeadersCallback);
  nghttp2_session_callbacks_set_on_header_callback2(
    callbacks, OnHeaderCallback);
  nghttp2_session_callbacks_set_on_frame_recv_callback(
    callbacks, OnFrameReceive);
  nghttp2_session_callbacks_set_on_stream_close_callback(
    callbacks, OnStreamClose);
  nghttp2_session_callbacks_set_on_data_chunk_recv_callback(
    callbacks, OnDataChunkReceived);
  nghttp2_session_callbacks_set_on_frame_not_send_callback(
    callbacks, OnFrameNotSent);
  nghttp2_session_callbacks_set_on_invalid_header_callback2(
    callbacks, OnInvalidHeader);
  nghttp2_session_callbacks_set_error_callback(
    callbacks, OnNghttpError);
  nghttp2_session_callbacks_set_send_data_callback(
    callbacks, OnSendData);
  nghttp2_session_callbacks_set_on_invalid_frame_recv_callback(
    callbacks, OnInvalidFrame);
  nghttp2_session_callbacks_set_on_frame_send_callback(
    callbacks, OnFrameSent);

  if (kHasGetPaddingCallback) {
    nghttp2_session_callbacks_set_select_padding_callback(
      callbacks, OnSelectPadding);
  }
}