process_cmd_delete(CMD_Request *msg, char *line)
{
  char *hostname;
  int ok = 0;
  IPAddr address;

  msg->command = htons(REQ_DEL_SOURCE);
  hostname = line;
  CPS_SplitWord(line);

  if (!*hostname) {
    fprintf(stderr, "Invalid syntax for address\n");
    ok = 0;
  } else {
    if (DNS_Name2IPAddress(hostname, &address) != DNS_Success) {
      fprintf(stderr, "Could not get address for hostname\n");
      ok = 0;
    } else {
      UTI_IPHostToNetwork(&address, &msg->data.del_source.ip_addr);
      ok = 1;
    }
  }

  return ok;
  
}