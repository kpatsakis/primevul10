Variant HHVM_FUNCTION(socket_accept,
                      const Resource& socket) {
  auto sock = cast<Socket>(socket);
  struct sockaddr sa;
  socklen_t salen = sizeof(sa);
  auto new_sock = req::make<ConcreteSocket>(
    accept(sock->fd(), &sa, &salen), sock->getType());
  if (!new_sock->valid()) {
    SOCKET_ERROR(new_sock, "unable to accept incoming connection", errno);
    return false;
  }
  return Variant(std::move(new_sock));
}