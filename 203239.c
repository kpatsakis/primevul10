process_cmd_local(CMD_Request *msg, const char *line)
{
  const char *p;
  int stratum;

  p = line;
  
  if (!*p) {
    return 0;
  } else if (!strcmp(p, "off")) {
    msg->data.local.on_off = htonl(0);
    msg->data.local.stratum = htonl(0);
  } else if (sscanf(p, "stratum%d", &stratum) == 1) {
    msg->data.local.on_off = htonl(1);
    msg->data.local.stratum = htonl(stratum);
  } else {
    fprintf(stderr, "Invalid syntax for local command\n");
    return 0;
  }

  msg->command = htons(REQ_LOCAL);
  return 1;
}