process_cmd_maxdelaydevratio(CMD_Request *msg, char *line)
{
  IPAddr address;
  double max_delay_dev_ratio;
  int ok;
  
  if (read_address_double(line, &address, &max_delay_dev_ratio)) {
    UTI_IPHostToNetwork(&address, &msg->data.modify_maxdelaydevratio.address);
    msg->data.modify_maxdelayratio.new_max_delay_ratio = UTI_FloatHostToNetwork(max_delay_dev_ratio);
    msg->command = htons(REQ_MODIFY_MAXDELAYDEVRATIO);
    ok = 1;
  } else {
    ok = 0;
  }

  return ok;

}