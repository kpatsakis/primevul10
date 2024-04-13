inline Http2Stream* GetStream(Http2Session* session,
                              int32_t id,
                              nghttp2_data_source* source) {
  Http2Stream* stream = static_cast<Http2Stream*>(source->ptr);
  if (stream == nullptr)
    stream = session->FindStream(id);
  CHECK_NOT_NULL(stream);
  CHECK_EQ(id, stream->id());
  return stream;
}