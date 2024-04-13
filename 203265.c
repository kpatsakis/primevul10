handle_add_source(NTP_Source_Type type, CMD_Request *rx_message, CMD_Reply *tx_message)
{
  NTP_Remote_Address rem_addr;
  SourceParameters params;
  NSR_Status status;
  
  UTI_IPNetworkToHost(&rx_message->data.ntp_source.ip_addr, &rem_addr.ip_addr);
  rem_addr.local_ip_addr.family = IPADDR_UNSPEC;
  rem_addr.port = (unsigned short)(ntohl(rx_message->data.ntp_source.port));
  params.minpoll = ntohl(rx_message->data.ntp_source.minpoll);
  params.maxpoll = ntohl(rx_message->data.ntp_source.maxpoll);
  params.presend_minpoll = ntohl(rx_message->data.ntp_source.presend_minpoll);
  params.authkey = ntohl(rx_message->data.ntp_source.authkey);
  params.online  = ntohl(rx_message->data.ntp_source.flags) & REQ_ADDSRC_ONLINE ? 1 : 0;
  params.auto_offline = ntohl(rx_message->data.ntp_source.flags) & REQ_ADDSRC_AUTOOFFLINE ? 1 : 0;
  params.iburst = ntohl(rx_message->data.ntp_source.flags) & REQ_ADDSRC_IBURST ? 1 : 0;
  params.sel_option = ntohl(rx_message->data.ntp_source.flags) & REQ_ADDSRC_PREFER ? SRC_SelectPrefer :
                      ntohl(rx_message->data.ntp_source.flags) & REQ_ADDSRC_NOSELECT ? SRC_SelectNoselect : SRC_SelectNormal;
  params.max_delay = UTI_FloatNetworkToHost(rx_message->data.ntp_source.max_delay);
  params.max_delay_ratio = UTI_FloatNetworkToHost(rx_message->data.ntp_source.max_delay_ratio);

 /* not transmitted in cmdmon protocol yet */
  params.min_stratum = SRC_DEFAULT_MINSTRATUM;       
  params.poll_target = SRC_DEFAULT_POLLTARGET;
  params.max_delay_dev_ratio = SRC_DEFAULT_MAXDELAYDEVRATIO;

  status = NSR_AddSource(&rem_addr, type, &params);
  switch (status) {
    case NSR_Success:
      tx_message->status = htons(STT_SUCCESS);
      break;
    case NSR_AlreadyInUse:
      tx_message->status = htons(STT_SOURCEALREADYKNOWN);
      break;
    case NSR_TooManySources:
      tx_message->status = htons(STT_TOOMANYSOURCES);
      break;
    case NSR_InvalidAF:
      tx_message->status = htons(STT_INVALIDAF);
      break;
    case NSR_NoSuchSource:
      assert(0);
      break;
  }
}