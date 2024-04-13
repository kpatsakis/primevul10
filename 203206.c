process_cmd_makestep(CMD_Request *msg, char *line)
{
  msg->command = htons(REQ_MAKESTEP);
}