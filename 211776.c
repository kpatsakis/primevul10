Variant socket_server_impl(
  const HostURL &hosturl,
  int flags,
  Variant& errnum,
  Variant& errstr,
  const Variant& context /* = uninit_variant */
) {
  errnum = 0;
  errstr = empty_string();
  auto sock = create_new_socket(hosturl, errnum, errstr, context);
  if (!sock) {
    return false;
  }

  sockaddr_storage sa_storage;
  struct sockaddr *sa_ptr;
  size_t sa_size;
  if (!set_sockaddr(sa_storage, sock, hosturl.getHost(),
                    hosturl.getPort(), sa_ptr, sa_size)) {
    return false;
  }
  int yes = 1;
  setsockopt(sock->fd(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
  if ((flags & k_STREAM_SERVER_BIND) != 0 &&
      ::bind(sock->fd(), sa_ptr, sa_size) < 0) {
    SOCKET_ERROR(sock, "unable to bind to given address", errno);
    return false;
  }
  if ((flags & k_STREAM_SERVER_LISTEN) != 0 && listen(sock->fd(), 128) < 0) {
    SOCKET_ERROR(sock, "unable to listen on socket", errno);
    return false;
  }

  return Variant(std::move(sock));
}