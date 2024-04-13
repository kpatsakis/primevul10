process_cmd_dns(const char *line)
{
  if (!strcmp(line, "-46")) {
    DNS_SetAddressFamily(IPADDR_UNSPEC);
  } else if (!strcmp(line, "-4")) {
    DNS_SetAddressFamily(IPADDR_INET4);
  } else if (!strcmp(line, "-6")) {
    DNS_SetAddressFamily(IPADDR_INET6);
  } else if (!strcmp(line, "-n")) {
    no_dns = 1;
  } else if (!strcmp(line, "+n")) {
    no_dns = 0;
  } else {
    fprintf(stderr, "Unrecognized dns command\n");
    return 0;
  }
  return 1;
}