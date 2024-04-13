process_cmd_add_server_or_peer(CMD_Request *msg, char *line)
{
  CPS_NTP_Source data;
  CPS_Status status;
  IPAddr ip_addr;
  int result = 0;
  
  status = CPS_ParseNTPSourceAdd(line, &data);
  switch (status) {
    case CPS_Success:
      if (DNS_Name2IPAddress(data.name, &ip_addr) != DNS_Success) {
        Free(data.name);
        fprintf(stderr, "Invalid host/IP address\n");
        break;
      }
      Free(data.name);

      if (data.params.min_stratum != SRC_DEFAULT_MINSTRATUM) {
        fprintf(stderr, "Option minstratum not supported\n");
        break;
      }

      if (data.params.poll_target != SRC_DEFAULT_POLLTARGET) {
        fprintf(stderr, "Option polltarget not supported\n");
        break;
      }

      if (data.params.max_delay_dev_ratio != SRC_DEFAULT_MAXDELAYDEVRATIO) {
        fprintf(stderr, "Option maxdelaydevratio not supported\n");
        break;
      }

      msg->data.ntp_source.port = htonl((unsigned long) data.port);
      UTI_IPHostToNetwork(&ip_addr, &msg->data.ntp_source.ip_addr);
      msg->data.ntp_source.minpoll = htonl(data.params.minpoll);
      msg->data.ntp_source.maxpoll = htonl(data.params.maxpoll);
      msg->data.ntp_source.presend_minpoll = htonl(data.params.presend_minpoll);
      msg->data.ntp_source.authkey = htonl(data.params.authkey);
      msg->data.ntp_source.max_delay = UTI_FloatHostToNetwork(data.params.max_delay);
      msg->data.ntp_source.max_delay_ratio = UTI_FloatHostToNetwork(data.params.max_delay_ratio);
      msg->data.ntp_source.flags = htonl(
          (data.params.online ? REQ_ADDSRC_ONLINE : 0) |
          (data.params.auto_offline ? REQ_ADDSRC_AUTOOFFLINE : 0) |
          (data.params.iburst ? REQ_ADDSRC_IBURST : 0) |
          (data.params.sel_option == SRC_SelectPrefer ? REQ_ADDSRC_PREFER : 0) |
          (data.params.sel_option == SRC_SelectNoselect ? REQ_ADDSRC_NOSELECT : 0));
      result = 1;

      break;
    case CPS_BadOption:
      fprintf(stderr, "Unrecognized subcommand\n");
      break;
    case CPS_BadHost:
      fprintf(stderr, "Invalid host/IP address\n");
      break;
    case CPS_BadPort:
      fprintf(stderr, "Unreadable port number\n");
      break;
    case CPS_BadMinpoll:
      fprintf(stderr, "Unreadable minpoll value\n");
      break;
    case CPS_BadMaxpoll:
      fprintf(stderr, "Unreadable maxpoll value\n");
      break;
    case CPS_BadPresend:
      fprintf(stderr, "Unreadable presend value\n");
      break;
    case CPS_BadMaxdelaydevratio:
      fprintf(stderr, "Unreadable max delay dev ratio value\n");
      break;
    case CPS_BadMaxdelayratio:
      fprintf(stderr, "Unreadable max delay ratio value\n");
      break;
    case CPS_BadMaxdelay:
      fprintf(stderr, "Unreadable max delay value\n");
      break;
    case CPS_BadKey:
      fprintf(stderr, "Unreadable key value\n");
      break;
    case CPS_BadMinstratum:
      fprintf(stderr, "Unreadable minstratum value\n");
      break;
    case CPS_BadPolltarget:
      fprintf(stderr, "Unreadable polltarget value\n");
      break;
  }

  return result;
}