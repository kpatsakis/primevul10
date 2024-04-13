Variant HHVM_FUNCTION(socket_sendto,
                      const Resource& socket,
                      const String& buf,
                      int64_t len,
                      int flags,
                      const String& addr,
                      int port /* = -1 */) {
  auto sock = cast<Socket>(socket);
  if (len < 0) return false;
  if (len > buf.size()) {
    len = buf.size();
  }
  int retval;
  switch (sock->getType()) {
  case AF_UNIX:
    {
#ifdef _MSC_VER
      retval = -1;
#else
      struct sockaddr_storage s_s;
      struct sockaddr* sa_ptr;
      size_t sa_size;
      if (!set_sockaddr(s_s, sock, addr, 0, sa_ptr, sa_size)) {
        return false;
      }

      retval = sendto(sock->fd(), buf.data(), (size_t)len, flags, sa_ptr,
                      sa_size);
#endif
    }
    break;
  case AF_INET:
    {
      if (port == -1) {
        throw_missing_arguments_nr("socket_sendto", 6, 5);
        return false;
      }

      struct sockaddr_in  sin;
      memset(&sin, 0, sizeof(sin));
      sin.sin_family = AF_INET;
      sin.sin_port = htons((unsigned short) port);
      if (!php_set_inet_addr(&sin, addr.c_str(), sock)) {
        return false;
      }

      retval = sendto(sock->fd(), buf.data(), (size_t)len, flags,
                      (struct sockaddr *)&sin, sizeof(sin));
    }
    break;
  case AF_INET6:
    {
      if (port == -1) {
        throw_missing_arguments_nr("socket_sendto", 6, 5);
        return false;
      }

      struct sockaddr_in6  sin6;
      memset(&sin6, 0, sizeof(sin6));
      sin6.sin6_family = AF_INET6;
      sin6.sin6_port = htons((unsigned short) port);

      if (!php_set_inet6_addr(&sin6, addr.c_str(), sock)) {
        return false;
      }

      retval = sendto(sock->fd(), buf.data(), (size_t)len, flags,
                      (struct sockaddr *)&sin6, sizeof(sin6));
    }
    break;
  default:
    raise_warning("Unsupported socket type %d", sock->getType());
    return false;
  }

  if (retval == -1) {
    SOCKET_ERROR(sock, "unable to write to socket", errno);
    return false;
  }

  return retval;
}