read_address_integer(char *line, IPAddr *address, int *value)
{
  char *hostname;
  int ok = 0;

  hostname = line;
  line = CPS_SplitWord(line);

  if (sscanf(line, "%d", value) != 1) {
    fprintf(stderr, "Invalid syntax for address value\n");
    ok = 0;
  } else {
    if (DNS_Name2IPAddress(hostname, address) != DNS_Success) {
      fprintf(stderr, "Could not get address for hostname\n");
      ok = 0;
    } else {
      ok = 1;
    }
  }

  return ok;

}