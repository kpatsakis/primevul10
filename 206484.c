std::string Http2Session::diagnostic_name() const {
  return std::string("Http2Session ") + TypeName() + " (" +
      std::to_string(static_cast<int64_t>(get_async_id())) + ")";
}