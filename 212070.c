bool HHVM_FUNCTION(socket_getpeername,
                   const Resource& socket,
                   Variant& address,
                   Variant& port) {
  auto sock = cast<Socket>(socket);

  sockaddr_storage sa_storage;
  socklen_t salen = sizeof(sockaddr_storage);
  struct sockaddr *sa = (struct sockaddr *)&sa_storage;
  if (getpeername(sock->fd(), sa, &salen) < 0) {
    SOCKET_ERROR(sock, "unable to retrieve peer name", errno);
    return false;
  }
  Variant a, p;
  auto ret = get_sockaddr(sa, salen, a, p);
  address = a;
  port = p;
  return ret;
}