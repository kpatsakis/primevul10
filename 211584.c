Variant HHVM_FUNCTION(getaddrinfo,
                      const String& host,
                      const String& port,
                      int family /* = 0 */,
                      int socktype /* = 0 */,
                      int protocol /* = 0 */,
                      int flags /* = 0 */) {
  const char *hptr = NULL, *pptr = NULL;
  if (!host.empty()) {
    hptr = host.c_str();
  }
  if (!port.empty()) {
    pptr = port.c_str();
  }

  struct addrinfo hints, *res;
  struct addrinfo *res0 = NULL;
  int error;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = family;
  hints.ai_socktype = socktype;
  hints.ai_protocol = protocol;
  hints.ai_flags = flags;
  error = getaddrinfo(hptr, pptr, &hints, &res0);

  if (error) {
    raise_warning("%s", gai_strerror(error));

    if (res0) {
      freeaddrinfo(res0);
    }
    return false;
  }

  Array ret = Array::CreateVArray();

  for (res = res0; res; res = res->ai_next) {
    Array data = make_darray(
      s_family, res->ai_family,
      s_socktype, res->ai_socktype,
      s_protocol, res->ai_protocol
    );

    switch (res->ai_addr->sa_family) {
      case AF_INET:
      {
        struct sockaddr_in *a;
        String buffer = ipaddr_convert(res->ai_addr, sizeof(*a));
        if (!buffer.empty()) {
          a = (struct sockaddr_in *)res->ai_addr;
          data.set(
            s_sockaddr,
            make_darray(
              s_address, buffer,
              s_port, ntohs(a->sin_port)
            )
          );
        }
        break;
      }
      case AF_INET6:
      {
        struct sockaddr_in6 *a;
        String buffer = ipaddr_convert(res->ai_addr, sizeof(*a));
        if (!buffer.empty()) {
          a = (struct sockaddr_in6 *)res->ai_addr;
          data.set(
            s_sockaddr,
            make_darray(
              s_address, buffer,
              s_port, ntohs(a->sin6_port),
              s_flow_info, (int32_t)a->sin6_flowinfo,
              s_scope_id, (int32_t)a->sin6_scope_id
            )
          );
        }
        break;
      }
      default:
        data.set(s_sockaddr, empty_array());
        break;
    }

    ret.append(data);
  }

  if (res0) {
    freeaddrinfo(res0);
  }

  return ret;
}