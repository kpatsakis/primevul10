process_cmd_add_server(CMD_Request *msg, char *line)
{
  msg->command = htons(REQ_ADD_SERVER);
  return process_cmd_add_server_or_peer(msg, line);
}