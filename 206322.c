const char* TLSWrap::Error() const {
  return error_.empty() ? nullptr : error_.c_str();
}