void TLSWrap::ClearIn() {
  Debug(this, "Trying to write cleartext input");
  // Ignore cycling data if ClientHello wasn't yet parsed
  if (!hello_parser_.IsEnded()) {
    Debug(this, "Returning from ClearIn(), hello_parser_ active");
    return;
  }

  if (ssl_ == nullptr) {
    Debug(this, "Returning from ClearIn(), ssl_ == nullptr");
    return;
  }

  if (pending_cleartext_input_.size() == 0) {
    Debug(this, "Returning from ClearIn(), no pending data");
    return;
  }

  std::vector<char> data = std::move(pending_cleartext_input_);
  crypto::MarkPopErrorOnReturn mark_pop_error_on_return;

  int written = SSL_write(ssl_.get(), data.data(), data.size());
  Debug(this, "Writing %zu bytes, written = %d", data.size(), written);
  CHECK(written == -1 || written == static_cast<int>(data.size()));

  // All written
  if (written != -1) {
    Debug(this, "Successfully wrote all data to SSL");
    return;
  }

  // Error or partial write
  HandleScope handle_scope(env()->isolate());
  Context::Scope context_scope(env()->context());

  int err;
  std::string error_str;
  Local<Value> arg = GetSSLError(written, &err, &error_str);
  if (!arg.IsEmpty()) {
    Debug(this, "Got SSL error (%d)", err);
    write_callback_scheduled_ = true;
    // XXX(sam) Should forward an error object with .code/.function/.etc, if
    // possible.
    InvokeQueued(UV_EPROTO, error_str.c_str());
  } else {
    Debug(this, "Pushing data back");
    // Push back the not-yet-written data. This can be skipped in the error
    // case because no further writes would succeed anyway.
    pending_cleartext_input_ = std::move(data);
  }

  return;
}