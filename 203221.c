process_cmd_reselect(CMD_Request *msg, char *line)
{
  msg->command = htons(REQ_RESELECT);
}