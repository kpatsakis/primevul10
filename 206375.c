void Http2Session::HandleHeadersFrame(const nghttp2_frame* frame) {
  Isolate* isolate = env()->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);

  int32_t id = GetFrameID(frame);
  Debug(this, "handle headers frame for stream %d", id);
  Http2Stream* stream = FindStream(id);

  // If the stream has already been destroyed, ignore.
  if (stream->IsDestroyed())
    return;

  std::vector<nghttp2_header> headers(stream->move_headers());
  DecrementCurrentSessionMemory(stream->current_headers_length_);
  stream->current_headers_length_ = 0;

  Local<String> name_str;
  Local<String> value_str;

  Local<Array> holder = Array::New(isolate);
  Local<Function> fn = env()->push_values_to_array_function();
  Local<Value> argv[NODE_PUSH_VAL_TO_ARRAY_MAX * 2];

  // The headers are passed in above as a queue of nghttp2_header structs.
  // The following converts that into a JS array with the structure:
  // [name1, value1, name2, value2, name3, value3, name3, value4] and so on.
  // That array is passed up to the JS layer and converted into an Object form
  // like {name1: value1, name2: value2, name3: [value3, value4]}. We do it
  // this way for performance reasons (it's faster to generate and pass an
  // array than it is to generate and pass the object).
  size_t n = 0;
  while (n < headers.size()) {
    size_t j = 0;
    while (n < headers.size() && j < arraysize(argv) / 2) {
      nghttp2_header item = headers[n++];
      // The header name and value are passed as external one-byte strings
      name_str =
          ExternalHeader::New<true>(this, item.name).ToLocalChecked();
      value_str =
          ExternalHeader::New<false>(this, item.value).ToLocalChecked();
      argv[j * 2] = name_str;
      argv[j * 2 + 1] = value_str;
      j++;
    }
    // For performance, we pass name and value pairs to array.protototype.push
    // in batches of size NODE_PUSH_VAL_TO_ARRAY_MAX * 2 until there are no
    // more items to push.
    if (j > 0) {
      fn->Call(env()->context(), holder, j * 2, argv).ToLocalChecked();
    }
  }

  Local<Value> args[5] = {
    stream->object(),
    Integer::New(isolate, id),
    Integer::New(isolate, stream->headers_category()),
    Integer::New(isolate, frame->hd.flags),
    holder
  };
  MakeCallback(env()->onheaders_string(), arraysize(args), args);
}