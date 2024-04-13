int StreamBase::Writev(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  CHECK(args[0]->IsObject());
  CHECK(args[1]->IsArray());

  Local<Object> req_wrap_obj = args[0].As<Object>();
  Local<Array> chunks = args[1].As<Array>();
  bool all_buffers = args[2]->IsTrue();

  size_t count;
  if (all_buffers)
    count = chunks->Length();
  else
    count = chunks->Length() >> 1;

  MaybeStackBuffer<uv_buf_t, 16> bufs(count);

  size_t storage_size = 0;
  size_t offset;

  if (!all_buffers) {
    // Determine storage size first
    for (size_t i = 0; i < count; i++) {
      Local<Value> chunk = chunks->Get(env->context(), i * 2).ToLocalChecked();

      if (Buffer::HasInstance(chunk))
        continue;
        // Buffer chunk, no additional storage required

      // String chunk
      Local<String> string = chunk->ToString(env->context()).ToLocalChecked();
      enum encoding encoding = ParseEncoding(env->isolate(),
          chunks->Get(env->context(), i * 2 + 1).ToLocalChecked());
      size_t chunk_size;
      if (encoding == UTF8 && string->Length() > 65535 &&
          !StringBytes::Size(env->isolate(), string, encoding).To(&chunk_size))
        return 0;
      else if (!StringBytes::StorageSize(env->isolate(), string, encoding)
                    .To(&chunk_size))
        return 0;
      storage_size += chunk_size;
    }

    if (storage_size > INT_MAX)
      return UV_ENOBUFS;
  } else {
    for (size_t i = 0; i < count; i++) {
      Local<Value> chunk = chunks->Get(env->context(), i).ToLocalChecked();
      bufs[i].base = Buffer::Data(chunk);
      bufs[i].len = Buffer::Length(chunk);
    }
  }

  AllocatedBuffer storage;
  if (storage_size > 0)
    storage = AllocatedBuffer::AllocateManaged(env, storage_size);

  offset = 0;
  if (!all_buffers) {
    for (size_t i = 0; i < count; i++) {
      Local<Value> chunk = chunks->Get(env->context(), i * 2).ToLocalChecked();

      // Write buffer
      if (Buffer::HasInstance(chunk)) {
        bufs[i].base = Buffer::Data(chunk);
        bufs[i].len = Buffer::Length(chunk);
        continue;
      }

      // Write string
      CHECK_LE(offset, storage_size);
      char* str_storage = storage.data() + offset;
      size_t str_size = storage.size() - offset;

      Local<String> string = chunk->ToString(env->context()).ToLocalChecked();
      enum encoding encoding = ParseEncoding(env->isolate(),
          chunks->Get(env->context(), i * 2 + 1).ToLocalChecked());
      str_size = StringBytes::Write(env->isolate(),
                                    str_storage,
                                    str_size,
                                    string,
                                    encoding);
      bufs[i].base = str_storage;
      bufs[i].len = str_size;
      offset += str_size;
    }
  }

  StreamWriteResult res = Write(*bufs, count, nullptr, req_wrap_obj);
  SetWriteResult(res);
  if (res.wrap != nullptr && storage_size > 0) {
    res.wrap->SetAllocatedStorage(std::move(storage));
  }
  return res.err;
}