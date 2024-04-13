Variant HHVM_FUNCTION(socket_send,
                      const Resource& socket,
                      const String& buf,
                      int64_t len,
                      int flags) {
  auto sock = cast<Socket>(socket);
  if (len < 0) return false;
  if (len > buf.size()) {
    len = buf.size();
  }
  int retval = send(sock->fd(), buf.data(), (size_t)len, flags);
  if (retval == -1) {
    SOCKET_ERROR(sock, "unable to write to socket", errno);
    return false;
  }
  return retval;
}