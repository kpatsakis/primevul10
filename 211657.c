Variant HHVM_FUNCTION(socket_write,
                      const Resource& socket,
                      const String& buffer,
                      int64_t length /* = 0 */) {
  auto sock = cast<Socket>(socket);
  if (length < 0) return false;
  if (length == 0 || length > buffer.size()) {
    length = buffer.size();
  }
  int retval = write(sock->fd(), buffer.data(), (size_t)length);
  if (retval < 0) {
    SOCKET_ERROR(sock, "unable to write to socket", errno);
    return false;
  }
  return retval;
}