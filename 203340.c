handle_modify_minstratum(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int status;
  IPAddr address;
  UTI_IPNetworkToHost(&rx_message->data.modify_minpoll.address, &address);
  status = NSR_ModifyMinstratum(&address,
                             ntohl(rx_message->data.modify_minstratum.new_min_stratum));
  
  if (status) {
    tx_message->status = htons(STT_SUCCESS);
  } else {
    tx_message->status = htons(STT_NOSUCHSOURCE);
  }
}