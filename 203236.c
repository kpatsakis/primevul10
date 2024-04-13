read_mask_address(char *line, IPAddr *mask, IPAddr *address)
{
  unsigned int bits;
  char *p, *q;

  p = line;
  if (!*p) {
    mask->family = address->family = IPADDR_UNSPEC;
    return 1;
  } else {
    q = strchr(p, '/');
    if (q) {
      *q++ = 0;
      if (UTI_StringToIP(p, mask)) {
        p = q;
        if (UTI_StringToIP(p, address)) {
          if (address->family == mask->family)
            return 1;
        } else if (sscanf(p, "%u", &bits) == 1) {
          *address = *mask;
          bits_to_mask(bits, address->family, mask);
          return 1;
        }
      }
    } else {
      if (DNS_Name2IPAddress(p, address) == DNS_Success) {
        bits_to_mask(-1, address->family, mask);
        return 1;
      } else {
        fprintf(stderr, "Could not get address for hostname\n");
        return 0;
      }
    }
  }

  fprintf(stderr, "Invalid syntax for mask/address\n");
  return 0;
}