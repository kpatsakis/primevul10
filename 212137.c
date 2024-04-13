Variant HHVM_FUNCTION(socket_recvfrom,
                      const Resource& socket,
                      Variant& buf,
                      int64_t len,
                      int flags,
                      Variant& name,
                      Variant& port) {
  if (len <= 0) {
    return false;
  }

  auto sock = cast<Socket>(socket);

  char *recv_buf = (char *)malloc(len + 2);
  socklen_t slen;
  int retval;

  switch (sock->getType()) {
  case AF_UNIX:
    {
#ifdef _MSC_VER
      return false;
#else
      struct sockaddr_un s_un;
      slen = sizeof(s_un);
      memset(&s_un, 0, slen);
      s_un.sun_family = AF_UNIX;
      retval = recvfrom(sock->fd(), recv_buf, (size_t)len, flags,
                        (struct sockaddr *)&s_un, (socklen_t *)&slen);
      if (retval < 0) {
        free(recv_buf);
        SOCKET_ERROR(sock, "unable to recvfrom", errno);
        return false;
      }

      recv_buf[retval] = 0;
      buf = String(recv_buf, retval, AttachString);
      // - `sun_path` MAY have trailing nulls
      // - `sun_len` MAY include that trailing null on Linux.
      const auto max_path_len = slen - offsetof(struct sockaddr_un, sun_path);
      const auto actual_path_len = ::strnlen(s_un.sun_path, max_path_len);
      name = String(s_un.sun_path, actual_path_len, CopyString);
#endif
    }
    break;
  case AF_INET:
    {
      struct sockaddr_in sin;
      slen = sizeof(sin);
      memset(&sin, 0, slen);
      sin.sin_family = AF_INET; // recvfrom doesn't fill this in

      retval = recvfrom(sock->fd(), recv_buf, (size_t)len, flags,
                        (struct sockaddr *)&sin, (socklen_t *)&slen);
      if (retval < 0) {
        free(recv_buf);
        SOCKET_ERROR(sock, "unable to recvfrom", errno);
        return false;
      }
      recv_buf[retval] = 0;
      buf = String(recv_buf, retval, AttachString);

      try {
        folly::SocketAddress addr;
        addr.setFromSockaddr(&sin);

        name = String(addr.getAddressStr(), CopyString);
        port = addr.getPort();
      } catch (...) {
        name = s_0_0_0_0;
        port = 0;
      }
    }
    break;
  case AF_INET6:
    {
      struct sockaddr_in6 sin6;
      slen = sizeof(sin6);
      memset(&sin6, 0, slen);
      sin6.sin6_family = AF_INET6; // recvfrom doesn't fill this in

      retval = recvfrom(sock->fd(), recv_buf, (size_t)len, flags,
                        (struct sockaddr *)&sin6, (socklen_t *)&slen);
      if (retval < 0) {
        free(recv_buf);
        SOCKET_ERROR(sock, "unable to recvfrom", errno);
        return false;
      }

      recv_buf[retval] = 0;
      buf = String(recv_buf, retval, AttachString);

      try {
        folly::SocketAddress addr;
        addr.setFromSockaddr(&sin6);

        name = String(addr.getAddressStr(), CopyString);
        port = addr.getPort();
      } catch (...) {
        name = s_2colons;
        port = 0;
      }
    }
    break;
  default:
    raise_warning("Unsupported socket type %d", sock->getType());
    return false;
  }

  return retval;
}