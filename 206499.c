bool Http2Session::AddSettings(Http2Session::Http2Settings* settings) {
  if (outstanding_settings_.size() == max_outstanding_settings_)
    return false;
  outstanding_settings_.push(settings);
  IncrementCurrentSessionMemory(sizeof(*settings));
  return true;
}