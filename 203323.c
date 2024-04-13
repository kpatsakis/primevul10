parse_allow_deny(CMD_Request *msg, char *line)
{
  unsigned long a, b, c, d, n;
  IPAddr ip;
  char *p;
  
  p = line;
  if (!*p) {
    /* blank line - applies to all addresses */
    ip.family = IPADDR_UNSPEC;
    UTI_IPHostToNetwork(&ip, &msg->data.allow_deny.ip);
    msg->data.allow_deny.subnet_bits = htonl(0);
  } else {
    char *slashpos;
    slashpos = strchr(p, '/');
    if (slashpos) *slashpos = 0;
    
    n = 0;
    if (!UTI_StringToIP(p, &ip) &&
        (n = sscanf(p, "%lu.%lu.%lu.%lu", &a, &b, &c, &d)) == 0) {

      /* Try to parse as the name of a machine */
      if (DNS_Name2IPAddress(p, &ip) != DNS_Success) {
        fprintf(stderr, "Could not read address\n");
        return 0;
      } else {
        UTI_IPHostToNetwork(&ip, &msg->data.allow_deny.ip);
        if (ip.family == IPADDR_INET6)
          msg->data.allow_deny.subnet_bits = htonl(128);
        else
          msg->data.allow_deny.subnet_bits = htonl(32);
      }        
    } else {
      
      if (n == 0) {
        if (ip.family == IPADDR_INET6)
          msg->data.allow_deny.subnet_bits = htonl(128);
        else
          msg->data.allow_deny.subnet_bits = htonl(32);
      } else {
        ip.family = IPADDR_INET4;

        a &= 0xff;
        b &= 0xff;
        c &= 0xff;
        d &= 0xff;
        
        switch (n) {
          case 1:
            ip.addr.in4 = htonl((a<<24));
            msg->data.allow_deny.subnet_bits = htonl(8);
            break;
          case 2:
            ip.addr.in4 = htonl((a<<24) | (b<<16));
            msg->data.allow_deny.subnet_bits = htonl(16);
            break;
          case 3:
            ip.addr.in4 = htonl((a<<24) | (b<<16) | (c<<8));
            msg->data.allow_deny.subnet_bits = htonl(24);
            break;
          case 4:
            ip.addr.in4 = htonl((a<<24) | (b<<16) | (c<<8) | d);
            msg->data.allow_deny.subnet_bits = htonl(32);
            break;
          default:
            assert(0);
        }
      }

      UTI_IPHostToNetwork(&ip, &msg->data.allow_deny.ip);

      if (slashpos) {
        int specified_subnet_bits, n;
        n = sscanf(slashpos+1, "%d", &specified_subnet_bits);
        if (n == 1) {
          msg->data.allow_deny.subnet_bits = htonl(specified_subnet_bits);
        } else {
          fprintf(stderr, "Warning: badly formatted subnet size, using %ld\n", (long) ntohl(msg->data.allow_deny.subnet_bits));
        }
      } 
    }
  }
  return 1;
}