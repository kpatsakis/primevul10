const char* Http2Session::TypeName() const {
  switch (session_type_) {
    case NGHTTP2_SESSION_SERVER: return "server";
    case NGHTTP2_SESSION_CLIENT: return "client";
    default:
      // This should never happen
      ABORT();
  }
}