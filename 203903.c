NCR_AddAccessRestriction(IPAddr *ip_addr, int subnet_bits, int allow, int all)
 {
  ADF_Status status;

  if (allow) {
    if (all) {
      status = ADF_AllowAll(access_auth_table, ip_addr, subnet_bits);
    } else {
      status = ADF_Allow(access_auth_table, ip_addr, subnet_bits);
    }
  } else {
    if (all) {
      status = ADF_DenyAll(access_auth_table, ip_addr, subnet_bits);
    } else {
      status = ADF_Deny(access_auth_table, ip_addr, subnet_bits);
    }
  }

  if (status != ADF_SUCCESS)
    return 0;

  /* Keep server sockets open only when an address allowed */
  if (allow) {
    NTP_Remote_Address remote_addr;

    if (server_sock_fd4 == INVALID_SOCK_FD &&
        ADF_IsAnyAllowed(access_auth_table, IPADDR_INET4)) {
      remote_addr.ip_addr.family = IPADDR_INET4;
      server_sock_fd4 = NIO_OpenServerSocket(&remote_addr);
    }
    if (server_sock_fd6 == INVALID_SOCK_FD &&
        ADF_IsAnyAllowed(access_auth_table, IPADDR_INET6)) {
      remote_addr.ip_addr.family = IPADDR_INET6;
      server_sock_fd6 = NIO_OpenServerSocket(&remote_addr);
    }
  } else {
    if (server_sock_fd4 != INVALID_SOCK_FD &&
        !ADF_IsAnyAllowed(access_auth_table, IPADDR_INET4)) {
      NIO_CloseServerSocket(server_sock_fd4);
      server_sock_fd4 = INVALID_SOCK_FD;
    }
    if (server_sock_fd6 != INVALID_SOCK_FD &&
        !ADF_IsAnyAllowed(access_auth_table, IPADDR_INET6)) {
      NIO_CloseServerSocket(server_sock_fd6);
      server_sock_fd6 = INVALID_SOCK_FD;
    }
  }

  return 1;
}