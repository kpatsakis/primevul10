process_cmd_minstratum(CMD_Request *msg, char *line)
{
  IPAddr address;
  int min_stratum;
  int ok;
  
  if (read_address_integer(line, &address, &min_stratum)) {
    UTI_IPHostToNetwork(&address, &msg->data.modify_minstratum.address);
    msg->data.modify_minstratum.new_min_stratum = htonl(min_stratum);
    msg->command = htons(REQ_MODIFY_MINSTRATUM);
    ok = 1;
  } else {
    ok = 0;
  }

  return ok;

}