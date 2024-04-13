Http2Session::Http2Settings* Http2Session::PopSettings() {
  Http2Settings* settings = nullptr;
  if (!outstanding_settings_.empty()) {
    settings = outstanding_settings_.front();
    outstanding_settings_.pop();
    DecrementCurrentSessionMemory(sizeof(*settings));
  }
  return settings;
}