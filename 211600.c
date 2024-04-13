bool HHVM_FUNCTION(socket_listen,
                   const Resource& socket,
                   int backlog /* = 0 */) {
  auto sock = cast<Socket>(socket);
  if (listen(sock->fd(), backlog) != 0) {
    SOCKET_ERROR(sock, "unable to listen on socket", errno);
    return false;
  }
  return true;
}