std::string TLSWrap::diagnostic_name() const {
  std::string name = "TLSWrap ";
  if (is_server())
    name += "server (";
  else
    name += "client (";
  name += std::to_string(static_cast<int64_t>(get_async_id())) + ")";
  return name;
}