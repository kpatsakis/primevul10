process_cmd_polltarget(CMD_Request *msg, char *line)
{
  IPAddr address;
  int poll_target;
  int ok;
  
  if (read_address_integer(line, &address, &poll_target)) {
    UTI_IPHostToNetwork(&address, &msg->data.modify_polltarget.address);
    msg->data.modify_polltarget.new_poll_target = htonl(poll_target);
    msg->command = htons(REQ_MODIFY_POLLTARGET);
    ok = 1;
  } else {
    ok = 0;
  }

  return ok;

}