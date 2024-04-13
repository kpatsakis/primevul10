accheck_getaddr(char *line, IPAddr *addr)
{
  unsigned long a, b, c, d;
  IPAddr ip;
  char *p;
  p = line;
  if (!*p) {
    return 0;
  } else {
    if (sscanf(p, "%lu.%lu.%lu.%lu", &a, &b, &c, &d) == 4) {
      addr->family = IPADDR_INET4;
      addr->addr.in4 = (a<<24) | (b<<16) | (c<<8) | d;
      return 1;
    } else {
      if (DNS_Name2IPAddress(p, &ip) != DNS_Success) {
        return 0;
      } else {
        *addr = ip;
        return 1;
      }
    }
  }
}