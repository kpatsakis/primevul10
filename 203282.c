transmit_reply(CMD_Reply *msg, union sockaddr_in46 *where_to, int auth_len)
{
  int status;
  int tx_message_length;
  int sock_fd;
  socklen_t addrlen;
  
  switch (where_to->u.sa_family) {
    case AF_INET:
      sock_fd = sock_fd4;
      addrlen = sizeof (where_to->in4);
      break;
#ifdef HAVE_IPV6
    case AF_INET6:
      sock_fd = sock_fd6;
      addrlen = sizeof (where_to->in6);
      break;
#endif
    default:
      assert(0);
  }

  tx_message_length = PKL_ReplyLength(msg) + auth_len;
  status = sendto(sock_fd, (void *) msg, tx_message_length, 0,
                  &where_to->u, addrlen);

  if (status < 0 && !LOG_RateLimited()) {
    unsigned short port;
    IPAddr ip;

    switch (where_to->u.sa_family) {
      case AF_INET:
        ip.family = IPADDR_INET4;
        ip.addr.in4 = ntohl(where_to->in4.sin_addr.s_addr);
        port = ntohs(where_to->in4.sin_port);
        break;
#ifdef HAVE_IPV6
      case AF_INET6:
        ip.family = IPADDR_INET6;
        memcpy(ip.addr.in6, (where_to->in6.sin6_addr.s6_addr), sizeof(ip.addr.in6));
        port = ntohs(where_to->in6.sin6_port);
        break;
#endif
      default:
        assert(0);
    }

    LOG(LOGS_WARN, LOGF_CmdMon, "Could not send response to %s:%hu", UTI_IPToString(&ip), port);
  }
}