prepare_socket(int family)
{
  int port_number, sock_fd;
  socklen_t my_addr_len;
  union sockaddr_in46 my_addr;
  IPAddr bind_address;
  int on_off = 1;

  port_number = CNF_GetCommandPort();

  sock_fd = socket(family, SOCK_DGRAM, 0);
  if (sock_fd < 0) {
    LOG(LOGS_ERR, LOGF_CmdMon, "Could not open %s command socket : %s",
        family == AF_INET ? "IPv4" : "IPv6", strerror(errno));
    return -1;
  }

  /* Close on exec */
  UTI_FdSetCloexec(sock_fd);

  /* Allow reuse of port number */
  if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on_off, sizeof(on_off)) < 0) {
    LOG(LOGS_ERR, LOGF_CmdMon, "Could not set reuseaddr socket options");
    /* Don't quit - we might survive anyway */
  }
#ifdef HAVE_IPV6
  if (family == AF_INET6) {
#ifdef IPV6_V6ONLY
    /* Receive IPv6 packets only */
    if (setsockopt(sock_fd, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&on_off, sizeof(on_off)) < 0) {
      LOG(LOGS_ERR, LOGF_CmdMon, "Could not request IPV6_V6ONLY socket option");
    }
#endif
  }
#endif

  memset(&my_addr, 0, sizeof (my_addr));

  switch (family) {
    case AF_INET:
      my_addr_len = sizeof (my_addr.in4);
      my_addr.in4.sin_family = family;
      my_addr.in4.sin_port = htons((unsigned short)port_number);

      CNF_GetBindCommandAddress(IPADDR_INET4, &bind_address);

      if (bind_address.family == IPADDR_INET4)
        my_addr.in4.sin_addr.s_addr = htonl(bind_address.addr.in4);
      else
        my_addr.in4.sin_addr.s_addr = htonl(INADDR_ANY);
      break;
#ifdef HAVE_IPV6
    case AF_INET6:
      my_addr_len = sizeof (my_addr.in6);
      my_addr.in6.sin6_family = family;
      my_addr.in6.sin6_port = htons((unsigned short)port_number);

      CNF_GetBindCommandAddress(IPADDR_INET6, &bind_address);

      if (bind_address.family == IPADDR_INET6)
        memcpy(my_addr.in6.sin6_addr.s6_addr, bind_address.addr.in6,
            sizeof (my_addr.in6.sin6_addr.s6_addr));
      else
        my_addr.in6.sin6_addr = in6addr_any;
      break;
#endif
    default:
      assert(0);
  }

  if (bind(sock_fd, &my_addr.u, my_addr_len) < 0) {
    LOG(LOGS_ERR, LOGF_CmdMon, "Could not bind %s command socket : %s",
        family == AF_INET ? "IPv4" : "IPv6", strerror(errno));
    close(sock_fd);
    return -1;
  }

  /* Register handler for read events on the socket */
  SCH_AddInputFileHandler(sock_fd, read_from_cmd_socket, (void *)(long)sock_fd);

  return sock_fd;
}