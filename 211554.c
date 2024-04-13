static bool php_set_inet6_addr(struct sockaddr_in6 *sin6,
                               const char *address,
                               req::ptr<Socket> sock) {
  struct in6_addr tmp;
  struct addrinfo hints;
  struct addrinfo *addrinfo = NULL;

  if (inet_pton(AF_INET6, address, &tmp)) {
    memcpy(&(sin6->sin6_addr.s6_addr), &(tmp.s6_addr),
           sizeof(struct in6_addr));
  } else {
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = PF_INET6;
    getaddrinfo(address, NULL, &hints, &addrinfo);
    if (!addrinfo) {
      // 10000 is a magic value to indicate a host error.
      SOCKET_ERROR(sock, "Host lookup failed", (-10000 - h_errno));
      return false;
    }
    if (addrinfo->ai_family != PF_INET6 ||
        addrinfo->ai_addrlen != sizeof(struct sockaddr_in6)) {
      raise_warning("Host lookup failed: Non AF_INET6 domain "
                      "returned on AF_INET6 socket");
      freeaddrinfo(addrinfo);
      return false;
    }

    memcpy(&(sin6->sin6_addr.s6_addr),
           ((struct sockaddr_in6*)(addrinfo->ai_addr))->sin6_addr.s6_addr,
           sizeof(struct in6_addr));
    freeaddrinfo(addrinfo);
  }

  const char *scope = strchr(address, '%');
  if (scope++) {
    int64_t lval = 0;
    double dval = 0;
    unsigned scope_id = 0;
    if (KindOfInt64 == is_numeric_string(scope, strlen(scope), &lval, &dval,
                                         0)) {
      if (lval > 0 && lval <= UINT_MAX) {
        scope_id = lval;
      }
    } else {
      php_string_to_if_index(scope, &scope_id);
    }

    sin6->sin6_scope_id = scope_id;
  }

  return true;
}