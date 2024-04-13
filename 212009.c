bool HHVM_FUNCTION(socket_bind,
                   const Resource& socket,
                   const String& address,
                   int port /* = 0 */) {
  auto sock = cast<Socket>(socket);

  sockaddr_storage sa_storage;
  struct sockaddr *sa_ptr;
  size_t sa_size;
  if (!set_sockaddr(sa_storage, sock, address, port, sa_ptr, sa_size)) {
    return false;
  }

  long retval = ::bind(sock->fd(), sa_ptr, sa_size);
  if (retval != 0) {
    std::string msg = "unable to bind address";
    msg += address.data();
    msg += ":";
    msg += folly::to<std::string>(port);
    SOCKET_ERROR(sock, msg.c_str(), errno);
    return false;
  }

  return true;
}