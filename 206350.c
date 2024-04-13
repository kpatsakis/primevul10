std::string Http2Stream::diagnostic_name() const {
  return "HttpStream " + std::to_string(id()) + " (" +
      std::to_string(static_cast<int64_t>(get_async_id())) + ") [" +
      session()->diagnostic_name() + "]";
}