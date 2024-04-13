bool HHVM_FUNCTION(socket_shutdown,
                   const Resource& socket,
                   int how /* = 0 */) {
  /* For some operations that are conceptually a socket operation
   * (eg fopen('http://...)) we actually complete it and store the result in
   * a memfile. As the fact that it's not really a socket is an implementation
   * detail, user code needs to be able to call shutdown on it.
   */
  if (socket->instanceof<MemFile>()) {
    return true;
  }
  auto sock = cast<Socket>(socket);
  if (shutdown(sock->fd(), how) != 0) {
    SOCKET_ERROR(sock, "unable to shutdown socket", errno);
    return false;
  }
  return true;
}