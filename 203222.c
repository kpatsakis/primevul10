process_cmd_manual(CMD_Request *msg, const char *line)
{
  const char *p;

  p = line;

  if (!*p) {
    return 0;
  } else if (!strcmp(p, "off")) {
    msg->data.manual.option = htonl(0);
  } else if (!strcmp(p, "on")) {
    msg->data.manual.option = htonl(1);
  } else if (!strcmp(p, "reset")) {
    msg->data.manual.option = htonl(2);
  } else {
    return 0;
  }
  msg->command = htons(REQ_MANUAL);

  return 1;
}