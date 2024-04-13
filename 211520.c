static bool php_set_inet_addr(struct sockaddr_in *sin,
                              const char *address,
                              req::ptr<Socket> sock) {
  struct in_addr tmp;

  if (inet_aton(address, &tmp)) {
    sin->sin_addr.s_addr = tmp.s_addr;
  } else {
    HostEnt result;
    if (!safe_gethostbyname(address, result)) {
      /* Note: < -10000 indicates a host lookup error */
      SOCKET_ERROR(sock, "Host lookup failed", (-10000 - result.herr));
      return false;
    }
    if (result.hostbuf.h_addrtype != AF_INET) {
      raise_warning("Host lookup failed: Non AF_INET domain "
                      "returned on AF_INET socket");
      return false;
    }
    memcpy(&(sin->sin_addr.s_addr), result.hostbuf.h_addr_list[0],
           result.hostbuf.h_length);
  }

  return true;
}