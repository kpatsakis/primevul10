static bool get_sockaddr(sockaddr *sa, socklen_t salen,
                         Variant &address, Variant &port) {
  switch (sa->sa_family) {
  case AF_INET6:
    {
      struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
      try {
        folly::SocketAddress addr;
        addr.setFromSockaddr(sin6);

        address = String(addr.getAddressStr(), CopyString);
        port = addr.getPort();
      } catch (...) {
        address = s_2colons;
        port = 0;
      }
    }
    return true;
  case AF_INET:
    {
      struct sockaddr_in *sin = (struct sockaddr_in *)sa;
      try {
        folly::SocketAddress addr;
        addr.setFromSockaddr(sin);

        address = String(addr.getAddressStr(), CopyString);
        port = addr.getPort();
      } catch (...) {
        address = s_0_0_0_0;
        port = 0;
      }
    }
    return true;
  case AF_UNIX:
    {
#ifdef _MSC_VER
      address = String("Unsupported");
#else
      // NB: an unnamed socket has no path, and sun_path should not be
      // inspected. In that case the length is just the size of the
      // struct without sun_path.
      struct sockaddr_un *s_un = (struct sockaddr_un *)sa;
      if (salen > offsetof(sockaddr_un, sun_path)) {
        // - `sun_path` MAY have trailing nulls
        // - `sun_len` MAY include that trailing null on Linux.
        const auto max_path_len = salen - offsetof(struct sockaddr_un, sun_path);
        const auto actual_path_len = ::strnlen(s_un->sun_path, max_path_len);
        address = String(s_un->sun_path, actual_path_len, CopyString);
      } else {
        address = empty_string();
      }
#endif
    }
    return true;

  default:
    break;
  }

  raise_warning("Unsupported address family %d", sa->sa_family);
  return false;
}