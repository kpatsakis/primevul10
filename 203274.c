process_cmd_add_peer(CMD_Request *msg, char *line)
{
  msg->command = htons(REQ_ADD_PEER);
  return process_cmd_add_server_or_peer(msg, line);
}