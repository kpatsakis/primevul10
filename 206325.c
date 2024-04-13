int Http2Session::HandleDataFrame(const nghttp2_frame* frame) {
  int32_t id = GetFrameID(frame);
  Debug(this, "handling data frame for stream %d", id);
  Http2Stream* stream = FindStream(id);

  if (!stream->IsDestroyed() && frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
    stream->EmitRead(UV_EOF);
  } else if (frame->hd.length == 0 &&
           !IsReverted(SECURITY_REVERT_CVE_2019_9518)) {
    return 1;  // Consider 0-length frame without END_STREAM an error.
  }
  return 0;
}