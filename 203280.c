handle_modify_maxpoll(CMD_Request *rx_message, CMD_Reply *tx_message)
{
  int status;
  IPAddr address;
  UTI_IPNetworkToHost(&rx_message->data.modify_minpoll.address, &address);
  status = NSR_ModifyMaxpoll(&address,
                             ntohl(rx_message->data.modify_minpoll.new_minpoll));
  
  if (status) {
    tx_message->status = htons(STT_SUCCESS);
  } else {
    tx_message->status = htons(STT_NOSUCHSOURCE);
  }
}