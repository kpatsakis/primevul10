Variant HHVM_FUNCTION(socket_get_option,
                      const Resource& socket,
                      int level,
                      int optname) {
  auto sock = cast<Socket>(socket);
  socklen_t optlen;

  switch (optname) {
  case SO_LINGER:
    {
      struct linger linger_val;
      optlen = sizeof(linger_val);
      if (getsockopt(sock->fd(), level, optname, (char*)&linger_val,
                     &optlen) != 0) {
        SOCKET_ERROR(sock, "unable to retrieve socket option", errno);
        return false;
      }

      return make_darray(
        s_l_onoff, linger_val.l_onoff,
        s_l_linger, linger_val.l_linger
      );
    }
    break;

  case SO_RCVTIMEO:
  case SO_SNDTIMEO:
    {
      struct timeval tv;
      optlen = sizeof(tv);
      if (getsockopt(sock->fd(), level, optname, (char*)&tv, &optlen) != 0) {
        SOCKET_ERROR(sock, "unable to retrieve socket option", errno);
        return false;
      }
      return make_darray(
        s_sec,  (int)tv.tv_sec,
        s_usec, (int)tv.tv_usec
      );
    }
    break;

  default:
    {
      int other_val;
      optlen = sizeof(other_val);
      if (getsockopt(sock->fd(), level, optname, (char*)&other_val, &optlen)) {
        SOCKET_ERROR(sock, "unable to retrieve socket option", errno);
        return false;
      }
      return other_val;
    }
  }
  not_reached();
}