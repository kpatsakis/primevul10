process_cmd_dump(CMD_Request *msg, char *line)
{
  msg->command = htons(REQ_DUMP);
  msg->data.dump.pad = htonl(0);
}