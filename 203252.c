open_io(const char *hostname, int port)
{
  IPAddr ip;

  /* Note, this call could block for a while */
  if (DNS_Name2IPAddress(hostname, &ip) != DNS_Success) {
    fprintf(stderr, "Could not get IP address for %s\n", hostname);
    exit(1);
  }

  memset(&his_addr, 0, sizeof (his_addr));

  switch (ip.family) {
    case IPADDR_INET4:
      sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

      his_addr.in4.sin_family = AF_INET;
      his_addr.in4.sin_addr.s_addr = htonl(ip.addr.in4);
      his_addr.in4.sin_port = htons(port);
      his_addr_len = sizeof (his_addr.in4);
      break;
#ifdef HAVE_IPV6
    case IPADDR_INET6:
      sock_fd = socket(AF_INET6, SOCK_DGRAM, 0);

      his_addr.in6.sin6_family = AF_INET6;
      memcpy(his_addr.in6.sin6_addr.s6_addr, ip.addr.in6,
          sizeof (his_addr.in6.sin6_addr.s6_addr));
      his_addr.in6.sin6_port = htons(port);
      his_addr_len = sizeof (his_addr.in6);
      break;
#endif
    default:
      assert(0);
  }

  if (sock_fd < 0) {
    perror("Can't create socket");
    exit(1);
  }
}