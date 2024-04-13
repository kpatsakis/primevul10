Variant HHVM_FUNCTION(socket_create_listen,
                      int port,
                      int backlog /* = 128 */) {
  HostEnt result;
  if (!safe_gethostbyname("0.0.0.0", result)) {
    return false;
  }

  struct sockaddr_in la;
  memcpy((char *) &la.sin_addr, result.hostbuf.h_addr,
         result.hostbuf.h_length);
  la.sin_family = result.hostbuf.h_addrtype;
  la.sin_port = htons((unsigned short)port);

  auto sock = req::make<ConcreteSocket>(
    socket(PF_INET, SOCK_STREAM, 0), PF_INET, "0.0.0.0", port);

  if (!sock->valid()) {
    SOCKET_ERROR(sock, "unable to create listening socket", errno);
    return false;
  }

  if (::bind(sock->fd(), (struct sockaddr *)&la, sizeof(la)) < 0) {
    SOCKET_ERROR(sock, "unable to bind to given address", errno);
    return false;
  }

  if (listen(sock->fd(), backlog) < 0) {
    SOCKET_ERROR(sock, "unable to listen on socket", errno);
    return false;
  }

  return Variant(std::move(sock));
}