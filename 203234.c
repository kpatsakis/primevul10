process_cmd_accheck(CMD_Request *msg, char *line)
{
  IPAddr ip;
  msg->command = htons(REQ_ACCHECK);
  if (accheck_getaddr(line, &ip)) {
    UTI_IPHostToNetwork(&ip, &msg->data.ac_check.ip);
    return 1;
  } else {    
    fprintf(stderr, "Could not read address\n");
    return 0;
  }
}