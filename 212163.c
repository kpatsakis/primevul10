bool HHVM_FUNCTION(socket_connect,
                   const Resource& socket,
                   const String& address,
                   int port /* = 0 */) {
  auto sock = cast<Socket>(socket);

  switch (sock->getType()) {
  case AF_INET6:
  case AF_INET:
    if (port == 0) {
      raise_warning("Socket of type AF_INET/6 requires 3 arguments");
      return false;
    }
    break;
  default:
    break;
  }

  sockaddr_storage sa_storage;
  struct sockaddr *sa_ptr;
  size_t sa_size;
  if (!set_sockaddr(sa_storage, sock, address, port, sa_ptr, sa_size)) {
    return false;
  }

  IOStatusHelper io("socket::connect", address.data(), port);
  int retval = connect(sock->fd(), sa_ptr, sa_size);
  if (retval != 0) {
    std::string msg = "unable to connect to ";
    msg += address.data();
    msg += ":";
    msg += folly::to<std::string>(port);
    SOCKET_ERROR(sock, msg.c_str(), errno);
    return false;
  }

  return true;
}