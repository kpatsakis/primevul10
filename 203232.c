handle_del_source(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  NTP_Remote_Address rem_addr;
  NSR_Status status;
  
  UTI_IPNetworkToHost(&rx_message->data.del_source.ip_addr, &rem_addr.ip_addr);
  rem_addr.local_ip_addr.family = IPADDR_UNSPEC;
  rem_addr.port = 0;
  
  status = NSR_RemoveSource(&rem_addr);
  switch (status) {
    case NSR_Success:
      tx_message->status = htons(STT_SUCCESS);
      break;
    case NSR_NoSuchSource:
      tx_message->status = htons(STT_NOSUCHSOURCE);
      break;
    case NSR_TooManySources:
    case NSR_AlreadyInUse:
    case NSR_InvalidAF:
      assert(0);
      break;
  }
}